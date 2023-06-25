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
      _isSorted = true;
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
      iterator& operator ++ () { _node = _node->_next; return *(this); }
      iterator operator ++ (int) { iterator temp = iterator(_node); _node = _node->_next; return temp; }
      iterator& operator -- () { _node = _node->_prev; return *(this); }
      iterator operator -- (int) { iterator temp = iterator(_node); _node = _node->_prev; return temp; }
      iterator& operator = (const iterator& i) { _node = i._node; return *(this); }

      bool operator != (const iterator& i) const { return (_node!=i._node) ? true : false; }
      bool operator == (const iterator& i) const { return (_node==i._node) ? true : false; }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_head->_next); }
   iterator end() const { return iterator(_head); }

   bool empty() const { return (begin()==end()) ? true : false; }
   size_t size() const {
      if(empty())
         return 0;

      size_t count = 0;
      for (iterator it = begin(); it != end(); ++it){
         count++;
      }
      return count; 
   }

   void push_back(const T& x) {
      DListNode<T> *temp = new DListNode<T>(x, _head->_prev, _head);
      _head->_prev->_next = temp;
      _head->_prev = temp;
      _isSorted = false;
   }

   void pop_front() { 
      if(empty())
         return;

      DListNode<T> *temp = _head->_next;
      _head->_next = temp->_next;
      _head->_next->_prev = _head;
      delete temp;
   }

   void pop_back() {
      if(empty())
         return;

      DListNode<T> *temp = _head->_prev;
      _head->_prev = temp->_prev;
      _head->_prev->_next = _head;
      delete temp;
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
      if(empty() || pos==end())
         return false;

      pos._node->_prev->_next = pos._node->_next;
      pos._node->_next->_prev = pos._node->_prev;
      delete pos._node;
      return true; 
   }

   bool erase(const T& x) {
      iterator it = find(x);
      return erase(it);
   }

   iterator find(const T& x) { 
      DListNode<T> *temp = _head->_next;
      while(temp->_data!=x){
         temp = temp->_next;
         if(temp==_head)
            return end();
      }
      
      return iterator(temp);
   }

   void clear() { 
      // delete all nodes except for the dummy node
      if(empty())
         return;
 
      DListNode<T> *cur = _head->_next;
      DListNode<T> *next = cur->_next;
      while(cur != _head){
         delete cur;
         cur = next;
         next = cur->_next;
      }
      _isSorted = true;
      _head->_prev = _head->_next = _head;
   }

   void sort() const {
      if(!empty() && !_isSorted){
         size_t len = size();
         
         //bubble sort
         while(len--){
            for(iterator j = begin(); j != --end();++j){
               if(j._node->_data > j._node->_next->_data){
                  swap_data(j._node,j._node->_next);
               }
            }
         }

         _isSorted = true;
      }
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
   void swap_data (DListNode<T>* a,DListNode<T>* b) const{
      T temp = a->_data;
      a->_data = b->_data;
      b->_data = temp;
   }
};

#endif // DLIST_H
