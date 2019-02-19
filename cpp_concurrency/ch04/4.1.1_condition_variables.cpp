#include <thread>
#include <mutex>
#include <condition_variable>

// 使用 condition_variable wait 来处理多次发生的事情

int main(int argc, char **argv) {
    bool ready = false;
    std::mutex mutex;
    std::condition_variable cond;

    std::thread t1([&ready, &mutex, &cond](){
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);

        std::unique_lock<std::mutex> ul(mutex);
        ready = true;
        cond.notify_one();
    });

    std::thread t2([&ready, &mutex, &cond](){
        std::unique_lock<std::mutex> ul(mutex);
        cond.wait(ul, [&ready](){ return ready; });
    });

    t1.join();
    t2.join();

    return 0;
}
