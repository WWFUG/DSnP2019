/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include <unordered_set>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

 

/*******************************/
/*   Global variable and enum  */
/*******************************/
struct Strash{
    size_t operator() ( const AIGate* g) const{
        size_t k1 = ((g->_faniList[0].gate()->getId())<<1) + g->_faniList[0].isInv();
        size_t k2 = ((g->_faniList[1].gate()->getId())<<1) + g->_faniList[1].isInv();
        if(k2 < k1){
            size_t tmp = k1;
            k1 = k2;
            k2 = tmp; 
        }
        return (k1 <<32) + k2;
    }
};
struct KeyEqual{
    bool operator () (const AIGate* g1, const AIGate* g2) const{
        Strash s;
        return s(g1)==s(g2);
    }
};
/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
    //TODO memory overhead
    unordered_set<AIGate*, Strash, KeyEqual> strash;
    for(auto g: _dfsList){
        if(g->isAig()){
            auto res = strash.emplace((AIGate*)g);
            if(!res.second){
                cout << "Strashing: ";
                --_miloa[4];
                if(!CirGate::isNetChanged()) CirGate::setChanged(); 
                g->replaceGate( GateV( (size_t) *(res.first) ) );
                _gateList[g->getId()] = 0;
                delete g;
            }
        }
    }
    if(CirGate::isNetChanged()){
        _dfsList.clear();_fltIdList.clear();_unusedIdList.clear();
        CirGate::resetChanged();
        genDFSList();
        genFltList();
    }
}

void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
