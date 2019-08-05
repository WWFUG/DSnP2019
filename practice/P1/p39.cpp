#include <iostream>

class C
{
public:
    void printAddr() const;
private:
    /* data */
    int _a , _b;
};

void C::printAddr() const{
    std::cout << "this_ptr ==> _a ==> _b" << std::endl;
    std::cout << this << std::endl;
    std::cout << &_a << std::endl;
    std::cout << &_b << std::endl;
}

int main(){
    C c1,c2;
    std::cout << "c1 = " << &c1 << std::endl;
    std::cout << "c2 = " << &c2 << std::endl;
    c1.printAddr();
    c2.printAddr();
    return 0;
}


