#include <iostream>
using namespace std;

class A
{
public:
    A (int entry = 0):_d(entry){}; 
    A& operator + (const A& a) const {
        A* tmp = new A;
        tmp->_d = this->_d + a._d;
        return *tmp;
    }
private:
    /* data */
    int _d;

};


int main(){
    A a1(10), a2(20);
    A a3;
    a3 = a3 + a2;
    return 0;
}
