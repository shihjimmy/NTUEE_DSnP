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
#include <vector>

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
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;

   BSTreeNode() {}
   BSTreeNode(const T& d,BSTreeNode<T>*l = 0, BSTreeNode<T>*r = 0):
      _data(d), _left(l), _right(r) {}
   ~BSTreeNode() {}

   T _data;
   BSTreeNode<T> *_left;
   BSTreeNode<T> *_right;
};

template <class T>
class BSTree
{
public:
   // TODO: design your own class!!
   BSTree(BSTreeNode<T>* p=0): _root(p){}
   ~BSTree() { clear(_root); }

   class iterator 
   {
      friend class BSTree;

   public:
      iterator(BSTreeNode<T> *p = 0) : _node(p) {
         _trace.clear();
         _directions.clear();
         _trace.push_back(p);
      }
      iterator(const iterator& i): _node(i._node), _trace(i._trace), _directions(i._directions) {}
      ~iterator() {}

      const T &operator*() const { return _node->_data; }
      T& operator * () { return _node->_data; }

      iterator& operator ++ () {
         if(!_node)
            return (*this);

         BSTreeNode<T> *tmp = BSTree::successor(_node);
         if(!tmp){
            // need to find from upper level
            bool dir;
            while(_directions.size()){
               // check if we go to root
               dir = _directions.back();
               _directions.pop_back();
               _node = _trace.back();
               _trace.pop_back();

               if(!dir){
                  // find a left path
                  return (*this);
               }
            }

            _node = 0; // not found
         }
         else{
            go_right();

            while(_node!=tmp){
               go_left();
            }
         }

         return (*this);
      }

      iterator& operator -- () {
         if(!_node){
            // ex: when --end()
            // we need to find max(_root)
            // get root
            _node = _trace.back();
            if(_node){ 
               // root
               while (_node->_right){
                  go_right();
               }
            }
            return (*this);
         }
        
         BSTreeNode<T> *tmp = BSTree::predecessor(_node);
         if(!tmp){
            // need to find from upper level
            bool dir;
            while(_directions.size()){
               // check if we go to root
               dir = _directions.back();
               _directions.pop_back();
               _node = _trace.back();
               _trace.pop_back();

               if(dir){
                  // find a right path
                  return (*this);
               }
            }

            _node = 0; // not found
         }
         else{
            go_left();

            while(_node!=tmp){
               go_right();
            }
         }

         return (*this);
      }
      
      iterator operator ++ (int) {iterator tmp = iterator(_node); ++(*(this)); return tmp;}
      iterator operator -- (int) {iterator tmp = iterator(_node); --(*(this)); return tmp;}

      iterator& operator = (const iterator& i) {
         _node = i._node;
         _trace = i._trace;
         _directions = i._directions;
         return (*this);
      }
      bool operator != (const iterator &i) const { return (_node != i._node) ? true : false; }
      bool operator == (const iterator& i) const {return (_node == i._node) ? true : false; }

   private:
      BSTreeNode<T> *_node;
      vector<BSTreeNode<T> *> _trace;
      vector<bool> _directions;  //  false->left  && right->true

      // return object& allows the caller 
      // to modify the state of the iterator object through method chaining
      // ex: goleft().goright()
      iterator& go_left() {
         _trace.push_back(_node);
         _node = _node->_left;
         _directions.push_back(false);
         return (*this);
      }

      iterator& go_right() {
         _trace.push_back(_node);
         _node = _node->_right;
         _directions.push_back(true);
         return (*this);
      }
      
   };

   iterator begin() const {
      iterator it(_root);

      if(_root){
         while(it._node->_left)
            it.go_left();
      }
      return it;
   }

   iterator end() const { 
      iterator it(0);
      it._trace.push_back(_root);
      return it;
   }

   bool empty() const { return (!_root) ? true : false; }
   size_t size() const{
      size_t count = 0;
      for (iterator it = begin(); it != end(); ++it){
         count++;
      }
      return count;
   }

   void insert(const T& x) {
      BSTreeNode<T>* newnode = new BSTreeNode<T>(x);
      
      if(empty()){
         _root = newnode;
         return;
      }

      BSTreeNode<T> *pos = _root;

      while(1){
         if(pos->_data >= x){
            // go left
            if(pos->_left)
               pos = pos->_left;
            else{
               pos->_left = newnode;
               return;
            }
         }
         else{
            // go right
            if(pos->_right)
               pos = pos->_right;
            else{
               pos->_right = newnode;
               return;
            }
         }
      }
   }

   // pop the minimum(front) / maximum(back) element
   void pop_front() { if(!empty()) erase(begin()); }
   void pop_back() { if(!empty()) erase(--end()); }
   
   bool erase(const T& x) {
      iterator it = find(x);

      if(it==end())
         return false;
      return erase(it);
   }

   bool erase(iterator pos) {
      BSTreeNode<T> *parent = pos._trace.back();
      iterator tmp = pos;

      if (pos._node->_left && pos._node->_right){
         // two subtrees
         *pos = *(++tmp); // copy successor's data to pos
         erase(tmp);
      }
      else if(pos._node->_left || pos._node->_right){
         // one one subtree
         if(pos._node->_left)
            tmp.go_left();
         else if(pos._node->_right)
            tmp.go_right();
         
         if(pos._node->_data == _root->_data)
            _root = tmp._node;
         else if(!pos._directions.back())
            parent->_left = tmp._node;
         else
            parent->_right = tmp._node;
         
         delete pos._node;
      }
      else{
         // leaf node
         if(pos._node->_data == _root->_data)
            _root = 0;
         else if(!pos._directions.back())
            parent->_left = 0;
         else
            parent->_right = 0;
         delete pos._node;
      }

      return true;
   }

   iterator find(const T& x) const {
      for (iterator it = begin(); it != end();++it){
         if(it._node->_data == x)
            return it;
      }
      return end();
   }

   void clear() {
      clear(_root);
   }

   void clear(BSTreeNode<T>* &_node) {
      if(!_node)
         return;

      clear(_node->_left);
      clear(_node->_right);
      delete _node;
      _node = 0;
   }
  
   // BST is already sorted!
   void sort () const {};
   void print() const {};

private:
   BSTreeNode<T> *_root;

   // return max/min element in the subtree
   static BSTreeNode<T>* max(BSTreeNode<T>* _node){
      if(!_node)
         return NULL;

      BSTreeNode<T> *tmp = _node;
      while (tmp->_right)
         tmp = tmp->_right;
      return tmp;
   }

   static BSTreeNode<T>* min(BSTreeNode<T>* _node){
      if(!_node)
         return NULL;

      BSTreeNode<T> *tmp = _node;
      while (tmp->_left)
         tmp = tmp->_left;
      return tmp;
   }

   // return successor/predecessor in the subtree
   static BSTreeNode<T>* successor(BSTreeNode<T>* _node){
      if(!_node)
         return NULL;

      if(!_node->_right)
         return NULL;
      return min(_node->_right);
   }

   static BSTreeNode<T>* predecessor(BSTreeNode<T>* _node){
      if(!_node)
         return NULL;

      if(!_node->_left)
         return NULL;
      return max(_node->_left);
   }

};

#endif // BST_H
