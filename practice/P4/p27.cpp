#include <iostream>
#include <stdlib.h>
using namespace std;
#define SIZE (1<<18)
class A
{
public:
    A (){ ++_cnt; };
    ~A(){};
    void operator = (int num){ _d = num; }
    static int getCnt(){ return A::_cnt; }
    static void reset(){ _cnt = 0; }
private:
    int _d;
    static int _cnt;
};
int A::_cnt = 0;

void f(){
    A::reset();
    A* a[SIZE];
    for(int i=0; i< SIZE;i++){
        a[i] = new A;
        *(a[i]) = i;
    } 
    cout << A::getCnt() << endl;
    for (int i = 0; i < SIZE; ++i) {
        delete a[i];
    }
}

void g(){
    A::reset();
    A* a[SIZE];
    //A* b = (A*)calloc(SIZE,sizeof(A));
    A* b = new A[SIZE];
    for (int i = 0; i < SIZE; ++i) {
        a[i] = b+i;
        *(a[i]) = i;
    }
    cout << A::getCnt() << endl;
    free(b);
}

int main(){
    f();
    g();
}



