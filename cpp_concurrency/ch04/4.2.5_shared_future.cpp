#include <thread>
#include <future>
#include <vector>
#include <chrono>
#include <functional>
#include <algorithm>

int Foo() {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(10s);
    return 42;
}

// TODO: 需要进一步研究
int main(int argc, char **argv) {
    std::vector<std::thread> tv;

    auto f = std::async(Foo);
    //std::shared_future<int> sf(std::move(f));
    auto sf = f.share();
    for (auto i = 0; i < 10; i++) {
        tv.emplace_back(std::thread([sf]() {
            sf.get();
        }));
    }

    std::for_each(tv.begin(), tv.end(), std::mem_fn(&std::thread::join));
    return 0;
}
