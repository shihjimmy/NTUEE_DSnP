/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "../util/util.h"

using namespace std;

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

// Initialize static data member
unsigned int CirGate::_globalMarker = 0;

// Indent
#define INDENT 2

/**************************************/
/*   class CirGate member functions   */
/**************************************/

/*
   Append CirGate* to _fanin

   @param c
      Fanin in type CirGate*
   @param isInv
      If True, store 0x1 at the LSB of CirGate*
*/
void 
CirGate::addFanin(CirGate* c, bool isInv)
{
   if (isInv) c = CirGate::setInv(c);
   _fanin.push_back(c);
}

/*
   Append CirGate* to _fanout

   @param c
      Fanout in type CirGate*
   @param isInv
      If True, store 0x1 at the LSB of CirGate*
*/
void
CirGate::addFanout(CirGate* c, bool isInv)
{
   if (isInv) c = CirGate::setInv(c);
   _fanout.push_back(c);
}

/*
 * Example:
 * 
 * ==================================================
 * = AIG(100), line 312                             =
 * ==================================================
 * 
*/

void
CirGate::reportGate() const
{
   stringstream ss;
   cout << "==================================================" << endl; // length = 49

   // Store informations in stringstream   
   ss << "= " << getTypeStr() << '(' << _gateId << ")";
   if (_symbol != "") { ss << '"' << _symbol << '"';}
   ss << ", line " << _lineno;
   
   // Get Lenght of StringStream, fill with space
   ss.seekp(0, ios::end);  
   if (49 - ss.tellp() > 0) { ss << string(49 - ss.tellp(), ' '); ss << '='; }
   ss.seekp(0, ios::beg);

   // Release string to cout.
   cout << ss.str() << endl;

   cout << "==================================================" << endl;
}

// Public Function (API)
void
CirGate::reportFanin(int level)
{
   assert (level >= 0);
   raiseGlobalMarker();
   reportFanin(level, 0, false);
}

// Public Function (API)
void
CirGate::reportFanout(int level)
{
   assert (level >= 0);
   raiseGlobalMarker();
   reportFanout(level, 0, false);
}

// Private Function
void
CirGate::reportFanin(int level, int indent, bool invert)
{
   // Cout state of (this) CirGate, with specified indent
   cout << string(indent, ' ');
   if (invert) cout << '!';
   cout << getTypeStr() << ' ' << _gateId;
   
   // Cout (*) if the fanin was hidden
   if (isMarked() && _fanin.size() && level) cout << " (*)";
   
   // End Message;
   cout << endl;
   
   // Recursive Call _fanin to Report
   if (level > 0 && !(this)->isMarked()) 
   {
      // Mark (this) CirGate
      (this)->mark();
   
      // Call Fanins
      indent += INDENT;
      for (vector<CirGate*>::iterator it = _fanin.begin(); it != _fanin.end(); ++it)  
         CirGate::gate(*it)->reportFanin(level - 1, indent, (CirGate::isInv(*it))? true : false);
   }
}

// Private Function
void
CirGate::reportFanout(int level, int indent, bool invert)
{
// Cout state of (this) CirGate, with specified indent
   cout << string(indent, ' ');
   if (invert) cout << '!';
   cout << getTypeStr() << ' ' << _gateId;
   
   // Cout (*) if the fanout was hidden
   if (isMarked() && _fanout.size() && level) cout << " (*)";
   
   // End Message;
   cout << endl;
   
   // Recursive Call _fanin to Report
   if (level > 0 && !(this)->isMarked()) 
   {
      // Mark (this) CirGate
      (this)->mark();
   
      // Call Fanouts
      indent += INDENT;
      for (vector<CirGate*>::iterator it = _fanout.begin(); it != _fanout.end(); ++it)
         CirGate::gate(*it)->reportFanout(level - 1, indent, (CirGate::isInv(*it))? true : false);
   }
}

/***************************************/
/* class CirConstGate member functions */
/***************************************/

void 
CirConstGate::printGate() const
{
   
}

/***************************************/
/* class CirUndefGate member functions */
/***************************************/

void 
CirUndefGate::printGate() const
{
   
}

/***************************************/
/* class CirPIGate member functions    */
/***************************************/

void
CirPIGate::printGate() const
{

}


/***************************************/
/* class CirPOGate member functions    */
/***************************************/

void
CirPOGate::printGate() const
{

}


/***************************************/
/* class CirAIGate member functions    */
/***************************************/

void 
CirAIGate::printGate() const
{
   
}