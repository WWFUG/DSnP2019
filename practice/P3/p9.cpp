#include <iostream>
using namespace std;

class Base
{
public:
    void f();
    void g();
private:
    /* data */
};

class Derived:public Base
{    
public:
    void f();
    void g();
private:
    /* data */
};

int main(){
    Base b;
    Derived d;
    b.f();b.g();
    d.f();d.g();
    return 0;
}

void Base::f(){
    cout << "Base::f() called" << endl;
}

void Base::g(){
    cout << "Base::g() called" << endl;
}

void Derived::f(){
    cout << "Derived::f() called" << endl;
}

void Derived::g(){
    cout << "Derived::g() called" << endl;
}














