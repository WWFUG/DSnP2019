/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;
CONST* CirMgr::_const0 = new CONST();

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static void
resetStaticVar(){
    lineNo = colNo = 0;
    memset(buf, 0, sizeof(buf));
    errInt = 0;
    errGate = 0;
}

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
    ifstream cirFile(fileName);
    if(!cirFile){
        cout << "Cannot open design \"" << fileName << "\"!!" << endl;
        cirFile.close();
        return false;
    } 
    if( !readHeader(cirFile) || !readIO(cirFile, 0)  || !readIO(cirFile, 1) ||
        !readAig(cirFile)    || !readSymbol(cirFile) || !readComment(cirFile)){
        reset();
        cirFile.close();
        resetStaticVar();
        return false;
    }
    cirFile.close();
    connect();
    genDFSList();
    genFltList();
    return true;
}

CirGate* 
CirMgr::getGate( unsigned gid ) const {
    if( !_gateList[gid] || gid > _miloa[0]+_miloa[3])
        return 0;
    return _gateList[gid];
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
        cout << "\nCircuit Statistics\n" <<
            "==================\n" <<
            "  PI   " << setw(9)   << right << _miloa[1] << endl <<
            "  PO   " << setw(9)   << right << _miloa[3] << endl <<
            "  AIG  " << setw(9)   << right <<_miloa[4] << endl <<
            "------------------\n" << 
            "  Total" << setw(9)   << _miloa[1]+_miloa[3]+_miloa[4] << endl; 
}

void
CirMgr::printNetlist() const
{
    unsigned cnt = 0;
    cout << endl;
    for (unsigned i = 0, n = _dfsList.size(); i < n; ++i) {
        if(_dfsList[i]->getTypeStr()!="UNDEF"){
            cout << "[" << cnt << "] ";
            _dfsList[i]->printGate();
            cout << endl; ++cnt;
        }
    }
}

void
CirMgr::printPIs() const
{
    unsigned size = _piIdList.size();
    cout << "PIs of the circuit: ";
    for (unsigned i=0 ; i<size ; ++i) {
        cout << _piIdList[i];
        if(i<size-1)
            cout << " " ;
    }
    cout << endl; 
}

void
CirMgr::printPOs() const
{
    cout << "POs of the circuit: ";
    for (unsigned i = 0; i < _miloa[3] ; ++i) {
        cout << i+_miloa[0]+1;
        if(i!=_miloa[3]-1)
            cout << " " ;
    }
    cout << endl; 
}

void
CirMgr::printFloatGates() const
{
    if(!_fltIdList.empty()){
        cout << "Gates with floating fanin(s): ";
        for(auto i=_fltIdList.begin(); i!=_fltIdList.end(); ++i){
            cout << *i ;
            if(i!=_fltIdList.end()-1) cout << " ";
        }
        cout << endl;
    }
    if(!_unusedIdList.empty()){
        cout << "Gates defined but not used  : ";
        for(auto i=_unusedIdList.begin(); i!=_unusedIdList.end(); ++i){
            cout << *i ;
            if(i!=_unusedIdList.end()-1) cout << " ";
        }
        cout << endl;
    } 
}

void
CirMgr::printFECPairs() const
{
}

