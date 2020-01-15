/****************************************************************************
  FileName     [ cirDef.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic data or var for cir package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_DEF_H
#define CIR_DEF_H

#include <vector>
#include "myHashMap.h"

using namespace std;

// TODO: define your own typedef or enum

class CirGate;
class CirMgr;
class AIGate;
class PIGate;
class POGate;
class CONST;
class UNDEF;
class GateV;
class SatSolver;

typedef vector< CirGate* >     GateList; 
typedef vector< unsigned >     IdList;
typedef vector< GateV >        GateVList;

#endif // CIR_DEF_H
