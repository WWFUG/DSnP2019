/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d) { 
       int n = _data.size()+1;
       _data.push_back(d); 
       int p = n;
       while(n>1){
           p/=2;
           if(!(_data[n-1] < _data[p-1]))
               break;
           Data tmp = _data[n-1];
           _data[n-1] = _data[p-1];
           _data[p-1] = tmp;
           n = p;
       }
   }
   void delMin() { 
       delData(0);
   }
   void delData(size_t i) {
       int n = _data.size();
       int p = i+1;
       int t = p*2;
       _data[p-1] = _data[n-1];
       _data.pop_back();
       while( t<=n ){
           if(t<n)
              if(!(_data[t-1] < _data[t]))
                  ++t;
           if(!(_data[t-1] < _data[p-1]))
               break;
           Data tmp = _data[p-1];
           _data[p-1] = _data[t-1];
           _data[t-1] = tmp;
           p = t;
           t = 2*p;
       }
       t = p/2;
       while(t > 1){
           if(_data[t-1] < _data[p-1])
               break;
           Data tmp = _data[p-1];
           _data[p-1] = _data[t-1];
           _data[t-1] = tmp;
           p = t;
           t = p/2;
       }
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
