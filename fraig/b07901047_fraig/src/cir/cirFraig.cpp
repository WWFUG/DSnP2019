/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include <unordered_set>
#include <unordered_map>
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
    while(!_fecGrps.empty()){ 
        SatSolver solver;
        solver.initialize();
        unordered_map<CirGate*, unsigned> dfs;
        genProofModel(solver, dfs);
        //solver.assumeProperty(_gateList[339]->getSatV(), unsigned(0));
        //cout << solver.assumpSolve() << endl;
        //return ;
        unsigned satCnt=0;
        unsigned dfsCnt = 0;
        for(auto& g: _dfsList) {
            if(!g->isFec() || !g->getId()){ dfsCnt++;continue;}
            bool result;
            unsigned index, lit;
            unsigned grpId = g->getFecId();
            auto iter1 = _fecGrps[grpId].begin();
            auto iter2 = _fecGrps[grpId].begin();
            // finding this gate's position
            for(unsigned i=0; i<_fecGrps[grpId].size();++i,++iter1){
                if(g->getId()==_fecGrps[grpId][i]/2){
                    index = i; 
                    lit = _fecGrps[grpId][i];
                    break;
                }
            }
            for(unsigned i=0; i<_fecGrps[grpId].size(); ++i,++iter2){
                if(i!=index && _fecGrps[grpId][i]!=-1){
                    result = proofFecPair( (AIGate*)_gateList[(_fecGrps[grpId][i]/2)], 
                                           (AIGate*)g, (lit+_fecGrps[grpId][i])%2, solver);
                    if(result){ 
                        collectPattern(solver);
                        ++satCnt;
                    }
                    else{
                        if(_fecGrps[grpId][i]/2==0){
                            cout << "Fraig: ";
                            g->replaceGate(GateV(_gateList[(_fecGrps[grpId][i]/2)], (lit+_fecGrps[grpId][i])%2));
                            delete g;
                            _gateList[lit/2] = 0;
                            _gateList[(_fecGrps[grpId][i]/2)]->setFecId(-1);
                            _fecGrps[grpId][index] = -1;
                        }
                        else{
                            //cout << dfs.at(g) << " " << dfs.at(_gateList[(_fecGrps[grpId][i]/2)]) << endl;
                            if(dfs.at(g) <= dfs.at(_gateList[(_fecGrps[grpId][i]/2)]) ) continue;
                            cout << "Fraig: ";
                            g->replaceGate(GateV(_gateList[(_fecGrps[grpId][i]/2)], (lit+_fecGrps[grpId][i])%2));
                            delete g; 
                            _gateList[lit/2] = 0;
                            _gateList[(_fecGrps[grpId][i]/2)]->setFecId(-1);
                            _fecGrps[grpId][index] = -1;
                        }
                        _dfsList[dfsCnt] = 0;
                        break;
                    }
                }
            }
            if(satCnt==8)
                break;
            ++dfsCnt;
        }
        _dfsList.clear();
        _fecGrps.clear();
        genDFSList();
        if(satCnt!=0){
            runSim(satCnt);
        }
        cout << "Updating FEC by SAT... #Total FECs = " << _fecGrps.size() << endl; 
    }
 
}
 
/********************************************/
/*   Private member functions about fraig   */
/********************************************/
void 
CirMgr::genProofModel(SatSolver& s, unordered_map<CirGate*, unsigned>& dfsOrder){
    //TODO allocate and record variables, No VarId for POs;
    //s.addAigCNF(gateVar,in1,inV1,in2,inV2);
    bool isConst = 0;
    for(auto& g: _dfsList){
        if(g->getId()==0) isConst = 1;
        unsigned k = s.newVar();
        g->setSatV(k);
    }
    if(!isConst){
        unsigned k = s.newVar();
        _const0->setSatV(k);
    }
    unsigned cnt = 1;
    for(auto& g: _dfsList){
        if(g->isAig()){
            GateV in1 = g->_faniList[0];
            GateV in2 = g->_faniList[1];
            s.addAigCNF(g->getSatV(), in1.gate()->getSatV(), in1.isInv()
                                    , in2.gate()->getSatV(), in2.isInv());
            dfsOrder.emplace(g, cnt);
        }
        ++cnt; 
    }
    dfsOrder.emplace(_const0, 0);
}

bool
CirMgr::proofFecPair(AIGate* g1, AIGate* g2, bool phase, SatSolver& s){
    if(g1->getId()==0){
        bool result;
        cout << "Proving SAT... " << g1->getId() << "!=" << g2->getId() << " " << phase;
        s.assumeRelease();
        s.assumeProperty(g2->getSatV(), phase);
        result = s.assumpSolve();
        cout << (!result? " SAT" : " UNSAT ") << endl;
        return !result;
    }
    bool result;
    cout << "Proving SAT... " << g1->getId() << "!=" << g2->getId() << " " << phase;
    unsigned newV = s.newVar();
    s.addXorCNF(newV, g1->getSatV(), false, g2->getSatV(), phase);
    s.assumeRelease();  // Clear assumptions
    s.assumeProperty(newV, true);  
    s.assumeProperty(_gateList[0]->getSatV(), false);
    result = s.assumpSolve();
    cout << (result? " SAT" : " UNSAT ") << endl;
    
    return result;
}


void
CirMgr::collectPattern(SatSolver& s){
    for(auto g: _dfsList){
        if(!g) continue;
        if(g->getTypeStr()=="PI"){
            ((PIGate*)g)->loadPattern( (bool)s.getValue(g->getSatV()), 1);
        }
    }
    //runSim(1);
    //cout << "Updating by SAT #Total FECs " << _fecGrps.size() << endl;
}
