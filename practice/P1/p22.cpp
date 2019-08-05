#include <iostream>

void f1(){
    int a,b,c;
    std::cout << &a << std::endl;
    std::cout << &b << std::endl;
    std::cout << &c << std::endl;
}

void f2(){
    int* p1 = new int;
    int* p2 = new int;
    std::cout << &p1 << std::endl;
    std::cout << &p2 << std::endl;
    std::cout << "p1 = " << p1 << std::endl;
    std::cout << "p2 = " << p2 << std::endl;
}
int main(){
    std::cout << "local variable init with static declaration" << std::endl;
    for (int i = 0; i < 3; i++) {
        std::cout << i+1 << "-----" << std::endl;
        f1();
    }
    std::cout << std::endl;
    std::cout << "local variable init with dynamically allocation" << std::endl;
    for (int i = 0; i < 3; ++i) {
        std::cout << i+1 << "-----" << std::endl;
        f2();    
    }
    return 0;
}
