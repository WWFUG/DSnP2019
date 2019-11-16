/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
    friend class BSTree<T>;
    friend class BSTree<T>::iterator;
    BSTreeNode<T>(const T& d, BSTreeNode<T>* L = 0, BSTreeNode<T>* R = 0, BSTreeNode<T>* P = 0, int dir = -1) :
    _data(d), _left(L), _right(R), _parent(P), _dir(dir){}

    // TODO: design your own class!!
    T              _data;
    BSTreeNode<T>* _left;
    BSTreeNode<T>* _right;
    BSTreeNode<T>* _parent;
    int            _dir; // recording the dir parent goes to current node(left(0)/right(1)/root(-1))
};


template <class T>
class BSTree
{
public:
    BSTree () : _root(0), _size(0){
    } // TODO: design your own class!!
    ~BSTree(){clear();}

    class iterator 
    {
        friend class BSTree;
    public:
        iterator(BSTreeNode<T>* n = 0, BSTreeNode<T>* r = 0): _node(n), _root(r){}
        iterator(const iterator& t): _node(t._node){}
        ~iterator(){};
        const T& operator* () const { return _node->_data;}
        T& operator* (){ return _node->_data;}
        iterator& operator++(){
            // if not end()
            if(_node){
                // if has right subtree, find successor
                if(_node->_right)
                    _node = successor(_node);
                else{
                    // find the next rightest root
                    while(_node->_dir==1){
                        _node = _node->_parent;
                    }
                    if(_node->_dir==-1) // this node is max(), return end()=0;
                        _node = 0;
                    else 
                        _node = _node->_parent; 
                }
            }
            return *(this);
        }
        iterator operator++(int){
            iterator t = *this;
            ++(*this);
            return t;
        }
        iterator& operator--(){
            BSTreeNode<T>* n = _node;
            if(_node){
                if(_node->_left)
                    _node = predecessor(_node);
                else{
                    while(_node->_dir==0){
                        _node = _node->_parent;
                    }
                    if(_node->_dir==-1) // if _node == begin()
                        _node = n;
                    else
                        _node = _node->_parent;
                }
            }
            else{
                //end()
                _node = max(_root);
            }
            return *this;
        }
        iterator operator--(int){
            iterator t = *this;
            --(*this);
            return t;
        }
        iterator& operator = (const iterator& i){
            _node = i._node;
            return *this; 
        }
        bool operator == (const iterator& i) const { return this->_node==i._node; }
        bool operator != (const iterator& i) const { return this->_node!=i._node; }

    private:
        BSTreeNode<T>* _node;
        BSTreeNode<T>* _root; //only for end()
    };
    iterator begin() const {
        if(empty()) return iterator(0, _root);
        return iterator(min(_root));
    } // return min(i.e. left most node)
    iterator end() const {return iterator(0, _root);}
    bool empty() const { return _size==0;}
    size_t size() const { return _size;}
    void insert(const T& x){
        if(empty()){ 
            _root = new BSTreeNode<T>(x); 
            ++_size;
            return ;
        }
        BSTreeNode<T>* iter = _root;
        while(true){
            int dir = 0;
            BSTreeNode<T>* root = iter;
            if( x>=iter->_data){
                iter = iter->_right;
                dir = 1;
            }
            else{
                iter = iter->_left;
                dir = 0;
            }
            if(!iter){
                if(dir==1)
                    root->_right = new BSTreeNode<T>(x, 0, 0, root, dir);
                else
                    root->_left = new BSTreeNode<T>(x, 0, 0, root, dir);
                //print();
                break;
            }
        }
        ++_size;
    }
    void pop_front(){
        if(delete_node(min(_root)))
            --_size;
    }
    void pop_back(){
        if(delete_node(max(_root)))
            --_size;
    }
    bool erase(const T& x){
        BSTreeNode<T>* finded = search(x);
        if(finded) {
            delete_node(finded);
            --_size;
            return true;
        }
        return false;
    }
    bool erase(iterator pos){
        if(delete_node(pos._node)){
            --_size;
            return true;
        }
        return false;
    }
    iterator find(const T& x) const{
        return iterator(search(x));
    }
    void clear(){
        clear_tree(_root);
        _size = 0;
        _root = 0;
    }
    // Pre-Order print 
    void print() const {
        pre_order_print(_root, 1);
    }
    void sort(){}
private:
    BSTreeNode<T>* _root;
    size_t         _size;

