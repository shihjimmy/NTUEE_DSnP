/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "../cmd/cmdParser.h"
#include "../util/util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   // Use try-catch to catch the bad_alloc exception
   vector<string> options;
   bool array_opt = false;
   int array_num = -1;
   int obj_num = -1;

   if(!lexOptions(option,options))
      return CMD_EXEC_ERROR;

   for (size_t i = 0; i < options.size();i++){
      
      if(!myStrNCmp("-Array",options[i],2)){
         if(array_opt)
            return errorOption(CMD_OPT_EXTRA, options[i]);
         array_opt = true;

         i++;
         if(!myStr2Int(options[i],array_num) || array_num <= 0){
            return errorOption(CMD_OPT_ILLEGAL, options[i]);
         }
      }
      else{
         if(array_opt && obj_num == -1){
            // no numObjects
            if(!myStr2Int(options[i],obj_num) || obj_num <= 0){
               return errorOption(CMD_OPT_ILLEGAL, options[i]);
            }
         }
         else if(array_opt && obj_num != -1){
            return errorOption(CMD_OPT_EXTRA, options[i]);
         }
         else if(!array_opt && obj_num != -1){
            return errorOption(CMD_OPT_EXTRA, options[i]);
         }
         else{
            if(!myStr2Int(options[i],obj_num) || obj_num <= 0){
               return errorOption(CMD_OPT_ILLEGAL, options[i]);
            }
         }
      }

   }

   if(!options.size())
      return errorOption(CMD_OPT_MISSING, "");
   
   if(array_opt){
      try{
         mtest.newArrs(obj_num, array_num);
      }
      catch(std::bad_alloc){
         return CMD_EXEC_ERROR;
      }
   }
   else{
      try{
         mtest.newObjs(obj_num);
      }
      catch(std::bad_alloc ){
         return CMD_EXEC_ERROR;
      }
   }

   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
   vector<string> options;
   bool array_opt = false;
   bool random_opt = false;
   bool index_opt = false;
   int index = -1;

   if(!lexOptions(option,options)){
      return CMD_EXEC_ERROR;
   }

   for (size_t i = 0; i < options.size();i++){
      
      if(!myStrNCmp("-Array",options[i],2)){
         //check -Array defined multiple times
         if(array_opt)
            return errorOption(CMD_OPT_EXTRA, options[i]);
         array_opt = true;
      }
      else if(!myStrNCmp("-Random",options[i],2)){
         //check -Random defined multiple times
         if(random_opt)
            return errorOption(CMD_OPT_EXTRA, options[i]);
         random_opt = true;

         if(random_opt && index_opt)
            return errorOption(CMD_OPT_ILLEGAL, options[i]);

         i++;
         if(i == options.size())
            return errorOption(CMD_OPT_MISSING, "-R");
         
         if(!myStr2Int(options[i],index) || index < 0 )
            return errorOption(CMD_OPT_ILLEGAL, options[i]);
      }
      else if(!myStrNCmp("-Index",options[i],2)){
         //check -Index defined multiple times
         if(index_opt)
            return errorOption(CMD_OPT_EXTRA, options[i]);
         index_opt = true;

         if(random_opt && index_opt)
            return errorOption(CMD_OPT_ILLEGAL, options[i]);

         i++;
         if(i == options.size())
            return errorOption(CMD_OPT_MISSING, "-I");
         
         if(!myStr2Int(options[i],index) || index < 0 )
            return errorOption(CMD_OPT_ILLEGAL, options[i]);
      }
      else{
         return errorOption(CMD_OPT_EXTRA, options[i]);
      }

   }

   if(random_opt){
      if(array_opt){
         if(!mtest.getArrListSize()){
            cerr << "Size of array list is 0!! " << endl;
            return CMD_EXEC_ERROR;
         }
      }
      else{
         if(!mtest.getObjListSize()){
            cerr << "Size of object list is 0!! " << endl;
            return CMD_EXEC_ERROR;
         }
      }

      if(array_opt){
         for (int i = 0; i < index;i++){
            mtest.deleteArr(rnGen(mtest.getArrListSize()));
         }
      }
      else{
         for (int i = 0; i < index;i++){
            mtest.deleteObj(rnGen(mtest.getObjListSize()));
         }
      }
   }
   else{
      //index-opt
      if(index >= mtest.getObjListSize() && !array_opt){
         cerr << "Size of object list (" << mtest.getObjListSize() << ") is <= " << index << "!!" << endl;
         return errorOption(CMD_OPT_ILLEGAL,to_string(index));
      }
      if(index >= mtest.getArrListSize() && array_opt){
         cerr << "Size of array list (" << mtest.getArrListSize() << ") is <= " << index << "!!" << endl;
         return errorOption(CMD_OPT_ILLEGAL,to_string(index));
      }

      if(array_opt){
         mtest.deleteArr(index);
      }
      else{
         mtest.deleteObj(index);
      }
   }

   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}
