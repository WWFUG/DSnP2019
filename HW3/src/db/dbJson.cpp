/****************************************************************************
  FileName     [ DBJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include <cstring>
#include "dbJson.h"
#include "util.h"

using namespace std;

#define MAX_LEN 1024 //For char[] use

/*****************************************/
/*          Global Functions             */
/*****************************************/

ostream &
operator<<(ostream &os, const DBJsonElem &j)
{
    return (os << "\"" << j._key << "\" : " << j._value);
}

istream &
operator>>(istream &is, DBJson &j)
{
    // TODO: to read in data from Json file and store them in a DB
    // - You can assume the input file is with correct JSON file format
    // - NO NEED to handle error file format
    assert(j._obj.empty());
    if(!j) j._json_read = true;
    string stmp;
    //convert each line to a DBJsonElem
    while(getline(is,stmp)){
        // handling EOF , since test.cpp assert ifstream not receiving EOF
        if(stmp.find('}')!=string::npos){
            break;
        }
        if( stmp.find('{')!=string::npos || stmp.empty()){
            continue;
        }
        //find key
        size_t key_beg = stmp.find('\"')+1;
        size_t key_end = stmp.rfind('\"')-1;
        string key = stmp.substr(key_beg, key_end-key_beg+1);
        //find value
        size_t colomn = stmp.find(':');
        size_t comma = stmp.find(',');
        string raw_value;
        if(comma!=string::npos){
            raw_value = stmp.substr(colomn+1, comma-colomn-1);
        }
        else{
            raw_value = stmp.substr(colomn+1);
        }
        size_t value_beg = raw_value.find_first_not_of(' '); 
        size_t value_end = raw_value.find_last_not_of(' ');
        int value = stoi(raw_value.substr(value_beg, value_end-value_beg+1));
        j.add(DBJsonElem(key, value));
    }
    return is;
}

ostream &
operator<<(ostream &os, const DBJson &j)
{
    //TODO
    cout << "{\n";
    for (size_t i = 0; i < j.size(); ++i) {
        os << "  " << j[i] ;
        if(i!=j.size()-1) cout << ',';
        cout << '\n';
    } 
    cout << '}';
    return os;
}

/*****************************************/
/*   Member Functions for class DBJsonElem*/
/*****************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/

void DBJson::reset()
{
    //TODO
    _obj = vector<DBJsonElem>();
}
//return false if key is repeated
bool DBJson::add(const DBJsonElem &elm)
{
    //TODO
    for (size_t i = 0; i < _obj.size(); ++i) {
        if(_obj[i].key()==elm.key()) 
            return false;
    }
    _obj.push_back(elm);
    return true;
}

// return NAN if DBJson is empty
float DBJson::ave(void) const
{
    //TODO
    if(_obj.empty()){
        return NAN;
    } 
    return (float)this->sum()/(float)_obj.size();
}

// If DBJson is empty, set idx to size() and return INT_MIN
int DBJson::max(size_t &idx) const
{
    //TODO
    if(this->empty()){
        idx = _obj.size();
        return INT_MIN;
    }
    int maxN = _obj[0].value();
    idx = 0;
    for (size_t i = 1; i < _obj.size(); ++i) {
        if( _obj[i].value() > maxN ){
            idx = i;
            maxN = _obj[i].value();
        }
    }
    return maxN;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int DBJson::min(size_t &idx) const
{
    //TODO
    if(this->empty()){
        idx = _obj.size();
        return INT_MIN;
    }
    int minN = _obj[0].value();
    idx = 0;
    for (size_t i = 1; i < _obj.size(); i++) {
        if(_obj[i].value() > minN){
            idx = i;
            minN = _obj[i].value();
        }
    }
    return minN;
}

void DBJson::sort(const DBSortKey &s)
{
    // Sort the data according to the order of columns in 's'
    ::sort(_obj.begin(), _obj.end(), s);
}

void DBJson::sort(const DBSortValue &s)
{
    // Sort the data according to the order of columns in 's'
    ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int DBJson::sum(void) const
{
    //TODO
    if(this->empty()){
        return 0;
    }
    int s = 0; 
    for (size_t i = 0; i < _obj.size(); i++) {
        s+=_obj[i].value();
    }
    return s;
}


