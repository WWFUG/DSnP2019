#include <iostream>
using namespace std;

class A
{
public:
    ~A(){};
};

int main(){
    int a[10] = {0};
    int* b = new int[10];

    A c[10];
    A* d = new A[10];

    cout << sizeof(a) << endl;
    cout << sizeof(b) << endl;
    cout << sizeof(c) << endl;
    cout << sizeof(d) << endl;
}
