/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include "p2Json.h"

using namespace std;

int main()
{
   Json json;

   // Read in the csv file. Do NOT change this part of code.
   string jsonFile;
   cout << "Please enter the file name: ";
   cin >> jsonFile;
   if (json.read(jsonFile))
      cout << "File \"" << jsonFile << "\" was read in successfully." << endl;
   else {
      cerr << "Failed to read in file \"" << jsonFile << "\"!" << endl;
      exit(-1); // jsonFile does not exist.
   }

   // TODO read and execute commands
   while (true) {
      cout << "Enter command: ";
      string str;
      cin >> str;
      if(str == "PRINT"){
         json.printJson();
      }
      else if(str == "EXIT"){
         break;
      }
      else if(str == "ADD"){
         string key;
         int val;
         cin >> key;
         cin >> val;
         json.addJson(JsonElem(key, val));
      }
      else if(str == "SUM"){
         json.sumJson();
      }
      else if(str == "AVE"){
         json.aveJson();
      }
      else if(str == "MAX"){
         json.maxJson();
      }
      else if(str == "MIN"){
         json.minJson();
      }
   }
   return 0;
}
