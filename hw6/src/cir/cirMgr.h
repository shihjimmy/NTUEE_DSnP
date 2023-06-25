/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <algorithm>
#include <list>
#include <vector>
#include <stack>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr();
   ~CirMgr();

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const;

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;

   void reset();

private:

   void DepthFirstTraversal(const unsigned int, vector<CirGate*> &) const;
   void DepthFirstTraversal(CirGate* , vector<CirGate*> &) const;

   bool readHeader(const string&);
   bool loadInput(const unsigned int&);
   bool loadOutput(const unsigned int&, const unsigned int&);
   bool loadAIG(const unsigned int&);
   bool loadLatch(const unsigned int&, const unsigned int&);
   bool loadSymbol(const unsigned int&, const string&);
   bool loadComment(const string&);
   // bool connect(const unsigned int &, const unsigned int &);

   vector<unsigned int> _pin;       // PinIn Number
   vector<unsigned int> _pout;      // PinOut Number
   vector<unsigned int> _aig;       // AIGs Number
   vector<unsigned int> _latch;     // Latch Number
   vector<unsigned int> _floating;  // Floating Gates
   vector<unsigned int> _notused;   // Not in used Gates
   vector<CirGate*>     _gates;

   stringstream _comment;           // Store the comment

   unsigned int _M;                 // Maximal Variable Index
   unsigned int _I;                 // Number of Inputs
   unsigned int _L;                 // Number of Latches
   unsigned int _O;                 // Number of Outputs
   unsigned int _A;                 // Number of AND Gates
};

#endif // CIR_MGR_H
