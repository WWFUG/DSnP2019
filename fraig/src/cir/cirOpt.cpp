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
    vector<unsigned> idList(_miloa[0]+_miloa[3]+1, 1);
    _unusedIdList.clear();_fltIdList.clear();_undefIdList.clear();
    for (auto g: _dfsList) {
        idList[g->_id] = 0;
        if(g->isFlt())
            _fltIdList.push_back(g->_id);
    }
    sort(_fltIdList.begin(), _fltIdList.end());
    for(unsigned i=1; i<idList.size(); ++i){
        if(idList[i] && _gateList[i])
            _gateList[i]->sweep();
    }
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
    _fltIdList.clear(); _unusedIdList.clear();
    for(auto g: _dfsList){
        if(g->isAig())
            g->optimize();
    }
    _dfsList.clear();
    genDFSList();
    genFltList();
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