    // return its next largest element in subtree
    static BSTreeNode<T>* successor(BSTreeNode<T>* root) {
        return min(root->_right);
    }
   
    // return its previous largest element in subtree 
    static BSTreeNode<T>* predecessor(BSTreeNode<T>* root) {
        return max(root->_left);
    }

    static BSTreeNode<T>* min(BSTreeNode<T>* root) {
        if(!root) return 0;
        while(root->_left)
            root = root->_left;
        return root;
    }

    static BSTreeNode<T>* max(BSTreeNode<T>* root) {
        if(!root) return 0;
        while(root->_right)
            root = root->_right;
        return root;
    }

    bool delete_node(BSTreeNode<T>* node){
        if(!node) return false;
        // if root
        if(node==_root) {
            if(_size==1){
                delete _root;
                _root = 0;
                return true;
            }
            else if(_root->_left && _root->_right){
                BSTreeNode<T>* succ = successor(_root);
                _root->_data = succ->_data;
                delete_node(succ);
            }
            else{
                BSTreeNode<T>* tmp = _root;
                if(_root->_left)
                    _root = _root->_left;
                else
                    _root = _root->_right;
                delete tmp;
            }
            _root->_parent = 0;
            _root->_dir = -1;
            return true;
        }
        // if not root
        if(node->_left && node->_right){
            BSTreeNode<T>* succ = successor(node);
            node->_data = succ->_data;
            delete_node(succ);
        }
        else if(!node->_left && !node->_right){
            if(node->_dir==0)
                node->_parent->_left = 0;
            else 
                node->_parent->_right = 0;
            delete node;
        }
        else{
            link(node);
            delete node;
        }
        return true;
    }

    BSTreeNode<T>* search (const T& x) const{
        if(empty()) return 0;
        BSTreeNode<T>* iter = _root;
        while(iter){
            if( x>iter->_data )
                iter = iter->_right;
            else if( x<iter->_data )
                iter = iter->_left;
            else if( x==iter->_data )
                return iter;
        } 
        return 0;
    }

    void clear_tree(BSTreeNode<T>* root){
        if(!_root) return;
        if(root->_left!=0){
            clear_tree(root->_left);
        }
        if(root->_right!=0){
            clear_tree(root->_right);
        }
        delete root;
    }

    void pre_order_print(BSTreeNode<T>* root, int level = 1) const{
        if(!root) return;
        cout << root->_data << " " << root << "\n";
        for(int i=0; i<level; ++i) cout << " ";
        if(root->_left!=0){
            pre_order_print(root->_left, level+1);
        }
        else{
            cout << "[0]" << endl;
        }
        for(int i=0; i<level; ++i) cout << " ";
        if(root->_right!=0){
            pre_order_print(root->_right, level+1);
        }
        else{
            cout << "[0]" << endl;
        }
    }

    // link its parent and child , this node should have exact 1 child
    void link(BSTreeNode<T>* node){
        if(!node->_parent) return;
        if(node->_left && !node->_right){
            if(node->_dir==0){
                node->_left->_parent = node->_parent;
                node->_parent->_left = node->_left;
            }
            else{
                node->_left->_parent = node->_parent;
                node->_parent->_right = node->_left;
                node->_left->_dir = 1;
            }
        }
        else if(!node->_left && node->_right){
            if(node->_dir==0){
                node->_right->_parent = node->_parent;
                node->_parent->_left = node->_right;
                node->_right->_dir = 0;
            }
            else{
                node->_right->_parent = node->_parent;
                node->_parent->_right = node->_right;
            }
        }
        else return;
    }

};

#endif // BST_H