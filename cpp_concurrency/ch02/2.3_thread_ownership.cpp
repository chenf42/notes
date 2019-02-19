#include <thread>
#include <iostream>
#include <chrono>
#include <assert.h>

// 这里并不会发生 copy，而是 move
std::thread f() {
    // 1. NRVO(Named Return Value Optimization)
    std::thread t([](){});
    return t;

    // 2. RVO(Return Value Optimization)
    //return std::thread([](){});
}
// 顺带一提，NRVO/RVO 是 Copy-elision 的一部分。Copy-elision 的另一个例子如下：
class Foo {
public:
    Foo() { std::cout << "ctor\n"; }
    Foo(const Foo &other) { std::cout << "copy\n"; }
};
void Bar(Foo f) {}
void Do() {
    Bar(Foo()); // 这里并不会复制。因为编译器知道它是临时对象，会使用移动。

    // 而下面这种写法就会触发拷贝动作了
    //Foo f;
    //Bar(f);
}

int main(int argc, char **argv) {
    {
        std::thread t1([](){
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1s);
        });

        // std::thread objects 是 non-copyable 的
        //std::thread t2(t1);
        //std::thread t2 =  t1;

        // 只能 move
        std::thread t2 = std::move(t1);

        assert(!t1.joinable());  // move 之后 t1 就不再关联到创建的线程了
        assert(t2.joinable());

        t2.join();
    }

    {
        std::thread t;
        t = std::thread([](){});  // 临时 std::thread object，会隐式地 move
        t.join();

        // 另，所有 move 的目标对象，比如上面的 t，如果已经关联到一个线程，那么
        // 这个线程上会被立即执行 std::terminate
    }

    Do();

    return 0;
}
