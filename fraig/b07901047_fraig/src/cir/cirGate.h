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
struct Strash;

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
    size_t simVal() const;
    void setGate(CirGate* g){ _gateV = size_t(g)+isInv(); }
    bool isInv() const { return _gateV & NEG;}
    GateV operator ~ () const { return GateV((size_t)gate()+!isInv()); }
    bool operator == (const GateV& g) const { return _gateV == g._gateV; }
    bool operator ! () const; 
    operator bool() const;
private:
    size_t _gateV;
};

class CirGate
{
    friend CirMgr;
    friend GateV;
    friend PIGate;
    friend POGate;
    friend AIGate;
public:
    CirGate(unsigned id, unsigned lNo): _id(id), _lineNo(lNo), _ref(0){}
    virtual ~CirGate() {}

    // Basic access methods
    virtual string getTypeStr() const = 0;
    virtual bool unUsed() const {return _fanoList.empty();}
    virtual bool isAig() const { return false; }
    virtual bool isFlt() const;
    virtual void setFecId(const unsigned& id = 0) = 0;
    virtual void setSatV(const unsigned v){};
    virtual bool isFec() { return false;}
    virtual unsigned getFecId(){ return unsigned(-1); }
    virtual unsigned getSatV(){ return 0; };
    unsigned getLineNo() const { return _lineNo; }
    unsigned getId() const {return _id;}
 
    // Printing functions
    virtual void printGate() const = 0;
    virtual void simulate() = 0 ;
    virtual void optimize(); // for AIGate Only
    virtual void dfsTraversal();
    virtual void sweep();
    void reportGate() const;
    void reportFanin(int level) const;
    void reportFanout(int level) const;
    void setToGlobalRef() const { _ref = CirGate::_globalRef; }
    void buildConnect();
    bool setSymbol(string s) { _symbol = s; return true;}
    bool isGlobalRef() const { return _ref==CirGate::_globalRef; }

    static bool isNetChanged () { return _netChanged; }
    static void setChanged   () { _netChanged = 1;}
    static void resetChanged () { _netChanged = 0;}
    static void setGlobalRef () { ++_globalRef; }
private:
    static unsigned _globalRef;
    static bool     _netChanged;
    void preOrderPrint(int& level , const int flag, int iter=0) const;
    void deleteFano(CirGate* g);
    void replaceFani(CirGate* g, GateV gV);
    void replaceGate(GateV gV);
    void pushFano(GateV fano){_fanoList.push_back(fano);}

protected:
    GateVList _faniList;
    GateVList _fanoList;
    unsigned _id;
    unsigned _lineNo;
    size_t _simValue;
    mutable unsigned _ref;
    string _symbol;
    
    //bool addFanin(GateV& g){ _faniList.push_back(g); return true; }     //TODO check if repeated
    //bool addFanout(CirGate*& g){ _fanoList.push_back(g); return true;}
};


class AIGate: public CirGate
{
    friend struct Strash;
    friend CirMgr;
    friend CirGate;
public:
    AIGate(unsigned id, unsigned lNo, size_t& in1, size_t& in2): CirGate(id, lNo) {
        _faniList.push_back(GateV(in1)); // in1 & in2 are literals 
        _faniList.push_back(GateV(in2));
        _fecId = -1;
    }
    string getTypeStr() const { return "AIG"; }
    void printGate() const;
    void simulate() { _simValue = _faniList[0].simVal() & _faniList[1].simVal(); }
    void setFecId(const unsigned& id = 0) { _fecId = id; }
    bool isAig() const { return true; }
    size_t fecKey(bool& isInv) const { isInv = (_simValue%2==0);
                                       return isInv?~_simValue:_simValue; }
    unsigned getFecId () const {return _fecId;}
    void setSatV(const unsigned v) { _satVar = v;}
    unsigned  getSatV() { return _satVar; }
    bool isFec() {return (_fecId!=(unsigned)-1);}
    unsigned getFecId() {return _fecId;} 
private:
    unsigned _fecId;
    unsigned _satVar;
};

class POGate: public CirGate
{
public:
    POGate(unsigned id, unsigned lNo, size_t& gateV): CirGate(id, lNo) {
        _faniList.push_back(GateV(gateV));
    }
    void printGate() const;
    void simulate() { _simValue = _faniList[0].isInv() ? (~(_faniList[0].gate()->_simValue)) : _faniList[0].gate()->_simValue;}
    bool unUsed() const {return false;}
    void sweep() {}
    void optimize() {}
    void setFecId(const unsigned& id = 0) {}
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
    void simulate() {} ;
    void loadPattern(size_t p) { _simValue = p; }
    void loadPattern(bool bit) { _simValue = (_simValue<<1)+bit;}
    void loadPattern(bool bit, unsigned times) {
        for(unsigned i=0; i<times; ++i)
            _simValue = (_simValue << 1) + bit;
    }
    void lShift(unsigned times){_simValue = _simValue<<times;}
    void optimize(){}
    void setFecId(const unsigned& id = 0) {}
    void setSatV(const unsigned v) { _satVar = v;}
    unsigned getSatV() { return _satVar; }
private:
    unsigned _satVar;
};

class UNDEF: public CirGate
{
public:
    UNDEF(unsigned id, unsigned lNo = 0): CirGate(id, lNo) {_simValue = 0;}
    string getTypeStr() const { return "UNDEF"; }
    void printGate() const {}
    void optimize() {}
    void simulate() {}
    void setFecId(const unsigned& id = 0) {}
    bool isFlt() const { return false; }
private:
    unsigned _simValue;
};

class CONST: public CirGate
{
public:
    CONST(unsigned id = 0, unsigned lNo = 0): CirGate(id, lNo) { _simValue = 0;}
    string getTypeStr() const { return "CONST"; }
    void printGate() const { cout << "CONST" << _id; }
    void sweep() {_fanoList.clear();}
    void optimize() {}
    void simulate() {}
    void setFecId(const unsigned& id = 0) { }
    void setSatV(const unsigned& v) { _satVar = v;}
    unsigned getFecId () const {return _fecId;}
    unsigned getSatV() { return _satVar; }
    bool unUsed() const {return false;} 
    bool isFec() {return (_fecId!=(unsigned)-1);}
    unsigned getFecId() {return _fecId;} 
private:
    unsigned _fecId;
    unsigned _satVar;
};

#endif // CIR_GATE_H
