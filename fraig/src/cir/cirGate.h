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
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class GateV
{
#define NEG 0X1 
    friend CirGate;
public:
    GateV(CirGate* g, size_t phase) : _gateV( (size_t)g+phase ){} 
    GateV(size_t gateV) : _gateV(gateV) {}
    CirGate* gate() const { return (CirGate*)(_gateV & ~NEG);}
    GateV operator ! () const { return GateV((size_t)gate()+!isInv()); }
    void setGate(CirGate* g){ _gateV = size_t(g)+isInv(); }
    bool isInv() const { return _gateV & NEG;}
    bool operator == (const GateV& g) const { return _gateV == g._gateV; }
private:
    size_t _gateV;
};

class CirGate
{
    friend CirMgr;
public:
    CirGate(unsigned id, unsigned lNo): _id(id), _lineNo(lNo), _ref(0){}
    virtual ~CirGate() {}

    // Basic access methods
    virtual string getTypeStr() const = 0;
    virtual bool unUsed() const {return _fanoList.empty();}
    virtual bool isAig() const { return false; }
    virtual bool isFlt() const;
    unsigned getLineNo() const { return _lineNo; }
    unsigned getId() const {return _id;}
 
    // Printing functions
    virtual void printGate() const = 0;
    virtual void optimize(); // for AIGate Only
    virtual void dfsTraversal();
    virtual void sweep();
    void reportGate() const;
    void reportFanin(int level) const;
    void reportFanout(int level) const;
    void setToGlobalRef() const { _ref = CirGate::_globalRef; }
    void buildConnect();
    bool setSymbol(string s){ _symbol = s; return true;}
    bool isGlobalRef() const { return _ref==CirGate::_globalRef; }


    static void setGlobalRef () { ++_globalRef; }
private:
    static unsigned _globalRef;
    void preOrderPrint(int& level , const int flag, int iter=0) const;
    void deleteFano(CirGate* g);
    void replaceFani(CirGate* g, GateV gV);
    void replaceGate(GateV gV);
    void pushFano(GateV fano){_fanoList.push_back(fano);}

protected:
    GateVList _faniList;
    GateVList  _fanoList;
    unsigned _id;
    unsigned _lineNo;
    mutable unsigned _ref;
    string _symbol;
    
        //bool addFanin(GateV& g){ _faniList.push_back(g); return true; }     //TODO check if repeated
    //bool addFanout(CirGate*& g){ _fanoList.push_back(g); return true;}
};


class AIGate: public CirGate
{
public:
    AIGate(unsigned id, unsigned lNo, size_t& in1, size_t& in2): CirGate(id, lNo) {
        _faniList.push_back(GateV(in1)); // in1 & in2 are literals 
        _faniList.push_back(GateV(in2));
    }
    string getTypeStr() const { return "AIG"; }
    void printGate() const;
    bool isAig() const { return true; }
private:
};

class POGate: public CirGate
{
public:
    POGate(unsigned id, unsigned lNo, size_t& gateV): CirGate(id, lNo) {
        _faniList.push_back(GateV(gateV));
    }
    void printGate() const;
    bool unUsed() const {return false;}
    void sweep(){}
    void optimize(){}
    string getTypeStr() const { return "PO"; }
private:
};

class PIGate: public CirGate
{
public:
    PIGate(unsigned id, unsigned lNo): CirGate(id, lNo) {}
    string getTypeStr() const { return "PI"; }
    void printGate() const;
    void sweep();
    void optimize(){};
private:
};

class UNDEF: public CirGate
{
public:
    UNDEF(unsigned id, unsigned lNo = 0): CirGate(id, lNo) {}
    string getTypeStr() const { return "UNDEF"; }
    void printGate() const {}
    void optimize(){}
    bool isFlt() const { return false; }
private:
};

class CONST: public CirGate
{
public:
    CONST(unsigned id = 0, unsigned lNo = 0): CirGate(id, lNo) {}
    string getTypeStr() const { return "CONST"; }
    void printGate() const { cout << "CONST" << _id; }
    void sweep(){_fanoList.clear();}
    void optimize(){}
    bool unUsed() const {return false;} 
private:
};

#endif // CIR_GATE_H
