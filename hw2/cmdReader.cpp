/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <cstring>
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream&);
ParseChar getChar(istream&);


//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void
CmdParser::readCmd()
{
   if (_dofile.is_open()) {
      readCmdInt(_dofile);
      _dofile.close();
   }
   else
      readCmdInt(cin);
}

void
CmdParser::readCmdInt(istream& istr)
{
   resetBufAndPrintPrompt();

   while (1) {
      ParseChar pch = getChar(istr);
      if (pch == INPUT_END_KEY) break;
      switch (pch) {
         case LINE_BEGIN_KEY : moveBufPtr(_readBuf); break;
         case HOME_KEY       : moveBufPtr(_readBuf); break;
         case LINE_END_KEY   : moveBufPtr(_readBufEnd); break;
         case END_KEY        : moveBufPtr(_readBufEnd); break;
         case BACK_SPACE_KEY : /* TODO */ if(moveBufPtr(_readBufPtr-1)) { deleteChar(); } break;
         case DELETE_KEY     : deleteChar(); break;
         case NEWLINE_KEY    : addHistory();
                               cout << char(NEWLINE_KEY);
                               resetBufAndPrintPrompt(); break;
         case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
         case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
         case ARROW_RIGHT_KEY: /* TODO */ moveBufPtr(_readBufPtr+1); break;
         case ARROW_LEFT_KEY : /* TODO */ moveBufPtr(_readBufPtr-1); break;
         case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
         case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
         case TAB_KEY        : /* TODO */ insertChar(' ',(TAB_POSITION - (_readBufPtr-_readBuf)%TAB_POSITION)); break;
         case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
         case UNDEFINED_KEY:   mybeep(); break;
         default:  // printable character
            insertChar(char(pch)); break;
      }
      #ifdef TA_KB_SETTING
      taTestOnly();
      #endif
   }
}


// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool
CmdParser::moveBufPtr(char* const ptr)
{
   // TODO...
   if(ptr > _readBufEnd || ptr < _readBuf){
      mybeep();
      return false;
   }

   while(ptr > _readBufPtr){
      cout << *_readBufPtr; // make cursor go forward
      _readBufPtr++;
   }
   /*
      If there is no character to be output after \b, 
      the \b operator will move the output cursor one position to the left, 
      but will not delete the character at the original position 
      or output any other characters to the right.
   */
   while(ptr < _readBufPtr){
      cout << '\b';   // make cursor go back
      _readBufPtr--;
   }

   return true;
}


// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing "visible" character. (use space instead)
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool
CmdParser::deleteChar()
{
   // TODO...
   if(_readBufPtr >= _readBufEnd){
      mybeep();
      return false;
   }

   char *temp = _readBufPtr;
   while(temp < _readBufEnd){
      *temp = *(temp + 1);
      cout << *temp;
      temp++;
   }

   cout << ' ';

   //move back the cursor
   for (char *ptr = _readBufPtr; ptr < _readBufEnd; ptr++){
      cout << '\b';
   }

   _readBufEnd--;

   return true;
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
//
void
CmdParser::insertChar(char ch, int repeat)
{
   // TODO...
   assert(repeat >= 1);

   _readBufEnd += repeat;
   for (char *temp = _readBufEnd-1; temp >= _readBufPtr + repeat;temp--){
      *temp = *(temp - repeat);
   }

   char *temp = _readBufPtr;
   for (int i = 0; i < repeat; i++){
      *temp = ch;
      cout << ch;
      temp++;
   }

   _readBufPtr += repeat;

   // show the remain string
   // cout \b to make your cursor go back to the place that _readBufPtr points to
   for (char* temp = _readBufPtr; temp < _readBufEnd; temp++)
      cout << *temp;
   for (char* temp = _readBufPtr; temp < _readBufEnd; temp++)
      cout << '\b';
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine()
{
    // TODO...
    // move cursor to the left
   moveBufPtr(_readBuf);

    // then cout space to cover original texts
    // when cout one space, _readBufPtr should go forward 1 space to act like cursor
   for (;_readBufPtr < _readBufEnd;_readBufPtr++){
      cout << ' ';
      *_readBufPtr = 0;
   }

   // finally, move the cursor back to the leading of _readBuf
   moveBufPtr(_readBuf);
   _readBufEnd = _readBuf;
}

// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void
CmdParser::moveToHistory(int index)
{
   // TODO...
   assert(index != _historyIdx);

   if(index < _historyIdx){
      if(_historyIdx == 0){
         mybeep();
         return;
      }
      if(index <= 0){
         index = 0;
      }
   }
   else if(index > _historyIdx){
      if(_historyIdx == _history.size()){
         mybeep();
         return;
      }
      if(index >= _history.size()-1){
         index = _history.size() - 1;
      }
   }

   //save temporary string into history
   if(!_tempCmdStored && _historyIdx == _history.size()){
      _history.push_back(string(_readBuf)); 
      _tempCmdStored = true;
   }

   //show history on the screen
   _historyIdx = index;
   retrieveHistory();

   if(_tempCmdStored && index == _history.size()-1){
      _history.pop_back();
      _tempCmdStored = false;
   }
}

// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether 
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
//
void
CmdParser::addHistory()
{
   // TODO...
   // find left right cursor should be
   string str = _readBuf;
   size_t left = -1, right = -1;

   for (size_t i = 0; i < str.length();i++){
      if(str[i]!=' '){
         left = i;
         break;
      }
   }
   for (size_t i = str.length() - 1; i >= 0;i--){
      if(str[i]!=' '){
         right = i;
         break;
      }
   }

   // clean temp recorded string
   if (_tempCmdStored){
      _history.pop_back();
      _tempCmdStored = false;
   }

   //if str isn't a null string
   if(left!=-1){
      if(right!=-1)
         str = str.substr(left, right-left+1);
      else
         str = str.substr(left);
      _history.push_back(str);
   }
   _historyIdx = _history.size();

   for (char *p = _readBuf; p < _readBufEnd;p++){
      *p = 0;
   }
}

// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory()
{
   deleteLine();
   strcpy(_readBuf, _history[_historyIdx].c_str());
   cout << _readBuf;
   _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
}
