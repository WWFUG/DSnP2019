#include "a64.h"
#include <iostream>
using namespace std;

int main(){
    int* a = new int;
    *a = 5;
    N n(&a);
    cout << n.getPtr() << endl;
    n.setMark();
    cout << n.checkMark() << endl;
    cout << n.getPtr() << endl;

}
