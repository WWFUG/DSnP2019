/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
    #define NEG 0x1
    friend CirMgr;
public:
    CirGate(){}
    CirGate(unsigned& id, int& lineNo, const IdList& input = IdList()): _lineNo(lineNo), _id(id), _ref(0){
        size_t size = input.size();
        _faninList.resize(size);
        for (size_t i = 0; i < size; ++i) 
            _faninList[i] = (CirGate*)(size_t)input[i] ;       
    } // input is a list of unsigned int
    virtual ~CirGate() {}

    // Basic access methods
    virtual string getTypeStr() const = 0;
    unsigned getLineNo() const { return _lineNo; }

    // Printing functions
    virtual void printGate() const = 0;
    void reportGate() const;
    void reportFanin(int level) const ;
    void reportFanout(int level) const ;

    // netlist function
    void pushFanout(CirGate* fanout){ _fanoutList.push_back(fanout) ;}
    void dfsTraversal() const;
    void preOrderPrint(int level, int& flag, int iter = 0) const;
    bool setSymbol(string sym){ 
        if(_symbol.empty()){ 
            _symbol = sym;
            return true;
        }
        return false;
    }
    virtual bool defNotUsed() = 0;
    virtual void buildConnect() = 0;
    virtual CirGate* getInGate(unsigned idx=0) = 0;
    static void setGlobalRef() { ++_globalRef; }
    static bool isInv(CirGate* fanin) { return size_t(fanin) & NEG ;}
    static CirGate* getGate(CirGate* fanin) { return (CirGate*)( size_t(fanin) & ~(NEG) );}

private:
    static unsigned _globalRef;

protected:
    // last bit in fanin store inverted/not
    GateList _faninList;   
    GateList _fanoutList;
    int _lineNo;
    unsigned _id;
    mutable unsigned _ref;
    string _symbol;

    void setToGlobalRef() const { _ref = CirGate::_globalRef; }
    bool isGlobalRef() const { return _ref==CirGate::_globalRef; }
};

class AigGate: public CirGate
{
public:
    AigGate(unsigned& id, int& lineNo, const IdList& varList): CirGate( id, lineNo, varList){}
    ~AigGate(){}
    // access methods
    string getTypeStr() const { return "AIG"; }

    // printing functions
    void buildConnect();
    void printGate() const;
    bool defNotUsed(){return _fanoutList.empty();}
    CirGate* getInGate(unsigned idx = 0){
        return getGate(_faninList[idx]);
    }

private:
};

class PiGate: public CirGate
{
public:
    PiGate(unsigned& id, int& lineNo): CirGate(id, lineNo) {}
    PiGate(): CirGate(){ _lineNo = 0; _id = 0;} // for const0 gate
    ~PiGate(){}
    string getTypeStr() const { 
        if(_id!=0) return "PI";
        else return "CONST";
    }
    void buildConnect(){}; // do nothing
    void printGate() const;
    bool defNotUsed(){return _fanoutList.empty();}
    CirGate* getInGate(unsigned idx=0) { return 0;}

private:
};

class PoGate:public CirGate
{
public:
    PoGate(unsigned& id, int& lineNo, IdList& varList): CirGate(id, lineNo, varList) {}
    ~PoGate(){}
    string getTypeStr() const { return "PO"; }
    void buildConnect();
    void printGate() const;
    bool defNotUsed(){return false;}
    CirGate* getInGate(unsigned idx=0){ return getGate(_faninList[0]);}
private:
};

// serve as a dummy class
class UndefGate: public CirGate
{
public:
    UndefGate(unsigned id): CirGate(id, 0) {}
    ~UndefGate(){}
    string getTypeStr() const { return "UNDEF"; }
    void buildConnect(){};
    void printGate() const{}; // do nothing
    bool defNotUsed(){return false;}
    CirGate* getInGate(unsigned idx=0){ return 0;}

private:
};


#endif // CIR_GATE_H
