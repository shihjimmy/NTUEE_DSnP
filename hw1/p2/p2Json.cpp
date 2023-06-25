/****************************************************************************
  FileName     [ p2Json.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <fstream>
#include <string>
#include <climits>
#include <iomanip>
#include <algorithm>
#include "p2Json.h"

using namespace std;

// TODO
// Implement member functions of class Row and Table here
bool Json::read(const string& jsonFile)
{  
   string key;
   int val;
   string line;
   ifstream myfile;
   myfile.open(jsonFile);
   
   while (getline(myfile, line)){
      //remove space from original line
      line.erase(remove_if(line.begin(),line.end(),::isspace),line.end());

      if(line=="{")
         continue;
      else if(line=="}")
         break;
      else if(line=="")
         continue;
      else{
         //remove ","
         if(line[-1] == ','){
            line = line.substr(0, line.length() - 1);
         }

         int pos = line.find(':');
         key = line.substr(1, pos - 2);
         val = stoi(line.substr(pos + 1, line.length() - 1));

         addJson(JsonElem(key, val));
      }
   }

   return true; 
}

ostream& operator << (ostream& os, const JsonElem& j)
{
   return (os << "\"" << j._key << "\" : " << j._value);
}

void Json::addJson(JsonElem j)
{
   _obj.push_back(j);
}

void Json::maxJson() const
{
   if(isempty()) return;

   int max = INT32_MIN;
   string key;
   for (auto j : _obj){
      if(j.getVal() > max){
         max = j.getVal();
         key = j.getKey();
      }
   }
   cout << "The maximum element is: { ";
   cout << '\"' << key << '\"' << " : " << max << " }." << endl;
}

void Json::minJson() const
{  
   if(isempty())  return;

   int min = INT32_MAX;
   string key;
   for (auto j : _obj){
      if(j.getVal() < min){
         min = j.getVal();
         key = j.getKey();
      }
   }
   cout << "The minimum element is: { ";
   cout << '\"' << key << '\"' << " : " << min << " }." << endl;
}

void Json::sumJson() const
{
   if(isempty())  return;

   int sum = 0;
   for(auto j:_obj){
      sum += j.getVal();
   }
   cout << "The summation of the values is: " << sum << "." << endl;
}

void Json::printJson() const
{
   size_t count = 0;
   cout << "{" << endl;
   for(auto j:_obj){
      cout << "  ";
      cout << '\"';
      cout << j.getKey();
      cout << '\"';
      cout << " : ";
      cout << j.getVal();
      if (count == _obj.size()-1){
         cout << "," << endl;
      }
      else
         cout << endl;
      count++;
   }
   cout << "}" << endl;
}

void Json::aveJson() const
{
   if(isempty())  return;

   double ave;
   int sum = 0;
   for(auto j:_obj){
      sum += j.getVal();
   }
   ave = sum / _obj.size();
   cout<< "The average of the values is: " << fixed << setprecision(1) << ave << "." << endl;
}

bool Json::isempty() const
{  
   if(_obj.empty())
      cout << "Error: No element found!!" << endl;
   return _obj.empty();
}

int JsonElem::getVal() const
{
   return _value;
}

string JsonElem::getKey() const
{
   return _key;
}