#include <iostream>
using namespace std;

int main(){
    int a,b,c;
    while(cin >> a >> b){
        cout << "========Before=========" << endl;
        cout << "address_a = " << &a << endl;
        cout << "address_b = " << &b << endl;
        cout << "address_c = " << &c << endl;
        c = a+b;
        cout << "========After=========" << endl;
        cout << "address_a = " << &a << endl;
        cout << "address_b = " << &b << endl;
        cout << "address_c = " << &c << endl;
    }
    return 0;
}
