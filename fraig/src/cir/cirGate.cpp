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

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;
unsigned CirGate::_globalRef = 0;

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
    preOrderPrint(level, 0); 
}

void
CirGate::reportFanout(int level) const
{
    assert (level >= 0);
    CirGate::setGlobalRef();
    preOrderPrint(level, 1); 
}

void
CirGate::preOrderPrint(int& level, int flag, int iter) const
{
    cout << this->getTypeStr() << " " << this->_id ;
    GateVList gl = flag?_fanoList:_faniList;
    if(iter>=level || gl.empty()){
        cout << endl;
        return;
    }
    if(this->isGlobalRef()){
        cout << " (*)" << endl;
        return;
    }
    else 
        this->setToGlobalRef(); 
    cout << endl;
    for (auto g : gl) { 
        for (int _ = 0; _ < iter+1; _++) cout << "  ";
        if(g.isInv())
            cout << "!";
        g.gate()->preOrderPrint(level, flag, iter+1);
    } 
}

bool
CirGate::isFlt() const
{
    for(auto g: _faniList){
        if((g.gate()->getTypeStr())=="UNDEF")
            return true;
    }
    return false;
}

void
CirGate::buildConnect()
{
    for(unsigned i=0; i<_faniList.size(); ++i){
        size_t lit = (size_t)(_faniList[i].gate());
        CirGate* gate = cirMgr->_gateList[lit/2];
        if(!gate) {
            gate = cirMgr->_gateList[lit/2] = new UNDEF(lit/2);
            cirMgr->_undefIdList.push_back(lit/2);
        }
        else
            (gate->_fanoList).push_back(GateV(this, lit%2));
        _faniList[i].setGate(gate);
    }
}

void
CirGate::deleteFano(CirGate* g){
    for(auto iter = _fanoList.begin(); iter!=_fanoList.end(); ++iter){
        if((*iter).gate()==g){
            _fanoList.erase(iter);
            return;
        }
    }
}

void
CirGate::sweep()
{
    // for sweep onty  
    assert(_fanoList.empty());
    if(getTypeStr()=="AIG") --(cirMgr->_miloa[4]);
    cout << "Sweeping: " << getTypeStr() << "(" << _id << ") removed" << endl;
    for(auto fanin: _faniList){
        CirGate* g = fanin.gate();
        if(!g->_fanoList.empty()){
            g->deleteFano(this);
        }            
        if(g->_fanoList.empty())
            g->sweep();
    }
    cirMgr->_gateList[_id] = 0;
    delete this;
}

void
CirGate::dfsTraversal()
{
    for( auto g: _faniList ){
        CirGate* thisG = g.gate();
        if(!thisG->isGlobalRef()){
            thisG->setToGlobalRef();
            thisG->dfsTraversal();
        }
    }
    cirMgr->_dfsList.push_back(this);
}

/*************************************/
/*          AIGATE FUNCTION          */
/*************************************/

void
AIGate::printGate() const
{
    cout << setw(4) << left << "AIG " << _id << " ";
    for (int i = 0; i < 2; i++) {
        CirGate* thisGate = _faniList[i].gate();
        if(thisGate->getTypeStr()=="UNDEF")
            cout << "*";
        if(_faniList[i].isInv())
            cout << "!";
        cout << ((AIGate*)thisGate)->_id;
        if(i!=1)
            cout << " " ;
    }
}


/*************************************/
/*          PIGATE FUNCTION          */
/*************************************/


void 
PIGate::printGate() const
{
    cout << setw(4) << left << "PI" << _id;
    if(!_symbol.empty())
        cout << " (" << _symbol << ")";
}

/*************************************/
/*          POGATE FUNCTION          */
/*************************************/

void 
POGate::printGate() const
{
    CirGate* thisGate = _faniList[0].gate();
    cout << setw(4) << left << "PO" << _id << " ";
    if(thisGate->getTypeStr()=="UNDEF")
        cout << "*";
    if(_faniList[0].isInv())
        cout << "!";
    cout << ((POGate*)thisGate)->_id;
    if(!_symbol.empty())
        cout << " (" << _symbol << ")";
}

/*************************************/
/*          UNDEF FUNCTION           */
/*************************************/


/*************************************/
/*          CONST FUNCTION           */
/*************************************/


