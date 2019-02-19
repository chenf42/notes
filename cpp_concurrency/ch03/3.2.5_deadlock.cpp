#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <algorithm>
#include <functional>
#include <iostream>

class Foo {
    std::mutex l_;
    std::mutex r_;

public:
    void Do() {
        //std::lock_guard<std::mutex> lg(l_);
        //std::lock_guard<std::mutex> lg2(r_);

        // 标准库提供了 std::lock 来锁住多个对象。std::lock 处理了多个锁的顺序问题，
        // 我们按任意顺序传送也不会死锁。
        std::lock(l_, r_);
        l_.unlock();
        r_.unlock();
    }
    void Do2() {
        // 若 Do2() 加锁顺序与 Do() 不一致，后面就会发生死锁
        //std::lock_guard<std::mutex> lg2(r_);
        //std::lock_guard<std::mutex> lg(l_);

        std::lock(r_, l_);
        l_.unlock();
        r_.unlock();
    }
};

int main(int argc, char **argv) {
    std::vector<std::thread> tv;
    Foo f;
    for (auto i = 0u; i < std::thread::hardware_concurrency(); i++) {
        tv.push_back(std::thread([&f, i](){
            for (int k = 0; k < 1000; k++) {
                if (i % 2 == 0)
                    f.Do();
                else
                    f.Do2();
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(1ms);
            }
        }));
    }
    std::cout << "total threads: " << tv.size() << '\n';

    std::for_each(tv.begin(), tv.end(), std::mem_fn(&std::thread::join));
    return 0;
}
