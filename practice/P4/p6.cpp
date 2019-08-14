#include <iostream>
using namespace std;

int main(){
    int a[4] = {0};
    int b = 5;
    cout << &a << endl;
    cout << &b << endl;
    a[5] = b;
    cout << a[5] << endl;
    cout << b << endl;
    return 0;
}
