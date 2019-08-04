#include <iostream>
using namespace std;

int main(){
    int a;
    int* p = NULL;
    cout << "======Before======" << endl;
    cout << "address_a = " << &a << endl;
    cout << "address_p = " << &p << endl;
    cout << "p = " << p << endl;
    cout << "======After=======" << endl;
    p = &a;
    cout << "p = &a " << endl;
    cout << "address_a = " << &a << endl;
    cout << "address_p = " << &p << endl;
    cout << "p = " << p << endl;
}
