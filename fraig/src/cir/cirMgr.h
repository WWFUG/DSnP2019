/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;

class CirMgr
{
    friend CirGate;
    friend AIGate;
    friend PIGate;
    friend POGate;
    friend CONST;
    friend UNDEF;
public:
    CirMgr() {}
    ~CirMgr() { reset(); } 

    // Access functions
    // return '0' if "gid" corresponds to an undefined gate.
    CirGate* getGate(unsigned gid) const;
    // Member functions about circuit construction
    bool readCircuit(const string&);

    // Member functions about circuit optimization
    void sweep();
    void optimize();

    // Member functions about simulation
    void randomSim();
    void fileSim(ifstream&);
    void setSimLog(ofstream *logFile) { _simLog = logFile; }

    // Member functions about fraig
    void strash();
    void printFEC() const;
    void fraig();

    // Member functions about circuit reporting
    void printSummary() const;
    void printNetlist() const;
    void printPIs() const;
    void printPOs() const;
    void printFloatGates() const;
    void printFECPairs() const;
    void writeAag(ostream&) const;
    void writeGate(ostream&, CirGate*) const;
 
private:
    ofstream           *_simLog;
    unsigned           _miloa[5];
    GateList           _gateList;
    GateList           _dfsList;
    IdList             _piIdList, _fltIdList, _unusedIdList, _undefIdList;
 
    static CONST* _const0;
    // private helper function
    bool readHeader(ifstream&);
    bool readIO(ifstream&, unsigned); // unsigned specified I/O
    bool readAig(ifstream&);
    bool readSymbol(ifstream&);
    bool readComment(ifstream&);
    void connect();
    void reset();    
    void genDFSList();
    void genFltList();
   
};

#endif // CIR_MGR_H
