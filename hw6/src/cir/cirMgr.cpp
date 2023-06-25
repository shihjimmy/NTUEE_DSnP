/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "../util/util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printing, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

void
popError(CirParseError err)
{
   parseError(err); cirMgr->reset(); cirMgr = NULL;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/

CirMgr::CirMgr() : _M(0), _I(0), _L(0), _O(0), _A(0)
{

}

// Release the memory allocated by CirGates.
CirMgr::~CirMgr()
{
   for (vector<CirGate*>::iterator it = _gates.begin() ; it != _gates.end(); ++it)  
      delete *it;
}

/*
   Return the pointer of specified Gate.

   @return CirGate*
      0 if not found
*/
CirGate*
CirMgr::getGate(unsigned int gid) const
{
   return (gid > _M + _O)? NULL : _gates[gid];
}

bool
CirMgr::readCircuit(const string& fileName)
{
   // Parsing Circuit Variables
   unsigned int i1, i2, i3;
   fstream file;
   streampos pos_o;
   string tmp, tmp2;
   int state;
   char type;

   // Open File   
   file.open(fileName);

   // Check if open error
   if (!file.good())
   {
      cerr << "Cannot open design \"" << fileName << "\"!!" << endl;
      cirMgr = NULL;
      return false;
   }

   // Set LineNo
   lineNo = 0;

   // Parsing Header
   getline(file, tmp);

   if (!readHeader(tmp))      
      { return false; }
   
   // Next Line
   ++lineNo;
   colNo = 0;

   // Set Attribute (1 (CONST 0) + M (PI / AIG) + O (PO)) 
   _gates.resize(_M + _O + 1); 
   fill(_gates.begin(), _gates.end(), (CirGate*)NULL);

   // Set Constant False
   _gates[0] = new CirConstGate();

   // Number ID of PIns in range [1, _M], Record LineNo also.
   for (int i = 0; i < _I; ++i)
   {
      getline(file, tmp);
      state = sscanf(tmp.c_str(), "%d", &i1);

      // Parsing
      if (state < 1)
         { errMsg = "PI literal ID"; parseError(MISSING_NUM); reset(); cirMgr = NULL; return false; }
      else if (state == 1 && loadInput(i1)) 
         { /* Do nothing */ }
         
      ++lineNo;
   }

   // Latch
   for (int l = 0; l < _L; ++l)
   {
      getline(file, tmp);
      state = sscanf(tmp.c_str(), "%d %d", &i1, &i2);

      // Parsing
      if (state < 2)
         { errMsg = "Latch literal ID"; parseError(MISSING_NUM); reset(); cirMgr = NULL; return false; }
      else if (state == 2 && loadLatch(i1, i2)) 
         { /* Do nothing */ }
      
      ++lineNo;
   }

   // Record Marker
   pos_o = file.tellg();

   // Number ID of POs in range [_M + 1, _M + _O], and fanin is in range [0, _M]
   for (int o = 0; o < _O; ++o)
   {
      getline(file, tmp);
      state = sscanf(tmp.c_str(), "%d", &i1);

      // Parsing
      if (state == -1)
         { errMsg = "PO literal ID"; parseError(MISSING_NUM); reset(); cirMgr = NULL; return false; }
      else if (state < 1)
         { parseError(MISSING_NUM); reset(); cirMgr = NULL; return false; }
      else if (state == 1 && loadOutput(_M + o + 1, i1)) 
         { /* Do nothing */ }

      ++lineNo;
   }

   // Number ID of AIGs
   for (int a = 0; a < _A; ++a)
   {
      getline(file, tmp);
      state = sscanf(tmp.c_str(), "%d %d %d", &i1, &i2, &i3);

      // Parse Error
      if (state == 0)
         { errMsg = "AIG literal ID"; parseError(MISSING_NUM); reset(); cirMgr = NULL; return false; }
      else if (state < 3)
         { errMsg = "AIG literal ID"; parseError(MISSING_NUM); reset(); cirMgr = NULL; return false; }
      else if (state == 3 && loadAIG(i1)) 
         { /* Do nothing */ }
      
      ++lineNo;
   }

   /*
    * Load Symbol and Comment, if they are at the file
   */
   while (true)
   {
      if (file.peek() == 'c')          // Read Comment
      { 
         getline(file, tmp);
         if (tmp != "c")
            { parseError(ILLEGAL_IDENTIFIER); }

         while(file.good()) 
         { 
            getline(file, tmp); 
            _comment << tmp << endl; 
         }

         break; 
      }
      if (file.peek() == EOF)          // No Symbol and Comment
         { break; }
         
      getline(file, tmp);
      state = sscanf(tmp.c_str(), "%c%u %s", &type, &i1, buf);
      
      if (state < 3)
         { parseError(MISSING_NUM); }
      else if (state == 3)
      {
         tmp2 = string(buf);

         if (type == 'i')
         {
            if (i1 > _A) 
               { parseError(NUM_TOO_BIG); }
            if (i1 < 0)    
               { parseError(NUM_TOO_SMALL); }
         }
         else if (type == 'o')
         {
            // Parsing Error
            if (i1 > _O)
               { parseError(NUM_TOO_BIG); }
            if (i1 < 0)    
               { parseError(NUM_TOO_SMALL); }
            
            i1 += _M;
         }
         else { parseError(ILLEGAL_SYMBOL_TYPE); reset(); cirMgr = NULL; return false; }
         
         // Load Symbol
         loadSymbol(i1 + 1, tmp2);
      }
      else { break; }
   }

   /*
    * Connecting AIG. Don't need to do error prevention after here.
    *  
    * Before connecting the circuit... the connecting information 
    * is keeped by only CirAIGate._fanout.
    * 
    * After connecting the circuit... each CirGate can see the 
    * ascenders (CirGate._fanin) and descenders (CirGate._fanout).
    * 
    * - CirPOGate
    * - CirAIGate
    * 
    * Find out the Undefined Gate
    * 
    * Find out the Floating Gate
    */

   // Reset LineNo to _I
   lineNo = _I + 1;

   // Load the fanin information.
   file.clear();  file.seekg(pos_o, ios::beg); 

   // Connect Output
   for (int o = 0; o < _O; ++o)
   {
      file >> i1;
      _gates[_M + o + 1]->addFanin(getGate(i1 / 2), i1 % 2);
      _gates[i1 / 2]->addFanout(getGate(_M + o + 1), i1 % 2);
   }

   // Connect And-Inverter Gate
   for (int a = 0; a < _A; ++a)
   {
      file >> i1 >> i2 >> i3;

      // if i2 is Undefined
      // that is, doesn't appear in the PI or PO gates
      if (!_gates[i2 / 2])
      {
         // _floating.push_back(i1 / 2);
         _gates[i2 / 2] = new CirUndefGate(i2 / 2);
      }   
      
      // if i3 is Undefined
      if (!_gates[i3 / 2])
      {
         // _floating.push_back(i1 / 2);
         _gates[i3 / 2] = new CirUndefGate(i3 / 2);
      }
         
      // i2 and i3 are the fanins of i1
      _gates[i1 / 2]->addFanin(getGate(i2 / 2), i2 % 2);  
      _gates[i1 / 2]->addFanin(getGate(i3 / 2), i3 % 2);
      
      // i1 is the fanin of i2 and i3
      _gates[i2 / 2]->addFanout(getGate(i1 / 2), i2 % 2); 
      _gates[i3 / 2]->addFanout(getGate(i1 / 2), i3 % 2);
   }

   // Find out the defined but not in used gates
   for (size_t i = 0; i < _aig.size(); ++i)
   {
      if (!getGate(_aig[i])->_fanout.size())
         _notused.push_back(_aig[i]);
   }

   for (size_t i = 0; i < _pin.size(); ++i)
   {
      if (!getGate(_pin[i])->_fanout.size())
         _notused.push_back(_pin[i]);
   }

   // Find out the undefined gates (floating)
   for (size_t i = 0; i < _aig.size(); ++i)
   {
      for (vector<CirGate*>::iterator it2 = getGate(_aig[i])->_fanin.begin(); it2 != getGate(_aig[i])->_fanin.end(); ++it2)
      {
         if (CirGate::gate(*it2)->isFloating())
            _floating.push_back(_aig[i]);
      }
   }

   for (size_t i = 0; i < _pout.size(); ++i)
   {
      for (vector<CirGate*>::iterator it2 = getGate(_pout[i])->_fanin.begin(); it2 != getGate(_pout[i])->_fanin.end(); ++it2)
      {
         if (CirGate::gate(*it2)->isFloating())
            _floating.push_back(_pout[i]);
      }
   }

   // Close fstream
   file.close();

   /*
    * Sorting _fanin and _fanout
    * - AIG (multiple _fanout)
    * - PIN (multiple _fanout)
   */
   for (unsigned int i = 0; i < _M; ++i )
   {
      if (getGate(i))
      {
         sort(getGate(i)->_fanout.begin(), getGate(i)->_fanout.end(),
            [](CirGate* a, CirGate* b) { return CirGate::gate(a)->_gateId < CirGate::gate(b)->_gateId; });
      }
   }

   // Reset LineNo to 0
   lineNo = 0;

   // Sorting the vector
   sort(_floating.begin(), _floating.end());
   sort(_notused.begin(), _notused.end());

   return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/

/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/

void
CirMgr::printSummary() const
{
   cout << endl
        << "Circuit Statistics" << endl
        << "==================" << endl
        << "  PI   " << right << setw(9) << _pin.size()  << endl
        << "  PO   " << right << setw(9) << _pout.size() << endl
        << "  AIG  " << right << setw(9) << _aig.size()  << endl
        << "------------------" << endl
        << "  Total" << right << setw(9) << _pin.size() + _pout.size() + _aig.size() << endl;
}

/*
   Forward (from PIn to POut) printing
*/
void
CirMgr::printNetlist() const
{
   vector<CirGate*> dfslist;
   CirGate* tmp;

   // Raise Up Global Marker
   CirGate::raiseGlobalMarker();

   // Reset lineNo
   lineNo = 0;

   // Pre-spacing
   cout << endl;

   // Depth First Traversal from all POut
   for (size_t i = 0; i < _pout.size(); ++i)
      DepthFirstTraversal(_pout[i], dfslist);
   
   // Print by the priority of dfslist
   for (vector<CirGate*>::iterator it = dfslist.begin(); it != dfslist.end(); ++it)
   {
      cout << '[' << lineNo << "] " << setw(4) << left << (*it)->getTypeStr() << (*it)->_gateId;

      for (vector<CirGate*>::iterator it2 = (*it)->_fanin.begin(); it2 != (*it)->_fanin.end(); ++it2)
      {
         tmp = CirGate::gate(*it2);
         
         cout << ' ';

         // Print "*" if needed.
         if (tmp->isFloating())     cout << '*';

         // Print "!" if needed.
         if (CirGate::isInv(*it2))  cout << '!';

         cout << tmp->_gateId;
      }

      // Print Symbol if needed.
      if ((*it)->hasSymbol()) cout << " (" << (*it)->_symbol << ')';

      cout << endl;
      ++lineNo;
   }

   // Reset lineNo
   lineNo = 0;
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";

   for (size_t i = 0; i < _pin.size(); ++i)
      cout << ' ' << (_pin[i]);
   
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";

   for (size_t i = 0; i < _pout.size(); ++i)
      cout << ' ' << (_pout[i]);
   
   cout << endl;
}

/*
   Print the Floating Gates Message

   Attribute
   - Floating fanins
   - Defined but not used 
*/
void
CirMgr::printFloatGates() const
{
   if (!_floating.empty())
   {
      cout << "Gates with floating fanin(s):";

      for (size_t i = 0; i < _floating.size(); ++i)
         cout << ' ' << (_floating[i]);
      
      cout << endl;
   }

   if (!_notused.empty())
   {
      cout << "Gates defined but not used  :";
      
      for (size_t i = 0; i < _notused.size(); ++i)
         cout << ' ' << (_notused[i]);
      
      cout << endl;
   }
}

void
CirMgr::writeAag(ostream& outfile) const
{
   vector<CirGate*> dfslist;
   size_t activeAIG = 0;
   CirGate* tmp;
   
   // Output + AIG + Input (With DFS order)
   CirGate::raiseGlobalMarker();
   for (size_t i = 0; i < _pout.size(); ++i)
      DepthFirstTraversal(getGate(_pout[i]), dfslist);

   // Count number of AIG.
   for (vector<CirGate*>::iterator it = dfslist.begin(); it != dfslist.end(); ++it)
      if ((*it)->_fanin.size() && (*it)->_fanout.size())
         ++activeAIG;

   // Header
   outfile << "aag " << _M << " " << _I << " " << _L << " " <<  _O << " " << activeAIG << endl;
   
   // Input
   for (size_t i = 0; i < _pin.size(); ++i)
      outfile << 2 * _pin[i] << endl;
   
   // Output
   for (size_t i = 0; i < _pout.size(); ++i)
   {
      tmp = getGate(_pout[i])->_fanin[0];
      outfile << ((CirGate::isInv(tmp))? (2 * CirGate::gate(tmp)->_gateId + 1): (2 * tmp->_gateId)) << endl;
   }

   // AIG (Print the dfslist)
   for (vector<CirGate*>::iterator it = dfslist.begin(); it != dfslist.end(); ++it)
   {
      // Check if it is an AIG
      if ((*it)->_fanin.size() && (*it)->_fanout.size())
      {
         outfile << 2 * (*it)->_gateId;
         for (vector<CirGate*>::iterator it2 = (*it)->_fanin.begin(); it2 != (*it)->_fanin.end(); ++it2)
            outfile << ' ' << ((CirGate::isInv(*it2))? (2 * CirGate::gate(*it2)->_gateId + 1) : (2 * (*it2)->_gateId));
         outfile << endl;
      }
   }

   // Symbol
   for (size_t i = 0; i < _pin.size(); ++i)
   {
      tmp = getGate(_pin[i]);
      if (tmp->hasSymbol()) outfile << 'i' << i << ' ' << tmp->_symbol << endl;
   }

   for (size_t o = 0; o < _pout.size(); ++o)
   {
      tmp = getGate(_pout[o]);
      if (tmp->hasSymbol()) outfile << 'o' << o << ' ' << tmp->_symbol << endl;
   }
}

void
CirMgr::reset()
{
   // Reset the attribute
   _M = _I = _L = _O = _A = 0;

   // Release the memory of CirGates
   for (vector<CirGate*>::iterator it = _gates.begin(); it != _gates.end(); ++it)
      delete (*it);

   // Set the pointer as NULL
   cirMgr = NULL;
}

/**********************************************************/
/*   HELPER FUNCTION                                      */
/**********************************************************/

/*
   Overloading.

   @param gateID
      The gateID to start searching.
   @param dfslist
      The gateID List
*/
void
CirMgr::DepthFirstTraversal(const unsigned int gateID, vector<CirGate*> &dfslist) const
{
   DepthFirstTraversal(getGate(gateID), dfslist);
}

/*
   DepthFirstSearch Algorithm, implemented by recursive
   
   Search gates from POut(s) to PIn(s). 
   
   Include floating gates

   @param c
      The start point of CirGate
   @param dfslist
      The gateID List
*/
void
CirMgr::DepthFirstTraversal(CirGate* c, vector<CirGate*> &dfslist) const
{
   // If have already seen the element, ignore it
   if (c->isMarked() || c->isFloating()) return;

   // Mark the Gate
   c->mark();

   // Left -> Rigth -> Center
   for (vector<CirGate*>::iterator it = c->_fanin.begin(); it != c->_fanin.end(); ++it)
      DepthFirstTraversal(CirGate::gate(*it), dfslist);
      
   dfslist.push_back(c);
}

bool
CirMgr::readHeader(const string &s)
{
   int state = sscanf(s.c_str(), "%s %d %d %d %d %d", buf, &_M, &_I, &_L, &_O, &_A);

   if ( s.find('\t') != string::npos )
      { errInt = '\t'; colNo += s.find('\t'); parseError(ILLEGAL_WSPACE); }
   else if (state == -1)
   {
      colNo = s.length();    { parseError(MISSING_NUM); }
   }
   else if (state < 6)
   { 
      if (string(buf) != "aag") 
         { errMsg = string(buf); parseError(ILLEGAL_IDENTIFIER); }
      else
         { colNo = s.length(); errMsg = "number of variables"; parseError(MISSING_NUM); }
   }
   else if (state == 6)
   {
      if (s[0] == ' ')        
         { parseError(EXTRA_SPACE); }
      else if (s[0] != 'a')   
         { errInt = s[0]; parseError(ILLEGAL_WSPACE); }
      else if (string(buf) != "aag") 
         { errMsg = ""; parseError(ILLEGAL_IDENTIFIER); }
      else if (count(s.begin(), s.end(), ' ') > 5)
         { 
            for (int i = 0; i < 6; ++i) colNo = s.find(' ', ++colNo); 
            parseError(MISSING_NEWLINE); 
         }
      else if (_M < _I + _A)       
         { errInt = _M; parseError(NUM_TOO_SMALL); }
      else                    
         { return true; }
   }

   reset(); cirMgr = NULL; return false;
}

/*
   Input pin will have no fanin, but multiple fanout

   @param id
      The ID Number of the Gate
   @return bool
      True if successfully load a PIn
*/
bool
CirMgr::loadInput(const unsigned int &id)
{
   // Parsing Error
   if (id > 2 * _M)         { errInt = id; parseError(MAX_LIT_ID); }
   else if (id == 0)        { errInt = id; parseError(REDEF_CONST); }
   else if (id == 1)        { errInt = id; parseError(REDEF_CONST); }
   else if (_gates[id / 2]) { errInt = id; errGate = _gates[id / 2]; parseError(REDEF_GATE); }
   else
   {
      _pin.push_back(id / 2);
      _gates[id / 2] = new CirPIGate(id / 2, lineNo + 1);

      return true;
   }

   return false;
}

/*
   Output pin will have no fanout, but 1 fanin

   @param id
      The ID Number of the Gate
   @param fanin
      The ID Number of the Fanin
   @return bool
      True if successfully load a POut
*/
bool 
CirMgr::loadOutput(const unsigned int &id, const unsigned int &fanin)
{
   // Parsing Error
   if (false) {}
   else 
   {
      _pout.push_back(id);
      _gates[id] = new CirPOGate(id, lineNo + 1);
      
      return true;
   }

   return false;
}

/*
   And-Inverter Gate will have 2 fanin, variant fanout.

   @param id
      The ID Number of the Gate Output
   @param i1, i2
      The ID Number of the fanin (input)
*/
bool
CirMgr::loadAIG(const unsigned int &id)
{
   // Parsing Error
   if (id > 2 * _M)         { parseError(NUM_TOO_BIG); }
   else if (id == 0)        { parseError(REDEF_CONST); }
   else if (id == 1)        { parseError(REDEF_CONST); }
   else if (id % 2)         { parseError(CANNOT_INVERTED); }
   else if (_gates[id / 2]) { errInt = id; errGate = _gates[id / 2]; parseError(REDEF_GATE); }
   else
   {
      _aig.push_back(id / 2);
      _gates[id / 2] = new CirAIGate(id / 2, lineNo + 1);

      return true;
   }

   return false;
}

/*
   Load the latches
*/
bool
CirMgr::loadLatch(const unsigned int &Q, const unsigned int &nextQ)
{
   return true;
}

/*
   Load the symbol

   @param id
      The ID Number of the gate
   @param s
      The Symbol
*/
bool 
CirMgr::loadSymbol(const unsigned int &id, const string &s)
{
   // Parsing Error
   if (_gates[id]->hasSymbol())   parseError(REDEF_SYMBOLIC_NAME);
   else
   {
      _gates[id]->_symbol = s;
      
      return true;
   }
   
   return false;
}

bool
CirMgr::loadComment(const string &s)
{
   return true;
}
