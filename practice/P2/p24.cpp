#include <iostream>

class T 
{
public:
    T (int num): _d(num){};
    T* incre(){ _d+=10; return this;};
    void print()const{ std::cout << _d << std::endl; }
private:
    /* data */
    int _d;
};


int main(void){
    const T a(10);
    a.print();
    const_cast<T *>(&a)->incre()->print(); // using const_cast ptr to modified the variable(object)
    a.print();
    return 0;
}











