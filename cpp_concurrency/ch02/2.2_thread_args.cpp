#include <thread>
#include <iostream>
#include <functional>

int Add(int a, int b) {
    auto result = a + b;
    std::cout << "result: " << result << '\n';
    return result;
}

void Foo(const std::string &s) {
    std::cout << s << '\n';
}

struct FooBar {
    FooBar() {
        std::cout << "FooBar object created\n";
    }
    FooBar(const FooBar &rhs) {
        std::cout << "FooBar object copyed\n";
    }
};

void UpdateFooBar(const FooBar &fb) {

}

class Worker {
public:
    void DoWork() {}
};

void HandleUniquePtr(std::unique_ptr<int> ptr) {

}

int main(int argc, char **argv) {
    {
        std::thread t(Add, 1, 2);
        t.join();
    }

    {
        char buf[100];
        sprintf(buf, "%s", "hello world");
        std::thread t(Foo, buf);  // TODO: 书上说是可能会引发 UB，没搞懂
        t.join();
    }

    {
        FooBar fb;
        // 这里会导致 FooBar 拷贝构造，因为 std::thread 的构造函数并不知晓第 2 个参数是个 ref
        // std::thread t(UpdateFooBar, fb);

        // 应当这样写：
        std::thread t(UpdateFooBar, std::ref(fb));  // TODO: std::ref 实现原理

        t.join();
    }

    // 成员函数
    {
        Worker w;
        std::thread t(&Worker::DoWork, &w);
        t.join();
    }

    // std::move
    {
        auto p = std::unique_ptr<int>(new int);
        std::thread t(HandleUniquePtr, std::move(p));
        t.join();
    }
    return 0;
}
