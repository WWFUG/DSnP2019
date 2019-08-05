#include <iostream>

class B 
{
public:
    B(int entry):_num(entry){};

private:
    /* data */
    int _num;
};

class A
{
public:
    A (int ad , int entry): _ad(ad),_b(entry) {std::cout << "successfully init" << std::endl;};

private:
    /* data */
    int _ad;
    B _b;
};

int main(){
    A myA(1,2);
    return 0;
}
































