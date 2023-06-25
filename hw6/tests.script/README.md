##### Test for all testcases in tests.fraig  #####

##### How to use #####
Step1: Put dofiles and all testcases in tests.fraig under main
Step2: In terminal
  1. script outputX.txt
  2. ./cirTest
  3. do doX
  4. exit
Step3:
  Remove Header and Bottom information in outputX.txt
Step4: In terminal
  diff outputX.txt doXoutput.txt

##### Purpose of each do file #####
do1
testing WRITE

do1a
testing WRITE and output to file

do2
testing PRINT (-summary / -pi / -po / -floating) 

do3
testing PRINT (-netlist) 

do4
testing GATE (without large testfile)

do5
testing GATE (with large testfile except sim12,sim13)

do6
testing GATE -fanin

do7
testing GATE -fanout
(known bug: In opt04.aag due to ref program limitation)

do8
General test for testcases in tests.fraig/ISCAS85 (C17 to C880)

do9
General test for testcases in tests.fraig/ISCAS85 (C1355 to C7552)

### For floating gate
1. use do.flt in tests.err
2. I have listed floating examples that occur in do1 to do7 in Floating.txt

### For error handling
1. use do.err and do.ok in tests.err
2. I have made some more error handling test case,
   which is named eXX.aag in subfolder ./Error_handling
   It can be tested with do10



