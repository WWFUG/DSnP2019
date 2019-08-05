#include <iostream>

class C
{
public:
private:
    /* data */
    int a,b,c,d,e,f;
};
int main()
{
    std::cout << "size of C = " << sizeof(C) << std::endl;
    C x,y,z;
    std::cout << "x : " << &x << std::endl;
    std::cout << "y : " << &y << std::endl;
    std::cout << "z : " << &z << std::endl; 

    int *a = new int;
    C *c1 = new C;
    std::cout << "a(int) : " << &a << " | " << a << std::endl;
    std::cout << "c1(object) : " << &c1 << " | " << c1 << std::endl;
    return 0;
}
