/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------

// TODO: Define your own data members and member functions, or classes
class CirGate
{
public:
   #define NEG 0x1

   friend class CirMgr;
   
   CirGate() {}
   CirGate(const unsigned int &id, const unsigned int &no) : _lineno(no), _gateId(id) { }
   virtual ~CirGate() {}

   // Basic access methods
   virtual string getTypeStr() const = 0;
   unsigned int getLineNo() const { return _lineno; }

   // Printing functions
   virtual void printGate() const = 0;
   virtual void reportGate() const;
   virtual void reportFanin(int level);
   virtual void reportFanout(int level);

private:

protected:

   virtual void reportFanin(int level, int indent, bool invert);
   virtual void reportFanout(int level, int indent, bool invert);

   static void raiseGlobalMarker() { ++_globalMarker; }
   static bool isInv(size_t c) { return c & size_t(NEG); }
   static bool isInv(CirGate* c) { return (size_t)c & size_t(NEG); }
   static unsigned int getNum(CirGate* c) { return (unsigned int)(size_t)c; }
   static CirGate* setNum(size_t n) { return (CirGate*)n; }
   static CirGate* setNum(unsigned int n) { return (CirGate*)(size_t)n; }
   static CirGate* setInv(size_t c) { return (CirGate*)(c | size_t(NEG)); }
   static CirGate* setInv(CirGate* c) { return (CirGate*)((size_t)c | size_t(NEG)); }
   static CirGate* gate(size_t c) { return (CirGate*)(c & ~size_t(NEG)); }
   static CirGate* gate(CirGate* c) { return (CirGate*)((size_t)c &~size_t(NEG)); }

   virtual bool isFloating() { return false; }

   void mark() { _marker = _globalMarker; }
   bool isMarked() { return _marker == _globalMarker; }
   bool hasSymbol() { return _symbol != ""; }
   void addFanin(CirGate* c, bool isInv);
   void addFanout(CirGate* c, bool isInv);
   
   static unsigned int _globalMarker;     // Design for Graph serach Algorithm

   unsigned int _lineno;                  // Where the gate was defined
   unsigned int _marker;                  // Design for Graph search Algorithm
   unsigned int _gateId;                  // Gate ID
   string _symbol;                        // AIGER Symbol
   vector<CirGate*> _fanin;
   vector<CirGate*> _fanout;
};

class CirConstGate : public CirGate
{
public:
   CirConstGate() : CirGate(0, 0) {}
   ~CirConstGate() {}

   // Basic access methods
   string getTypeStr() const { return "CONST"; }

   // Print function
   void printGate() const;                   // Self Printing Format
   // void reportFanin(int level) const {};  // None of Fanin
};

class CirUndefGate : public CirGate 
{
public:
   CirUndefGate(const unsigned int &id) : CirGate(id, 0) {}
   ~CirUndefGate() {}

   // Basic access methods
   string getTypeStr() const { return "UNDEF"; }

   // Special Designed For CirUndefGate
   bool isFloating() { return true; }

   // Print function
   void printGate() const;
   // void reportFanin(int level) const {};
   // void reportFanout(int level) const {};
};

class CirPIGate : public CirGate 
{
public:
   CirPIGate(const unsigned int &id, const unsigned int &no) : CirGate(id, no) {}
   ~CirPIGate() {}

   // Basic access methods
   string getTypeStr() const { return "PI"; }

   // Print function
   void printGate() const;                // Self Printing Format
   // void reportFanin(int level) const {};  // None of Fanin

};

class CirPOGate : public CirGate 
{
public:
   CirPOGate(const unsigned int &id, const unsigned int &no) : CirGate(id, no) { }
   ~CirPOGate() {}

   // Basic access methods
   string getTypeStr() const { return "PO"; }

   // Print function
   void printGate() const;                // Self Printing Format
   // void reportFanout(int level) const {}; // None of Fanout

};

class CirAIGate : public CirGate 
{
public:
   CirAIGate(const unsigned int &id, const unsigned int &no) : CirGate(id, no) {}
   ~CirAIGate() {}

   // Basic access methods
   string getTypeStr() const { return "AIG"; }

   // Print function
   void printGate() const;
};

#endif // CIR_GATE_H
