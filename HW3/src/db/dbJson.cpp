/****************************************************************************
  FileName     [ DBJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
               [ Modified by Orange Hsu ]
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
    
    return is;
}

ostream &
operator<<(ostream &os, const DBJson &j)
{
    //TODO
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
}
//return false if key is repeated
bool DBJson::add(const DBJsonElem &elm)
{
    //TODO
    return true;
}

// return NAN if DBJson is empty
float DBJson::ave(void) const
{
    //TODO
    return 0.0;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int DBJson::max(size_t &idx) const
{
    //TODO
    int maxN = INT_MAX;
    return maxN;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int DBJson::min(size_t &idx) const
{
    //TODO
    int minN = INT_MAX;
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
    int s = 0; 
    return s;
}


