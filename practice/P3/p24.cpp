#include <iostream>
using namespace std;

// pure virtual funtion make Base a virtual class 
class Base
{
public:
    virtual void f(){
        cout << "Base::f()" << endl;
    };
    void g(){
        cout << "Base::g()" << endl;
    };
    virtual void h()=0; 
    private:
    /* data */
};

class Derived:public Base
{
public:
    void f(){
        cout << "Derived::f()" << endl;
    };
    void g(){
        cout << "Derived::g()" << endl;
    };
    void h(){
        cout << "Derived::h()" << endl;
    };
private:
    /* data */
};

int main(){
    Base* p = new Derived;
    p->f();p->g();p->h();
}


