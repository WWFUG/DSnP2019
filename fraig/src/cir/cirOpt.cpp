/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include <algorithm>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
    //TODO determine whether the undefList is needed or not
    for(auto id: _unusedIdList)
        _gateList[id]->sweep();
    //TODO update UNDEF float List
    _unusedIdList.clear();_fltIdList.clear();_undefIdList.clear();
    for (auto g: _dfsList) {
        if(g->getTypeStr()=="UNDEF")
            _undefIdList.push_back(g->_id);
        if(g->isFlt())
            _fltIdList.push_back(g->_id);
    }
    for(auto id: _piIdList)
        if(_gateList[id]->unUsed())
            _unusedIdList.push_back(id);
    sort(_fltIdList.begin(), _fltIdList.end());
    sort(_undefIdList.begin(), _undefIdList.end());
    sort(_unusedIdList.begin(), _unusedIdList.end());
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
