#include <iostream>

class C;
class B{
    int _bd;
};
class A{
    B  _b;
    C *_c; // legal , because the size ptr is arbitrary 
};

int main(){
    A a; 
}
























