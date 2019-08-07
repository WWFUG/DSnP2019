#include <iostream>
#include <assert.h>
#include <cstdlib>
using namespace std;

#define MAX_DEPTH 5

class N;
class N_;
N_* nList[1 << MAX_DEPTH] = {0};

class N
{
public:
    N ():_n(0) {};
    void gen();
    void statistics() const;

private:
    /* data */
    N_* _n;
};


class N_
{
    friend class N;
    unsigned _refCnt;
    N _child1;
    N _child2;
    N_():_refCnt(0){};
};

void N::gen(){
    assert(_n==0);  
    int i = rand()%(1<<MAX_DEPTH);
    if(nList[i] == 0){
        N_* temp = new N_;
        _n = temp;
        _n->_refCnt++;
        nList[i] = _n;
        _n->_child1.gen();
        _n->_child2.gen();
        temp = NULL;
    }
    else if(nList[i]!=0){
        _n = nList[i];
        nList[i]->_refCnt++;
    }
}

void N::statistics()const {
    unsigned int maxRef = 0;
    for (int i = 0; i < 1<<MAX_DEPTH; i++) {
        if(nList[i]->_refCnt > maxRef){
            maxRef = nList[i]->_refCnt;
        }
    }
    unsigned int Ref[maxRef+1];
    for (unsigned int i=0; i<maxRef+1; i++){ Ref[i] = 0;} 
    for (int i = 0; i < 1<<MAX_DEPTH; i++) {
        if(nList[i]==0){
            Ref[0]+=1;
        }
        else{
            Ref[nList[i]->_refCnt]+=1;
        }
    }
    for (unsigned int i = 0; i < maxRef+1; i++) {
        cout << "Ref["  << i << "]" << " = " << Ref[i] << endl;
    }
}

int main(){
    N root;
    root.gen();
    root.statistics();

    return 0;
}


















