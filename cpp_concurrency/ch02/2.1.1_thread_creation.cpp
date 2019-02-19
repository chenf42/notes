#include <thread>
#include <iostream>

class Functor {
public:
    void operator()() const {
        std::cout << "functor: hello world\n";
    }
};




int main(int argc, char **argv) {
    // 使用 Functor 初始化线程
    {
        // std::thread t(Functor());

        //  按上面这样写，g++ 报错：
        //  error: request for member ‘join’ in ‘t’, 
        //  which is of non-class type ‘std::thread(Functor (*)())’
        
        // 改成下面两种写法之一就 OK 了。具体原因见 main() 函数后的两个 F1 函数定义。
        // ref: https://en.wikipedia.org/wiki/Most_vexing_parse
        //std::thread t((Functor()));
        std::thread t{Functor()};
        t.join();
    }

    // 使用 lambda 表达式初始化线程
    {
        std::thread t([]() {
            std::cout << "lambda: hello world\n";
        });
        t.join();
    }

    return 0;
}

// 下面的代码会报错：
//  error: redefinition of ‘void F1(int (*)())’

// void F1(int()) {  // 匿名的 int(*)() 参数，在 F1 内部实际上无法使用该函数指针
// }
// void F1(int(*f)()) {
// }
