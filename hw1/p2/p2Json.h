#ifndef P2JSON_H
#define P2JSON_H
#include <vector>
#include <string>
#include <iostream>
using namespace std;
class Json;
class JsonElem;

class Json
{
private:
    vector<JsonElem> _obj;
    bool line2Elem(const string&);

public:
    Json(){};
    bool isEmpty() const;
    bool read(const string&);
    bool add(const string&, int);
    int sum() const;
    const JsonElem& max() const;
    const JsonElem& min() const;
    float ave() const;
    void print() const;

};

class JsonElem
{
private:
    string _key;
    int _value;
public:
    JsonElem ();
    JsonElem(const string& k, int v): _key(k), _value(v) {};
    string getKey() const {return _key;};
    int getVal() const {return _value;};
    friend ostream& operator << (ostream&, const JsonElem&);
    
};
#endif /* P2JSON_H */
