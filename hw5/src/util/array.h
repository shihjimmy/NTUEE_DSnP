/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0),_isSorted(true) {}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {} // copy constructor
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*_node); } // dereference
      T& operator * () { return (*_node); }
      iterator& operator ++ () { _node++; return (*this); }
      iterator operator ++ (int) { iterator temp = iterator(_node); _node++; return temp; }
      iterator& operator -- () { _node--; return (*this); }
      iterator operator -- (int) { iterator temp = iterator(_node); _node--; return temp; }
      iterator operator + (int i) const { return iterator(_node + i); }
      iterator operator - (int i) const { return iterator(_node - i); }
      iterator& operator += (int i) { _node+=i; return (*this); }
      iterator& operator = (const iterator& i) { _node = i._node; return (*this); } // assignment operator

      bool operator != (const iterator& i) const { return !(_node == i._node); }
      bool operator == (const iterator& i) const { return _node == i._node; }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_data); }   
   iterator end() const { return (!_capacity) ? iterator(0) : iterator(_data + _size); }
   bool empty() const { return (!_size) ? true : false; }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { assert(i < _size); return _data[i]; }
   const T& operator [] (size_t i) const { assert(i < _size); return _data[i]; }

   void push_back(const T& x) { 
      if( _size == _capacity){
         if(_capacity == 0){
            _capacity++;
            _data = new T[_capacity];
         }
         else{
            _capacity *= 2;
            T *temp = new T[_capacity];
            for (int i = 0; i < _size;i++){
               temp[i] = _data[i];
            }
            delete[] _data;
            _data = temp;
         }
      }
      _data[_size++] = x;
      _isSorted = false;
   }

   void pop_front() {
      if(_size==0)
         return;

      if(_size >= 2){
         _data[0] = _data[--_size];
         _isSorted = false;
      }
   }

   void pop_back() {
      if(_size==0)
         return;
      _size--;
   }

   bool erase(iterator pos) { 
      if (pos._node < _data || pos._node >= (_data + _size)) {
         return false;
      }

      *pos = *(end()-1);
      _size--;
      return true;
   }

   bool erase(const T& x) {
      return erase(find(x));
   }

   iterator find(const T& x) {
      for (iterator it = begin(); it != end();++it){
         if((*it)==x)
            return it;
      }
      return end(); 
   }

   void clear() { _size = 0; _isSorted = true;}

   // [Optional TODO] Feel free to change, but DO NOT change ::sort()
   void sort() const { if (!empty() && !_isSorted) ::sort(_data, _data+_size); _isSorted = true;}

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] Helper functions; called by public member functions
};

#endif // ARRAY_H
