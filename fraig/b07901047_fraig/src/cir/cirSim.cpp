/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <cassert>
#include <string>
#include "rnGen.h"
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <typeinfo>

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

#define PATNUM 64
/*******************************/
/*   Global variable and enum  */
/*******************************/

struct fecKey{
    size_t alnValue;
    unsigned grpId;
    fecKey(const size_t& v,const unsigned& id){ alnValue = v; grpId = id;}
    bool operator == (const fecKey& k) const{
        return (alnValue==k.alnValue && grpId==k.grpId) ;
    }
};

struct fecHash{
    size_t operator ()(const fecKey& k) const{
        return k.alnValue;
    }
};


/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/

RandomNumGen gen = RandomNumGen();
void
CirMgr::randomSim()
{
    //TODO logfile
    unsigned patNum = 0;
    unsigned stackNum = 0;
    unsigned prevSize = 0, thisSize = 0;
    size_t max = ((size_t)1<<31);
    while(true){
        prevSize = _fecGrps.size();
        for(auto& id: _piIdList)
            ((PIGate*)_gateList[id])->loadPattern( ((size_t)gen(max)<<32)+gen(max) );
        runSim(PATNUM);
        thisSize = _fecGrps.size();
        if( thisSize-prevSize == 0 ){
            if (++stackNum > 50){
                cout << patNum*64 << " pattterns simulated." << endl;
                return;
            }
        }
        else
            stackNum = 0;
        ++patNum;
    }
}

void
CirMgr::fileSim(ifstream& patternFile)
{
    //TODO logfile
    string line;
    unsigned cnt = 0;
    while(patternFile >> line){
        if(cnt%PATNUM==0 && cnt)
            runSim(PATNUM);

        if( line.length() != _miloa[1] ){
            cerr << "Error: Pattern(" << line << ") length(" << line.length() << ")"
                 << " does not match the number of inputs("   << _miloa[1] << ") in a circuit!!" << endl;
            cout << (cnt/PATNUM)*PATNUM << " patterns simulated." << endl;
            return ;
        }
        for(unsigned i=0; i<_miloa[1]; ++i){
            if( line[i]!='0' && line[i]!='1' ){
                cerr << "Error: Pattern(" << line << ") contains a non-0/1 character('" << line[i] << "')." << endl;
                cout << (cnt/PATNUM)*PATNUM << " patterns simulated." << endl;
                return;
            }
            else
                ((PIGate*)_gateList[_piIdList[i]])->loadPattern(line[i]=='1');
        }
        ++cnt;
    }
    if(cnt%PATNUM!=0){
        for(auto id: _piIdList)
            ((PIGate*)_gateList[id])->lShift(PATNUM-cnt%PATNUM);
    }
    runSim(cnt%PATNUM);
    cout << cnt << " patterns simulated." << endl;
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
void 
CirMgr::runSim(const unsigned cnt){
    for(auto gate: _dfsList)
        gate->simulate();
    
    if(_simLog){
        for(int i=PATNUM; i>(int)(PATNUM-cnt); --i){
            for(auto& id: _piIdList)
                *_simLog << ((_gateList[id]->_simValue)>>(i-1))%2;
            *_simLog << ' ';
            for(unsigned j=0; j<_miloa[3]; ++j)
                *_simLog << ((_gateList[_miloa[0]+j+1]->_simValue)>>(i-1))%2; 
            *_simLog << '\n';
        }
    }
    
    if(!_fecGrps.empty())
        updateFEC();
    else
        initFEC();
}

void 
CirMgr::initFEC(){
    assert(_fecGrps.empty());
    //cout << "Init FEC ..." <<endl;
    unordered_map<size_t, IdList> fecDict; // Key is the concatenation of grpId & simValue
    fecDict.emplace(~(size_t)0, IdList(1, 1));
    for(auto g : _dfsList){
        if(g->isAig()){
            bool isInv = 0;
            AIGate* gate = (AIGate*) g;
            size_t key = gate->fecKey(isInv);
            auto pair = fecDict.emplace( key, IdList(1, 2*(gate->getId())+isInv));
            if(!pair.second)
                (pair.first->second).push_back(2*(gate->getId())+isInv);
        }
    }
    unsigned grpId = 0;
    for(auto pair: fecDict){
        if(pair.second.size()>1){
            _fecGrps.push_back(pair.second);
            for(auto lit: _fecGrps[grpId])
                ((AIGate*)_gateList[lit/2])->setFecId(grpId);
            ++grpId;
        }
    }
}

void 
CirMgr::updateFEC(){
    //cout << "Updatin FEC ..." << endl;
    vector<IdList> newGrps; 
    unsigned grpId = 0;
    for(auto grp: _fecGrps){
        unordered_map<size_t, IdList> localHash;
        for(auto lit: grp){
            if(lit/2==0){
                auto pair = localHash.emplace(~(size_t)0, IdList(1,1));
                if(!pair.second)
                    (pair.first->second).push_back(1);
                continue;
            }
            bool isInv = 0;
            AIGate* gate = (AIGate*) _gateList[lit/2];
            size_t key = gate->fecKey(isInv);
            auto pair = localHash.emplace( key , IdList(1, 2*(gate->getId())+isInv));
            if(!pair.second)
                (pair.first->second).push_back(2*(gate->getId())+isInv);
        }
        for(auto pair: localHash){
            if(pair.second.size()>1){
                sort(pair.second.begin(), pair.second.end());
                newGrps.push_back(pair.second);
                for(auto lit: newGrps[grpId])
                    _gateList[lit/2]->setFecId(grpId);
                ++grpId;       
            }
            else
                _gateList[(pair.second)[0]/2]->setFecId(-1);
        }
    }
    _fecGrps.swap(newGrps); 
}
