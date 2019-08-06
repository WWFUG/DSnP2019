#include <iostream>


const int a = 10;
int b = 10;

int main(){
    const int& c = b;b = 20; // const var. can't be put on the left(reassignment)
    std::cout << c << std::endl;
}









