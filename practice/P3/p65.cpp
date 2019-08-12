#include <algorithm>
#include <iostream>
using namespace std;
//error! since operator() is a nonstatic member(this ptr as an argument)
//which contradicts the orig. def. of sort(beg,end,com)'s compare function 
struct Compare{
    virtual bool operator() (int, int) const;
};

struct Less : public Compare{
    bool operator() (int i, int j) const { return (i<j); }
};

struct Greater : public Compare{
    bool operator() (int i, int j) const { return (i>j); }
};

void f(const Compare& c){
    int arr[10] = {4,2,6,7,1,3,5,9,8,0};
    std::sort(arr, arr+10, c);
    for (int i = 0; i < 10; ++i) {
        cout << arr[i] << endl;  
    }
}

int main(){
    f(Less());
    f(Greater());
}
