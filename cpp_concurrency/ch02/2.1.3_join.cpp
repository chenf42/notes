#include <chrono>
#include <thread>
#include <iostream>
#include <functional>

void throw_error() {
    throw std::runtime_error("oops");
}

class ScopeGuard {
    std::function<void()> on_exit_;
    bool dismissed_;
public:
    ScopeGuard(std::function<void()> on_exit)
        : on_exit_(on_exit),
          dismissed_(false)
    {}

    ~ScopeGuard() {
        if (!dismissed_)
            on_exit_();
    }

    void Dismiss() { dismissed_ = true; }
};

int main(int argc, char **argv) {
    // {
    //     std::thread t([](){
    //         std::cout << "new thread\n";
    //         throw std::runtime_error("oops");
    //     });

    //     using namespace std::chrono_literals;
    //     std::this_thread::sleep_for(10s);
    //     try {
    //         t.join();
    //     } catch (const std::exception &e) {
    //         // 子线程有未捕获的异常，是不会通过 join() 抛到主线程的。
    //         //  这里不会捕获到 std::runtime_error
    //         std::cerr << e.what() << '\n';
    //     }

    //     // 子线程炸了，主线程跟着炸，不会跑到这里
    //     std::cout << "about to exit\n";
    // }

    // 异常安全的 join()
    {
        try {
            std::thread t([](){
                std::cout << "hello world\n";
            });

            try {
                throw_error();
            } catch (...) {
                std::cout << "except catched\n";
                t.join();
                throw;  // 由于这里 throw 了，后面的 join() 被跳过了
            }
            
            std::cout << "about to join\n";
            t.join();
        } catch (const std::exception &e) {
            std::cerr << e.what() << " handled\n";
        }
    }

    // 使用 RAII(scope guard) 进行异常安全的 join()
    {
        try {
            std::thread t([](){
                std::cout << "hello world\n";
            });

            ScopeGuard sg([&t](){
                t.join();
            });

            try {
                throw_error();
            } catch (...) {
                std::cout << "except catched\n";
                //t.join();
                throw;  // 由于这里 throw 了，后面的 join() 被跳过了
            }
            
            std::cout << "about to join\n";
            //t.join();
        } catch (const std::exception &e) {
            std::cerr << e.what() << " handled\n";
        }
    }

    return 0;
}


