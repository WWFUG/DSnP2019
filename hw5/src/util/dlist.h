/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
      _isSorted = false;
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { 
          _node = _node->_next;
          return *(this); 
      }
      iterator operator ++ (int) { 
          iterator tmp = *this;
          _node = _node->_next;
          return tmp; 
      }
      iterator& operator -- () { 
          _node = _node->_prev;
          return *(this); 
      }
      iterator operator -- (int) { 
          iterator tmp = *this;
          _node = _node->_prev;
          return tmp; 
      }

      iterator& operator = (const iterator& i) { 
          _node = i._node;
          return *this;
      }
      bool operator != (const iterator& i) const { return this->_node!=i._node; }
      bool operator == (const iterator& i) const { return this->_node==i._node; }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_head); }
   iterator end() const { return iterator(_head->_prev);}
   bool empty() const { return _head==_head->_prev; }
   size_t size() const {  
       size_t num = 0;
       DListNode<T>* iter = _head->_next;
       while(iter!=_head){
           iter = iter->_next;
           ++num;
       }
       return num; 
   }

   void push_back(const T& x) {
       if(empty()){
           DListNode<T>* dummy = _head;
           _head = new DListNode<T>(x, dummy, dummy);
           dummy->_next = dummy->_prev = _head;
           return ;
       }
       DListNode<T>* dummy = _head->_prev;
       DListNode<T>* back = dummy->_prev;
       DListNode<T>* pushed = new DListNode<T>(x, back, dummy);
       back->_next = pushed; 
       dummy->_prev = pushed; 
       //print_verbose();
   }
   void pop_front() {
       if(empty()) return;
       DListNode<T>* popped = _head;
       _head = _head->_next;
       _head->_prev = popped->_prev;
       popped->_prev->_next = _head;
       delete popped;
   }
   void pop_back() { 
       if(empty()) return;
       DListNode<T>* popped = _head->_prev->_prev;
       if(popped==_head){
           _head = _head->_prev;
           _head->_next = _head->_prev = _head;
       }
       popped->_prev->_next = _head->_prev;
       _head->_prev->_prev = popped->_prev;
       delete popped;
   }

   // return false if nothing to erase
   bool erase(iterator pos) { 
       if(empty()) return false;
       DListNode<T>* prev = pos._node->_prev;
       DListNode<T>* next = pos._node->_next;
       prev->_next = next;
       next->_prev = prev;
       if(pos._node==_head) _head = next;
       delete pos._node;
       return true;
   }
   bool erase(const T& x) { 
       if(empty()) return false;
       DListNode<T>* iter = _head;
       while(true){
           if(iter->_data==x){ 
               DListNode<T>* prev = iter->_prev;
               DListNode<T>* next = iter->_next;
               prev->_next = next;
               next->_prev = prev;
               if(iter==_head) _head = next;
               delete iter;
               return true;
           }
           iter = iter->_next; 
           if(iter==_head) return false;
       }
       return false; 
   }

   iterator find(const T& x) { 
       DListNode<T>* iter = _head;
       while(true){
           if(iter->_data==x) return iterator(iter);
           iter = iter->_next;
           if(iter==_head) return end();
       }
   }

   void clear() {
       DListNode<T>* iter = _head->_next; 
       DListNode<T>* term = _head;
       while(iter!=term){
           delete _head;
           _head = iter;
           iter = iter->_next;
       }
       _head->_next = _head->_prev = _head;
   }  // delete all nodes except for the dummy node

   void sort() const{ 
       bubble_sort(); 
   }


private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
   void merge_sort(){
   }  
   
   void bubble_sort() const{
       if(empty()) return ;
       unsigned num = size();
       DListNode<T>* iter = _head;
       for (unsigned i = 0; i < num-1; ++i) {
           T &now = iter->_data;
           T &next = iter->_next->_data;
           for (unsigned j = 0; j < num-i-1; ++j) {
               if(now>next) swap(now,next);
           }
           iter = iter->_next;
       }
   }
   void swap (T &a, T &b) const{
       T tmp = a;
       a = b;
       b = tmp;
   }

};

#endif // DLIST_H
