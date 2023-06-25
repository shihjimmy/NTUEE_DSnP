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
      _data.push_back(d);
      int n = _data.size();

      while(true){
         int parent = n / 2;
         if (parent >= 1 && _data[n - 1] < _data[parent - 1]){
            swap(_data[n - 1], _data[parent - 1]);
            n = parent;
         }
         else
            break;
      }
   }

   void delMin() {
      swap(_data[0], _data[_data.size() - 1]);
      _data.pop_back();

      int size = _data.size();
      int cur = 1;
      while(true){
         int left = 2 * cur;
         int right = 2 * cur + 1;

         if(left > size && right > size)
            break;

         if(right > size){
            if(_data[left-1] < _data[cur-1])
               swap(_data[left - 1], _data[cur - 1]);
            break;
         }

         if(_data[left-1] < _data[cur-1] || _data[right-1] < _data[cur-1]){
            if(_data[left-1] < _data[cur-1]){
               if(_data[left-1] < _data[right-1]){
                  swap(_data[left - 1], _data[cur - 1]);
                  cur = left;
               }
               else {
                  swap(_data[right - 1], _data[cur - 1]);
                  cur = right;
               }
            }
            else if(_data[right-1] < _data[cur-1]){
               if(_data[left-1] < _data[right-1]){
                  swap(_data[left - 1], _data[cur - 1]);
                  cur = left;
               }
               else {
                  swap(_data[right - 1], _data[cur - 1]);
                  cur = right;
               }
            }
         }
         else
            break;
      }
   }

   void delData(size_t i) {
      vector<Data> _new;
      for (int t = 0; t < _data.size();t++){
         if(t!=i)
            _new.push_back(_data[t]);
      }

      _data.clear();

      for(auto& data : _new)
         insert(data);
   }

   void swap(Data& a,Data& b){
      Data temp = a;
      a = b;
      b = temp;
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
