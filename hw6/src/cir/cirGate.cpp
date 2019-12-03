/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;
unsigned CirGate::_globalRef = 0;

#define PHASE(var) var%2
#define ID(var) unsigned(var/2)



// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
    string gInfo = getTypeStr() + "(" + to_string(_id) + ")"; 
    if(!_symbol.empty())
        gInfo += "\"" + _symbol + "\"" ;
    gInfo += ", line " + to_string(_lineNo);
    cout << "==================================================" << endl 
         << "= " << setw(47) << left <<       gInfo       << "=" << endl
         << "==================================================" << endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   CirGate::setGlobalRef();
   int flag = 0;
   preOrderPrint(level, flag);
}

//  NOTE!!!!!!!!!!!!! const is taken away
void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   CirGate::setGlobalRef();
   int flag = 1;
   preOrderPrint(level, flag);
}

void 
CirGate::dfsTraversal() const{
    for (auto i : _faninList) {
        CirGate* thisGate = getGate(i);
        if(!thisGate->isGlobalRef()){
            thisGate->setToGlobalRef();
            thisGate->dfsTraversal();
        }
    }
    cirMgr->_dfsList.push_back((CirGate*)this);
}


void 
CirGate::preOrderPrint(int level, int& flag, int iter ) const {          
    cout << this->getTypeStr() << " " << this->_id ;
    GateList v = flag? _fanoutList:_faninList;
    if(iter>=level || v.empty()){
        cout << endl;
        return;
    }
    if(this->isGlobalRef()){
        cout << "(*)" << endl;
        return;
    }
    else 
        this->setToGlobalRef(); 
    cout << endl;
    for (auto i : v) { 
        for (int _ = 0; _ < iter+1; _++) cout << "  ";
        if(isInv(i)) cout << "!";
        getGate(i)->preOrderPrint(level, flag, iter+1);
    }
}

/****************************************/
/*              AIG Gate                */
/****************************************/
void 
AigGate::buildConnect(){
    size_t var0 = (size_t)_faninList[0];
    size_t var1 = (size_t)_faninList[1];
    // setting inverting input
    if(!cirMgr->_gateList[ID(var0)] || !cirMgr->_gateList[ID(var1)]){
        cirMgr->_floatIdList.push_back(_id);
        if(!cirMgr->_gateList[ID(var0)])
            cirMgr->_gateList[ID(var0)] = new UndefGate(ID(var0));
        if(!cirMgr->_gateList[ID(var1)])
            cirMgr->_gateList[ID(var1)] = new UndefGate(ID(var1));
    }
    _faninList[0] = (CirGate*)( (size_t)(cirMgr->_gateList[ID(var0)]) | (size_t)PHASE(var0) );
    _faninList[1] = (CirGate*)( (size_t)(cirMgr->_gateList[ID(var1)]) | (size_t)PHASE(var1) );
    // connect fanout
    getGate(_faninList[0])->pushFanout( (CirGate*)((size_t)(this) | PHASE(var0) ) );
    getGate(_faninList[1])->pushFanout( (CirGate*)((size_t)(this) | PHASE(var1) ) );
}

void 
AigGate::printGate() const{
    cout << setw(4) << left << "AIG " << _id << " ";
    for (int i = 0; i < 2; i++) {
        CirGate* thisGate = getGate(_faninList[i]);
        if(thisGate->getTypeStr()=="UNDEF")
            cout << "*";
        if(isInv(_faninList[i]))
            cout << "!";
        cout << ((AigGate*)thisGate)->_id;
        if(i!=1)
            cout << " " ;
    }
    cout << endl;
}

/****************************************/
/*               PO Gate                */
/****************************************/

void
PoGate::buildConnect(){
    size_t var0 = (size_t)_faninList[0];
    if(!cirMgr->_gateList[ID(var0)]){
        cirMgr->_gateList[ID(var0)] = new UndefGate(ID(var0));
        cirMgr->_floatIdList.push_back(_id);
    }
    _faninList[0] = (CirGate*)( (size_t)(cirMgr->_gateList[ID(var0)]) | PHASE(var0) );
    getGate(_faninList[0])->pushFanout( (CirGate*)((size_t)(this) | PHASE(var0)));
}

void 
PoGate::printGate() const{
    CirGate* thisGate = getGate(_faninList[0]);
    cout << setw(4) << left << "PO" << _id << " ";
    if(thisGate->getTypeStr()=="UNDEF")
        cout << "*";
    if(isInv(_faninList[0]))
        cout << "!";
    cout << ((PoGate*)thisGate)->_id ;
    if(!_symbol.empty())
        cout << " (" << _symbol << ")";
    cout << endl;
}
/****************************************/
/*               PI Gate                */
/****************************************/

void 
PiGate::printGate() const{
    if(_id==0){ 
        cout << "CONST0" << endl;
        return ;
    }
    cout << setw(4) << left << "PI" << _id;
    if(!_symbol.empty())
        cout << " (" << _symbol << ")";
    cout << endl;
}

/****************************************/
/*              UNDEF Gate              */
/****************************************/


