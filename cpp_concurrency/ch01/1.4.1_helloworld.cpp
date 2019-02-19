#include <thread>
#include <iostream>

void hello() {
    std::cout << "hello world\n";
}

int main(int argc, char **argv) {
    std::thread t(hello);
    t.join();
    return 0;
}
