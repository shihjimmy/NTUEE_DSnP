/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include "dbJson.h"
#include "../util/util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream&
operator << (ostream& os, const DBJsonElem& j)
{
   os << "\"" << j._key << "\"" <<" : " << j._value;
   return os;
}

istream& operator >> (istream& is, DBJson& j)
{
   // TODO: to read in data from Json file and store them in a DB 
   // - You can assume the input file is with correct JSON file format
   // - NO NEED to handle error file format
   assert(j._obj.empty());
   string key;
   int value;
   string line;
   while(getline(is,line,'\n')){
      line.erase(remove_if(line.begin(),line.end(),::isspace),line.end());

      if(line == "{")
         continue;
      else if(line == "}")
         break;
      else if(line == "")
         continue;
      else{
         if (line[-1] == ',')
            line = line.substr(0, line.length() - 1);

         int pos = line.find(':');
         key = line.substr(1, pos - 2);
         value = stoi(line.substr(pos+1));

         j.add(DBJsonElem(key, value));
      }
   }

   j._readin = true;

   return is;
}

ostream& operator << (ostream& os, const DBJson& j)
{
   // TODO
   cout << "{" << endl;
   for (int i = 0; i < j._obj.size();i++){
      if(i!=j._obj.size()-1)
         cout << j._obj[i] << ',' << endl;
      else
         cout << j._obj[i] << endl;
   }
   cout << "}" << endl;

   return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/

// return true if JSON file hasn't been read in
bool 
DBJson::operator!() 
{ 
   return !_readin;  
}

// return this if JSON file has been read in; return NLL if not.
DBJson::operator void *() const 
{ 
   return (!_readin) ? NULL : (void*)this; 
}

void
DBJson::reset()
{
   // TODO
   _obj.clear();
   _readin = false;
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{
   // TODO
   string str = elm.key();
   for(auto json:_obj){
      if(json.key() == str)
         return false;
   }

   _obj.push_back(elm);

   return true;
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{
   // TODO
   if(_obj.empty())
      return 0.0;

   float sum = 0;
   for (auto json : _obj){
      sum += json.value();
   }

   return sum / _obj.size();
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
   // TODO
   if(_obj.empty()){
      idx = size();
      return INT_MIN;
   }
   
   int maxN = INT_MIN;
   for (int i = 0;i<_obj.size();i++){
      if(_obj[i].value() > maxN){
         maxN = _obj[i].value();
         idx = i;
      }
   }

   return  maxN;
}

// If DBJson is empty, set idx to size() and return INT_MAX
int
DBJson::min(size_t& idx) const
{
   // TODO
   if(_obj.empty()){
      idx = size();
      return INT_MAX;
   }
   
   int minN = INT_MAX;
   for (int i = 0;i<_obj.size();i++){
      if(_obj[i].value() < minN){
         minN = _obj[i].value();
         idx = i;
      }
   }

   return  minN;
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
   // TODO
   if(_obj.empty())
      return 0;

   int s = 0;
   for(auto json:_obj)
      s += json.value();

   return s;
}
