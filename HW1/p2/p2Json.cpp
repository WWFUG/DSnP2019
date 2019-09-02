#include <vector>
#include <string>
#include <fstream>
#include "p2Json.h"

bool Json::line2Elem(const string& str){
    if(str.find('{') < str.length() || str.find('}') < str.length() || str.empty()) return 0;
    //find the key
    const size_t key_beg = str.find('\"')+1;
    const size_t key_end = str.rfind('\"')-1;
    const string key = str.substr( key_beg, key_end-key_beg+1 );
    //find the value
    const size_t comma = str.find(',');
    const size_t colon = str.find(':');
    string raw_value;
    int value;
    if(comma!=string::npos){
        raw_value = str.substr( colon+1, comma-colon-1);
    }
    else{
        raw_value = str.substr(colon+1);
    }
    const size_t val_beg = raw_value.find_first_not_of(" ");
    const size_t val_end = raw_value.find_last_not_of(" "); 
    value = stoi(raw_value.substr( val_beg, val_end-val_beg+1 ));
    JsonElem new_elem(key, value);
    _obj.push_back(new_elem);
    return 1;
}

bool Json::read(const string& fname){
    fstream fin(fname);
    string stmp;
    if(!fin) { cout << "Read File Error" << endl; return 0;} 
    while(getline(fin, stmp)){
        this->line2Elem(stmp);
    }
    fin.close();
    return 1;
}

void Json::print() const{
    cout << "{"<< endl;
    for (size_t i = 0; i < _obj.size(); ++i) {
        cout << "  " << _obj[i];
        if(i<_obj.size()-1) cout << ",";
        cout << endl;
    }
    cout << "}" << endl;
}

bool Json::isEmpty() const{
    if(_obj.empty()){
        cout << "Error: No element found!!" << endl;
        return 1;
    }
    return 0;
}

bool Json::add(const string& key, int value){ 
    _obj.push_back(JsonElem(key , value));
    return 1;
}

int Json::sum() const{
    this->isEmpty();
    int sum = 0;
    for (size_t i = 0; i < _obj.size(); ++i) {
        sum+=_obj[i].getVal();        
    }
    return sum;
}

float Json::ave() const{
    this->isEmpty();
    return ((float)this->sum()/(float)this->_obj.size());
}

const JsonElem& Json::max() const{
    this->isEmpty();
    size_t idx = 0;
    int max = _obj[0].getVal();
    for (size_t i = 1; i < _obj.size(); ++i) {
        if( _obj[i].getVal()>max ){
            idx = i;
            max = _obj[i].getVal();
        }
    }
    return _obj[idx];
}

const JsonElem& Json::min() const{
    this->isEmpty();
    size_t idx = 0;
    int min = _obj[0].getVal();
    for (size_t i = 1; i < _obj.size(); ++i) {
        if( _obj[i].getVal()<min ){
            idx = i;
            min = _obj[i].getVal();
        }
    }
    return _obj[idx];  
}
ostream& operator << (ostream& os, const JsonElem& ele){
    os << "\"" <<  ele.getKey() << "\"" << " : " << ele.getVal();
    return os;
}