void
CirMgr::writeAag(ostream& outfile) const
{
    int cnt=0;
    for (auto i: _dfsList) {
        if(i->getTypeStr()=="AIG")
            cnt+=1;
    }
    outfile << "aag" << " ";
    outfile << _miloa[0] << " " << _miloa[1] << " " <<_miloa[2] << " "
            << _miloa[3] << " " << cnt;
    for (auto i: _piIdList){
        outfile << endl;
        outfile << 2*i;
    }
    for (unsigned i=0; i<_miloa[3]; ++i){
        outfile << endl;
        CirGate* thisG = _gateList[i+_miloa[0]+1];
        //TODO
        outfile << 2*(thisG->_faniList[0].gate()->_id) + thisG->_faniList[0].isInv(); 
    }
    for (auto g: _dfsList){
        if(g->getTypeStr()=="AIG"){
            outfile << endl;
            //TODO
            unsigned var1 = 2*g->_faniList[0].gate()->_id + g->_faniList[0].isInv();
            unsigned var2 = 2*g->_faniList[1].gate()->_id + g->_faniList[1].isInv() ;
            outfile << 2*g->_id << " " << var1 << " " << var2;
        }
    }
    cnt = 0;
    for (auto i: _piIdList){
        if(!(_gateList[i]->_symbol.empty())){
            outfile << endl;
            outfile << "i" << cnt << " " << _gateList[i]->_symbol;
        }
        ++cnt;
    }    
    for (unsigned i=0; i<_miloa[3]; ++i){
        CirGate* thisG = _gateList[i+_miloa[0]+1];
        if(!(thisG->_symbol.empty())){
            outfile << endl;
            outfile << "o" << i << " " << thisG->_symbol;
        }
    } 
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
}

bool 
CirMgr::readHeader(ifstream& ifs)
{
    ifs >> buf;
    for(unsigned i=0; i<5; ++i){
        ifs >> _miloa[i];
    }
    _gateList.resize(_miloa[0]+_miloa[3]+1);
    _piIdList.resize(_miloa[1]); 
    for (unsigned i = 0; i < _miloa[0]+_miloa[3]+1; ++i) 
        _gateList[i] = 0;  
    ++lineNo; _gateList[0] = _const0;
    return true;
}

bool 
CirMgr::readIO(ifstream& ifs, unsigned flag) // input 0, output 1
{
    size_t lit;
    unsigned size = _miloa[2*flag+1];
    for (unsigned i = 0; i < size; ++i) {
        ifs >> lit;
        if(!flag){
            _gateList[lit/2] = new PIGate(lit/2, lineNo+1);
            _piIdList[i] = lit/2;
        }
        else{
            _gateList[_miloa[0]+i+1] = new POGate(_miloa[0]+i+1, lineNo+1, lit);
        }
        ++lineNo;
    }
    return true;
}

bool 
CirMgr::readAig(ifstream& ifs)
{
    size_t lit[3] = {0};
    for (unsigned i = 0; i < _miloa[4]; ++i) {
        ifs >> lit[0] >> lit[1] >> lit[2];
        _gateList[lit[0]/2] = new AIGate(lit[0]/2, lineNo+1, lit[1], lit[2]);

    }
    return true;
}

bool 
CirMgr::readSymbol(ifstream& ifs)
{
    string buffer;
    while(ifs >> buffer){
        if(buffer[0]=='c') return true;
        size_t idx = stoi(buffer.substr(1));
        string symbol;
        ifs >> symbol;
        if(buffer[0]=='i')
            _gateList[_piIdList[idx]]->setSymbol(symbol);
        else
            _gateList[_miloa[0]+1+idx]->setSymbol(symbol);
    }
    return true;
}

bool 
CirMgr::readComment(ifstream& ifs)
{
    return true;
}

void 
CirMgr::connect()
{
    for(auto g: _gateList){
        if(g)
            g->buildConnect();
    }
}

void 
CirMgr::reset()
{
    for (unsigned i = 1; i < _gateList.size(); ++i) {
        if(_gateList[i])
            delete _gateList[i];
    }
}

void
CirMgr::genDFSList()
{
    CirGate::setGlobalRef();
    for (unsigned i = 0; i < _miloa[3]; ++i) {
        _gateList[_miloa[0]+i+1]->dfsTraversal();
    }
}

void 
CirMgr::genFltList()
{
    for(unsigned i=0; i<_gateList.size(); ++i){
        if(_gateList[i]){
            if(_gateList[i]->isFlt())
                _fltIdList.push_back(i);
            if(_gateList[i]->unUsed() && _gateList[i]->getTypeStr()!="UNDEF")
                _unusedIdList.push_back(i);
        }
    }
}







