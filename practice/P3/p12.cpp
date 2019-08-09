#include <iostream>
using namespace std;

class Base
{
public:
    virtual void f(){
        cout << "Base::f()" << endl;
    };
    void g(){
        cout << "Base::g()" << endl;
    };
    virtual void h(){
        cout << "Base::h()" << endl;
    };

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
    }

private:
    /* data */
};

int main(){
    Base* p = new Derived;
    Base* q = new Base;
    Derived* r = new Derived;
    cout << "Base ptr containing Derived Obj:" << endl;
    p->f();p->g();p->h();
    cout << "Base ptr containing Base Obj:" << endl;
    q->f();q->g();q->h();
    cout << "Derived ptr containing Derived Obj:" << endl;
    r->f();r->g();r->h();
}






