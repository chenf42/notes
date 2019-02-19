#include <math.h>
#include <thread>
#include <future>
#include <iostream>
#include <chrono>

double SquareRoot(double x) {
    if (x < 0) throw std::out_of_range("x < 0");
    return sqrt(x);
}

double SquareRoot2(double x, std::promise<double> sqrt_promise) {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(2s);

    // 这样不行，后面 future::get() 处无法捕获异常
    // if (x < 0) throw std::out_of_range("x < 0");
    
    // 下面这样可以
    /*
    try {
        if (x < 0) throw std::out_of_range("x < 0");
    } catch (...) {
        sqrt_promise.set_exception(std::current_exception());
        return 0.0;
    }
    */

   // 另一种写法
   if (x < 0) {
       sqrt_promise.set_exception(std::make_exception_ptr(std::out_of_range("x < 0")));
       return 0.0;
   }

    sqrt_promise.set_value(sqrt(x));
}

int main(int argc, char **argv) {
    {
        // 新线程中抛出的异常可以在 future::get() 处捕获到
        try {
            auto f = std::async(SquareRoot, -1);
            auto result = f.get();
            std::cout << "result: " << result << '\n';
        } catch (const std::exception &e) {
            std::cerr << "error: " << e.what() << '\n';
        }
    }

    // std::packaged_task 也是一样的
    {
        std::packaged_task<double(double)> sqrt_task(SquareRoot);
        auto f = sqrt_task.get_future();
        
        // 使用指定的线程来执行
        std::thread t(std::move(sqrt_task), -1);
        t.join();  // 注意这里得手动去 join()

        try {
            auto result = f.get();
            std::cout << "result: " << result << '\n';
        } catch (const std::exception &e) {
            std::cerr << "error: " << e.what() << '\n';
        }
    }

    // std::promise
    {
        std::promise<double> sqrt_promise;
        auto sqrt_future = sqrt_promise.get_future();

        std::thread t(SquareRoot2, -1, std::move(sqrt_promise));
        t.join();  // 这里 detach 也行，反正下面 get() 已经可以当作同步信号了

        try {
            auto result = sqrt_future.get();
            std::cout << "result: " << result << '\n';
        } catch (const std::exception &e) {
            std::cerr << "error: " << e.what() << '\n';
        }
    }
    
    return 0;
}
