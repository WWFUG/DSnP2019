#ifndef A64_H
#define A64_H

class N
{
public:
    N(void* p): _p(p){};
    void setMark() {  *static_cast<int*>(_p)|=1; };
    bool checkMark() const {return *static_cast<int*>(_p)&1;};
    void* getPtr() const {return _p;};
private:
    /* data */
    void* _p;
};

#endif
