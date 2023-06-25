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
      iterator(){}
      iterator(vector<Data>* _bucket, size_t num = 0, size_t bucket = 0,size_t t = 0): 
         _CurrentBucket(bucket), _CurrentNum(num), _buckets(_bucket), _numBuckets(t){}
      ~iterator(){}

      const Data& operator * () const { return (*(_buckets+_CurrentBucket))[_CurrentNum]; }
      
      iterator& operator ++ () {
         int max_size = (*(_buckets + _CurrentBucket)).size()-1;
         bool change = false;

         if(_CurrentNum < max_size){
            _CurrentNum++;
         }
         else{
            for (int i = _CurrentBucket+1;i < _numBuckets; ++i){
               if((*(_buckets+i)).size()){
                  _CurrentNum = 0;
                  _CurrentBucket = i;
                  change = true;
                  break;
               }
            }

            if(!change)
               _CurrentNum++;
         }

         return (*this);
      }

      bool operator != (const iterator& i) const { return ((*this)==i) ? false : true; }
      bool operator ==(const iterator &i) const { 
         if(_CurrentBucket!=i._CurrentBucket || _CurrentNum!=i._CurrentNum || _buckets!=i._buckets || _numBuckets!=i._numBuckets)
            return false;
         return true;
      }

      void operator=(const iterator &i) { 
         _CurrentBucket = i._CurrentBucket;
         _CurrentNum = i._CurrentNum;
         _buckets = i._buckets;
         _numBuckets = i._numBuckets;
      }

   private:
      size_t _CurrentNum;
      size_t _CurrentBucket;
      size_t _numBuckets;
      vector<Data> *_buckets;
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
      for (int i = 0; i < _numBuckets;++i){
         if(_buckets[i].size())
            return iterator(_buckets,0,i,_numBuckets);
      }
      return iterator(NULL); 
   }

   // Pass the end
   iterator end() const {
      for (int i = _numBuckets - 1; i >= 0;--i){
         if(_buckets[i].size())
            return iterator(_buckets,_buckets[i].size(),i,_numBuckets);
      }
      return iterator(NULL); 
   }

   // return true if no valid data
   bool empty() const { return (!end()._buckets) ? true : false; }

   // number of valid data
   size_t size() const { 
      size_t s = 0;
      for (int i = 0; i < _buckets->size();++i)
         s += _buckets[i].size();
      return s; 
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {
      size_t num = bucketNum(d);

      if(_buckets[num].size()){
         for(auto data : _buckets[num]){
            if(d == data)
            // in fact, d == data mean they share same name
               return true;
         }
      }
      return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const { 
      size_t num = bucketNum(d);

      if(_buckets[num].size()){
         for(auto data : _buckets[num]){
            if(d == data){
               // Since = and == operator have different meanings in Data (tasknode)
               // in fact, d == data mean they share same name
               // and d = data mean assign data's load to d's load
               d = data;                
               return true;
            }
         }
      }
      return false; 
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) { 
      // update --> find same-name node in the hash
      // and update that same-name node with d's load
      size_t num = bucketNum(d);

      if(_buckets[num].size()){
         for(auto& data : _buckets[num]){
            if(d == data){  // same name
               data = d;    // update load
               return true;
            }
         }
      }

      _buckets[num].push_back(d);
      return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) { 
      if(!check(d)){
         int num = bucketNum(d);
         _buckets[num].push_back(d);
         return true;
      }
      return false; 
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) { 
      if(check(d)){
         int num = bucketNum(d);
         typename vector<Data>::iterator it;
         for (it = _buckets[num].begin(); it != _buckets[num].end();++it)
            if((*it) == d)
               break;

         _buckets[num].erase(it);
         return true;
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
