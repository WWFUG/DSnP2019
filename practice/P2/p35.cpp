#include <iostream>
using namespace std;
    


int main(void){
    char c[33] = "0123456789abcdefghijklmnopqrstu";
    void *p = c;
    char *p1 = (char*)p;
    short *p2 = (short*)p;
    int *p3 = (int*)p;

    cout << p1 << " , " << p1+1 << endl; // char ptr will auto. dereference 
    cout << p2 << " , " << p2+1 << endl; // short(2Bytes) the addr. of p2+2
    cout << p3 << " , " << p3+1 << endl; // int(8Bytes) the addr. of p3+4 

    short *q = p2+1; // pointing to char '2'
    int *s = (int*)q; // pointing to char '2' while taking 4 Bytes(int)
    *s = 0; // pointing to nothing ==> char in '2345' not pointed 
    cout << p1 << endl;
   
    cout << p1 << " , " << p1+2 << " , " << p1+4 << " , " << p1+6 << endl;
    return 0;
}


















