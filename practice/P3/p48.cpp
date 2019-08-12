#include <iostream>
using namespace std;

template<class T>class Array
{
public:
    Array (size_t i = 0){ _data = new T[i]; }
    T& operator[] (size_t i) const { return _data[i]; }
//    const T& operator[] (size_t i) const { return _data[i]; }
private:
    /* data */
    T* _data;
};
template<class T>
void f(const Array<T>& arr){
    arr[1] = 20;
    int a = arr[0];
}
int main(){
    Array<int> arr(10);
    f(arr);
}
