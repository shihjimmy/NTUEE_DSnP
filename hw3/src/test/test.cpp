/****************************************************************************
  FileName     [ test.cpp ]
  PackageName  [ test ]
  Synopsis     [ Test program for simple database db ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "dbJson.h"

using namespace std;

extern DBJson dbjson;

class CmdParser;
CmdParser* cmdMgr = 0; // for linking purpose

int
main(int argc, char** argv)
{
   if (argc != 2) {  // testdb <jsonfile>
      cerr << "Error: using testdb <jsonfile>!!" << endl;
      exit(-1);
   }

   ifstream inf(argv[1]);

   if (!inf) {
      cerr << "Error: cannot open file \"" << argv[1] << "\"!!\n";
      exit(-1);
   }

   if (dbjson) {
      cout << "Table is resetting..." << endl;
      dbjson.reset();
   }
   if (!(inf >> dbjson)) {
      cerr << "Error in reading JSON file!!" << endl;
      exit(-1);
   }

   cout << "========================" << endl;
   cout << " Print JSON object" << endl;
   cout << "========================" << endl;
   cout << dbjson << endl;
   cout << endl;

   // TODO
   // Insert what you want to test here by calling DBJson's member functions
   cout << "========================" << endl;
   cout << " SUM JSON object" << endl;
   cout << "========================" << endl;
   cout << dbjson.sum() << endl;
   cout << endl;

   cout << "========================" << endl;
   cout << " AVE JSON object" << endl;
   cout << "========================" << endl;
   cout << dbjson.ave() << endl;
   cout << endl;

   cout << "========================" << endl;
   cout << " MAX JSON object" << endl;
   cout << "========================" << endl;
   size_t idx = -1;
   cout << dbjson.max(idx) << endl;
   cout << "index: " << idx << endl;
   cout << endl;

   cout << "========================" << endl;
   cout << " MIN JSON object" << endl;
   cout << "========================" << endl;
   cout << dbjson.min(idx) << endl;
   cout << "index: " << idx << endl;
   cout << endl;

   cout << "========================" << endl;
   cout << " Print SORTED JSON object by keys" << endl;
   cout << "========================" << endl;
   dbjson.sort(DBSortKey());
   cout << dbjson << endl;

   cout << "========================" << endl;
   cout << " Print SORTED JSON object by values" << endl;
   cout << "========================" << endl;
   dbjson.sort(DBSortValue());
   cout << dbjson << endl;

   cout << "========================" << endl;
   cout << " Print ADDED JSON object" << endl;
   cout << "========================" << endl;
   dbjson.add(DBJsonElem("new_added", 1234567));
   cout << dbjson << endl;


   return 0;
}
