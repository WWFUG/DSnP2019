/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
      iterator(vector<Data>* b, size_t ii, size_t bi, size_t numb) : _buckets(b), _itemIdx(ii), _buckIdx(bi), 
                                                                     _numB(numb) {}
      iterator(const iterator& i) : _buckets(i._buckets), _itemIdx(i._itemIdx), _buckIdx(i._buckIdx),
                                    _numB(i._numB) {}
      const Data& operator * () const { 
          return _buckets[_buckIdx][_itemIdx]; 
      }
      iterator& operator ++ () {
          int size = _buckets[_buckIdx].size();
          if(++_itemIdx < size || _buckIdx==_numB) {return *this;}
          _itemIdx = 0;
          while((++_buckIdx)<_numB && _buckets[_buckIdx].empty()){}
          return *this;
      }
      iterator operator ++ (int){
          iterator ret = *this;
          ++*this;
          return ret;
      }
      iterator& operator --(){
          int size = _buckets[_buckIdx].size();
          if(--_itemIdx >= 0) return *this;
          size_t tmp = _buckIdx;
          while(!size){
              if(!_buckIdx){ // is begin();
                  ++_itemIdx;
                  _buckIdx = tmp;
                  return *this;
              } 
              size = _buckets[--_buckIdx]->size();
          }
          _itemIdx = 0;
          return *this;
      }
      iterator operator -- (int){
          iterator ret = *this;
          --*this;
          return ret;
      }
      iterator& operator = (const iterator& i){ 
          _buckets = i._buckets; _itemIdx = i._itemIdx; _buckIdx = i._buckIdx; _numB = i._numB;
      }
      bool operator != (const iterator& i) const { return ((_buckets!=i._buckets) || 
                                                           (_itemIdx!=i._itemIdx) || 
                                                           (_buckIdx!=i._buckIdx)); 
      }
      bool operator == (const iterator& i) const { return !(*this!=i) ; }

   private:
      vector<Data>* _buckets;
      size_t _itemIdx;
      size_t _buckIdx;
      size_t _numB;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const { 
       for (int i = 0; i < _numBuckets; ++i) {
           if(!_buckets[i].empty())
               return iterator(_buckets, 0, i, _numBuckets);
       }
       return iterator(_buckets, 0, _numBuckets, _numBuckets); 
   }
   // Pass the end
   iterator end() const { 
       return iterator(_buckets, 0, _numBuckets, _numBuckets);
   }
   // return true if no valid data
   bool empty() const { return begin()==end(); }
   // number of valid data
   size_t size() const { 
       size_t i = 0;
       for(iterator it=begin(); it!=end(); ++it,++i ){}
       return i;
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const { 
       size_t idx = bucketNum(d);
       for(int i=0; i<_buckets[idx].size(); ++i){
           if(_buckets[idx][i]==d)
               return true;
       }
       return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const { 
       size_t idx = bucketNum(d);       
       for(int i=0; i<_buckets[idx].size(); ++i){
           if(_buckets[idx][i]==d){
               d = _buckets[idx][i];
               return true;
           }
       }
       return false; 
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) { 
       size_t idx = bucketNum(d);
       for (int i = 0; i < _buckets[idx].size(); i++) {
           if(_buckets[idx][i]==d){
               _buckets[idx][i]=d;
               return true;
           }
       }
       _buckets[idx].push_back(d);
       return false; 
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) { 
       size_t idx = bucketNum(d);
       for (int i = 0; i < _buckets[idx].size(); i++) {
           if(_buckets[idx][i]==d)
               return false;
       }
       _buckets[idx].push_back(d);
       return true; 
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) { 
       if(!check(d)) return false;
       size_t idx = bucketNum(d);
       for (int i=0; i<_buckets[idx].size(); ++i) {
           if(_buckets[idx][i]==d){
               _buckets[idx].erase(_buckets[idx].begin()+i);
               return true;
           } 
       }
       return false; 
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
