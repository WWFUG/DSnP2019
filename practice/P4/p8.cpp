#include <iostream>
using namespace std;

int main(){
    int* p = new int;
    cout << p << endl;
    delete p;
    //p = 0; good style , avoid pointing the same obj.
    cout << p << endl;
    *p = 30;
    int j = *p;
    cout << j << endl;
    int* q = new int(20);
    int k = *p;
    cout << k << endl;
    *p = 40;
    cout << k << endl;
    cout << p << " " << q << endl;
}
