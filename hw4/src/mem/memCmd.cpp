/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
    if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
          cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
          cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
          cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
       )){
        cerr << "Registering \"mem\" commands fails... exiting" << endl;
        return false;
    }
    return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
    // check option
    string token;
    if (!CmdExec::lexSingleOption(option, token))
       return CMD_EXEC_ERROR;
    if (token.size()) {
        int b;
        if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
            cerr << "Illegal block size (" << token << ")!!" << endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
        }
        #ifdef MEM_MGR_H
        mtest.reset(toSizeT(b));
        #else
        mtest.reset();
        #endif // MEM_MGR_H
    }
    else
       mtest.reset();
    return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
    os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
    cout << setw(15) << left << "MTReset: " 
         << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
    // TODO
    // Use try-catch to catch the bad_alloc exception
    try{
        string test;
        vector<string> options;
        if(!lexOptions(option, options))
            return CMD_EXEC_ERROR;
        if(options.empty())
            return CmdExec::errorOption(CMD_OPT_MISSING, "");
        bool isArr = false;
        int numO = 0; int numA = 0;
        for (size_t i = 0; i < options.size(); ++i) {
            if(myStrNCmp("-Array", options[i], 2)==0){
                if(isArr)
                    return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
                isArr = true;
            }
            else{
                int num;
                if(!myStr2Int(options[i], num))
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
                else{
                    if(num==0)
                        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
                    if(num<0)
                        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
                    if(!isArr){
                        if(numO!=0)
                            return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
                        numO = num;
                    }
                    else{
                        if(numA!=0){
                            if(numO!=0)
                                return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
                            numO = num;
                        }
                        else{
                            numA = num;
                        }
                    }
                }
            }
        }
        //new Objects
        if(isArr){
            if(numA==0||numO==0)
                return CmdExec::errorOption(CMD_OPT_MISSING, "");
            mtest.newArrs(numO, numA);
        }
        else
            mtest.newObjs(numO);
    }
    catch(bad_alloc){
        return CMD_EXEC_ERROR;
    }
    return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
    os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
    cout << setw(15) << left << "MTNew: " 
         << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
    // TODO
    vector<string> options;
    if(!lexOptions(option, options))
        return CMD_EXEC_ERROR;
    if(options.empty())
        return CmdExec::errorOption(CMD_OPT_MISSING, "");
    bool isArr = false;
    int random = 0; // {-1 = -i}, {1 = -r}
    int id = -1;
    for (size_t i = 0; i < options.size() ; ++i) {
        int num = 0;
        if(myStrNCmp("-Array", options[i], 2)==0){
            if(isArr)
                return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
            isArr= true;
        }
        else if(myStrNCmp("-Index", options[i], 2)==0){
            if(random!=0)
                return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
            random = -1;
        }
        else if(myStrNCmp("-Random", options[i], 2)==0){
            if(random!=0)
                return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
            random = 1;
        }
        else if(!myStr2Int(options[i], num)){
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]); 
        }
        else{
            if(num<0)
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
            else{
                if(random!=0){
                    if(id<0)
                        id = num;
                    else
                        return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
                }
                else
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
            }
        }
    }
    if(random==0 || id<0) return CmdExec::errorOption(CMD_OPT_MISSING,"");
    // Index specified, delete object
    if(isArr){
        size_t size = mtest.getArrListSize();
        if(size==0){
            cerr << "Size of array list is 0!!" << endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL,to_string(id));
        }
        if(random==-1){  // Index specified
            if((size_t)id>=size){
                cerr << "Size of array list (" << size << ") is <= " << id << "!!" << endl;
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, to_string(id));
            }
            mtest.deleteArr(id);
        } 
        else{            // randomly generate indexes
            for(int i=0; i<id; ++i){
                size_t idx = rnGen(size);
                mtest.deleteArr(idx);
            }
        }
    }
    else{
        size_t size = mtest.getObjListSize();
        if(size==0){
            cerr << "Size of object list is 0!!" << endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, to_string(id));
        }
        if(random==-1){
            if((size_t)id>=size){
                cerr << "Size of object list (" << size << ") is <= " << id << "!!" << endl;
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, to_string(id));
            }
            mtest.deleteObj(id);
        } 
        else{            // randomly generate indexes
            for(int i=0; i<id; ++i){
                mtest.deleteObj(rnGen(size));
            }
        }
    }
    return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
    os << "Usage: MTDelete <-Index (size_t objId) | "
       << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
    cout << setw(15) << left << "MTDelete: " 
         << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
    if (option.size())
        return CmdExec::errorOption(CMD_OPT_EXTRA, option);
    mtest.print();

    return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
    os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
    cout << setw(15) << left << "MTPrint: " 
         << "(memory test) print memory manager info" << endl;
}


