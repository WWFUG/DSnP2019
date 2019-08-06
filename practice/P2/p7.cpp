#include <iostream>

void foo(int a = 1);

int main(){
    foo(3);
    foo();
}

void foo(int a){
    std::cout << a << std::endl;
}




