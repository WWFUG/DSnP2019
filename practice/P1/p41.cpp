#include <iostream>

class C
{
public:
    C (int a,int b):_a(a),_b(b){};
    void printAddr() const;
    void printData() const;
private:
    /* data */
    int _a,_b;
};

void C::printData()const {
    std::cout << _a << " " << _b << std::endl;
}

void C::printAddr()const {
    std::cout << &_a << " " << &_b << std::endl;
}

int main(){
    C c1(1,2);
    C c2(0,0);
    c1.printAddr();
    c1.printData();
    c2.printAddr();   
    c2.printData();
}











