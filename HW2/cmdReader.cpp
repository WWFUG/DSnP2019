/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <cstring>
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream &);
ParseChar getChar(istream &);

//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void CmdParser::readCmd()
{
    if (_dofile.is_open())
    {
        readCmdInt(_dofile);
        _dofile.close();
    }
    else
        readCmdInt(cin);
}

void CmdParser::readCmdInt(istream &istr)
{
    resetBufAndPrintPrompt();

    while (1)
    {
        ParseChar pch = getChar(istr);
        if (pch == INPUT_END_KEY)
            break;
        switch (pch)
        {
        case LINE_BEGIN_KEY:
            moveBufPtr(_readBuf);
            break;
        case HOME_KEY: 
        case LINE_END_KEY:
            moveBufPtr(_readBufEnd);
            break;
        case END_KEY:
        case BACK_SPACE_KEY:
            if (_readBufPtr > _readBuf)
            {
                moveBufPtr(_readBufPtr - 1);
                deleteChar();
            }
            else
                mybeep();
            break;
        case DELETE_KEY:
            deleteChar();
            break;
        case NEWLINE_KEY:
            addHistory();
            cout << char(NEWLINE_KEY);
            resetBufAndPrintPrompt();
            break;
        case ARROW_UP_KEY:
            moveToHistory(_historyIdx - 1);
            break;
        case ARROW_DOWN_KEY:
            moveToHistory(_historyIdx + 1);
            break;
        case ARROW_RIGHT_KEY:
            moveBufPtr(_readBufPtr + 1);
            break;
        case ARROW_LEFT_KEY:
            moveBufPtr(_readBufPtr - 1);
            break;
        case PG_UP_KEY:
            moveToHistory(_historyIdx - PG_OFFSET);
            break;
        case PG_DOWN_KEY:
            moveToHistory(_historyIdx + PG_OFFSET);
            break;
        case TAB_KEY:
            insertChar(' ', TAB_POSITION);
            break;
        case INSERT_KEY: // not yet supported; fall through to UNDEFINE
        case UNDEFINED_KEY:
            mybeep();
            break;
        default: // printable character
            insertChar(char(pch));
            break;
        }
#ifdef TA_KB_SETTING
        taTestOnly();
#endif
    }
}

// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool CmdParser::moveBufPtr(char *const ptr)
{
    if(ptr<_readBuf || ptr>_readBufEnd){
        mybeep();
        return false;
    }
    int ndir = ptr - _readBufPtr;//number of dif. and direction
    if(ndir>0){
        for (int i = 0; i < ndir; ++i) {
            cout << *_readBufPtr;
            ++_readBufPtr;
        }
    }
    else{
        for (int i = 0; i < -ndir; ++i) {
            cout << '\b' ;
            --_readBufPtr;
        }
    }    
    return true;
}

// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool CmdParser::deleteChar()
{
    if(_readBufPtr==_readBufEnd){
        mybeep();
        return false;
    }
    int cnt = 0;
    for (size_t i = _readBufPtr-_readBuf; i < strlen(_readBuf); ++i,++cnt) {
        _readBuf[i] = _readBuf[i+1];
        cout << _readBuf[i];
    }
    cout << ' ';
    _readBufEnd--;*_readBufEnd = '\0';
    for (int i = 0; i < cnt ; ++i) {
        cout << '\b';
    }
    return true;
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
//
void CmdParser::insertChar(char ch, int repeat)
{
    assert(repeat>=1);
    int idx = _readBufPtr - _readBuf;//cursor idx before shifted
    char tmp[strlen(_readBuf)-idx+1];//store to-be-right-shifted string

    for(size_t i = idx; i<strlen(_readBuf); ++i){
        tmp[i-idx] = _readBuf[i];
    }
    tmp[strlen(_readBuf)-idx] = '\0';

    for (int i = idx; i < idx+repeat;++i) {
        _readBuf[i] = ch;
        cout << ch;
    }
    _readBuf[idx+repeat] = '\0';//for strcat

    strcat(_readBuf, tmp);
    cout << tmp;
    for(size_t i=0;i<strlen(tmp);++i){
        cout << '\b';
    }
    _readBufEnd+=repeat;*_readBufEnd = '\0';
    _readBufPtr+=repeat; 
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void CmdParser::deleteLine()
{
    for (int i = 0; i < _readBufPtr-_readBuf; ++i) {
        cout << '\b';
    }
    size_t buf_len = strlen(_readBuf);
    for(size_t i = 0; i<buf_len ; ++i){
        cout << ' ';
        _readBuf[i] = '\0';
    }
    _readBufPtr = _readBufEnd =  _readBuf+buf_len;
    moveBufPtr(_readBuf);  
    _readBufEnd = _readBuf;
    if(*_readBufEnd!='\0') *_readBufEnd = '\0';
}

// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
// Index from 0 to _historyIdx_
//
void CmdParser::moveToHistory(int index)
{
    if( (_historyIdx==0 && index<_historyIdx) || (_historyIdx==(int)_history.size() && index>_historyIdx)){
        mybeep();
        return;
    }
    if(index<_historyIdx){
        if(index<0) index = 0;
        else if(_historyIdx==(int)_history.size()){
            if(!_tempCmdStored){
                _history.push_back(string(_readBuf));
                _tempCmdStored = true;
            }
        }
        
    }
    else{
        if(index>=(int)_history.size()){
            index = _history.size()-1;
        }
    }
    _historyIdx = index;
    retrieveHistory();

}

// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
//
void CmdParser::addHistory()
{   
    string buf_str = string(_readBuf); 
    size_t start = buf_str.find_first_not_of(' '); // find the first not ' ' char
    size_t end = buf_str.find_last_not_of(' ');    // find the last not ' ' char
    if(start!=end || start!=string::npos){                             
        buf_str = buf_str.substr(start, end-start+1); 
    }
    else{
        buf_str = "";
    }
    if(buf_str!=""){ 
        if(_tempCmdStored){
            _history.pop_back();
            _tempCmdStored = false;
        }
        _history.push_back(buf_str);
    }
    _historyIdx = _history.size();
}

// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void CmdParser::retrieveHistory()
{
    deleteLine();
    strcpy(_readBuf, _history[_historyIdx].c_str());
    cout << _readBuf;
    _readBufPtr = _readBufEnd = _readBuf+strlen(_readBuf);
    /*
    cout << endl;
    cout << "========retrieveHostory=========\n" ;
    for(int i = 0; i<(int)_history.size(); ++i){
        cout << i <<  " : " << _history[i] << endl;
    }
    */
}
