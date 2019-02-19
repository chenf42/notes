#include <thread>
#include <future>
#include <iostream>
#include <chrono>
#include <numeric>
#include <vector>

int Add(int a, int b) {
    std::cout << "work thread: " << std::hex << std::this_thread::get_id() << '\n';
    return a + b;
}

void F1() {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(2s);
    std::cout << "about to exit F1()\n";
}

void F2() {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);
    std::cout << "about to exit F2()\n";
}

template <typename Container>
void accumulate(typename Container::const_iterator first, 
                typename Container::const_iterator last,
                std::promise<typename Container::value_type> accumulate_promise) {
    auto result = std::accumulate(first, last, 0);

    // 若这里直接死循环，那么对应 future 的 get 操作就会一直阻塞
    //while (1) {}

    // 因为这个 promise 是 move 过来的，会在函数退出时析构。
    // 若没有下面的 set_value 操作，下面的 future 等待时会发现它依赖的 promise 已经
    // 被销毁了，会抛出 broken promise 异常。
    accumulate_promise.set_value(result);
}

int main(int argc, char **argv) {
    {
        std::cout << "main thread: " << std::hex << std::this_thread::get_id() << '\n';

        // std::launch::async：使用新线程
        // std::launch::deferred：当调用 future::get() 时再执行
        std::future<int> fut = std::async(std::launch::async, Add, 1, 2);

        std::cout << "result: " << fut.get() << '\n';
    }

    // ref: https://stackoverflow.com/questions/18143661/what-is-the-difference-between-packaged-task-and-async
    // 上面的链接说明了 std::async 和 std::packaged_task 的差异
    // 简单来说，可以这样认为： std::async 是基于 std::packaged_task 封装的。如果你只想跑一个任务，而不关心其他东西（在哪个线程执行、返回值等等），
    // 就使用 std::async。否则就使用 std::packaged_task

    {
        std::async(std::launch::async, F1);  // 这里会 block！根本不是真正的 async
        std::async(std::launch::async, F2);
    }

    {
        // 如果保存了返回的 std::future，就是 async 的了
        auto f1 = std::async(std::launch::async, F1);  // 不会 block
        auto f2 = std::async(std::launch::async, F2);

        // 这里通过局部变量，将 ~future() 的动作放到了这里而已
    }

    {
        // 如果这样写，还是会 block。
        // 无论 std::async 还是 std::packaged_task，它们终究要返回一个 std::future，
        // 目前的问题还是源自 std::future 的设计错误
        std::cout << "packaged_task:\n";
        std::packaged_task<void()> task1(F1);
        std::packaged_task<void()> task2(F2);

        task1();
        task2();
    }

    // ref: https://stackoverflow.com/questions/11004273/what-is-stdpromise
    // std::promise 是啥？
    // --
    // std::promise 和 std::packaged_task、std::async 的内部实现一样，是 async 操作的 Provider。
    // 而对应的 std::future 则是 return object。
    //
    // 层级：从高到低（高级的实体可使用低级实体进行构造）
    // std::async -> std::packaged_task -> std::promise

    // std::promise 的基本使用
    // ref: https://zh.cppreference.com/w/cpp/thread/promise
    {
        std::vector<int> nums = {1,2,3,4,5,6,7,8,9,10};
        std::promise<int> accumulate_promise;
        std::future<int> accumulate_future = accumulate_promise.get_future();
        std::thread t(accumulate<std::vector<int>>, nums.begin(), nums.end(), std::move(accumulate_promise));
        std::cout << "future result: " << std::oct << accumulate_future.get() << '\n';
        t.join();
    }

    return 0;
}