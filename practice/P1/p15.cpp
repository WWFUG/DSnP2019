#include <iostream>
using namespace std;

int main(){
    int a = 10;
    int &r = a;
    cout << "address_a = " << &a << endl;
    cout << "address_r = " << &r << endl;
    int b = 20;
    r = b;
    cout << "value_a = " << a << endl;
    return 0;
}
