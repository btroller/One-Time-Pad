#!/bin/bash

# Used to assert whether there's been a failure - useful for build systems 
#  running script
aTestHasFailed=false

# Search for .in files in the current directory
fileFound=false
for file in ./*.in
do
   fileFound=true;
   break
done

# Check if .in files exist in the current directory
#   If not, complain to user and exit
if [ $fileFound = false ]; then
   echo "Error: no files with .in extension present"
   exit 1
fi

make

# Check if otp file exists in the current directory
#   If not, complain to user and exit
if [ ! -f ./otp ]; then
   echo "Error: no otp executable present"
   exit 1
fi

diffCheckCleanup() {
   # Check that there are 2 args
   if [ "$#" -ne 2 ]; then
      echo "Usage: diffCheckCleanup testInput testNumber"
      return
   fi
   
   # Check for differences between initial and decrypted files
   diff -q $1 $1.dec

   # If there are differences, then complain
   if [ $? != 0 ]; then
      echo "Test ${2} for file ${1} has failed"
      aTestHasFailed=true
   fi

   # Clean up
   rm $1.enc $1.dec EX_PAD
}
 
for testInput in *.in; do

   # Run test 1
   ./otp -g -p EX_PAD
   ./otp -e -i $testInput -o $testInput.enc -p EX_PAD
   ./otp -d -i $testInput.enc -o $testInput.dec -p EX_PAD
   diffCheckCleanup $testInput 1 

   # Run test 2
   ./otp -p EX_PAD 7000000 -g
   ./otp -i $testInput -p EX_PAD -o $testInput.enc -e
   ./otp -p EX_PAD -i $testInput.enc -d -o $testInput.dec 
   diffCheckCleanup $testInput 2 

   # Run test 3
   ./otp -p EX_PAD 100 -g
   ./otp < $testInput -p EX_PAD > $testInput.enc -e
   ./otp -p EX_PAD < $testInput.enc -d > $testInput.dec 
   diffCheckCleanup $testInput 3 

done

if [ $aTestHasFailed == true ]; then
   echo "At least 1 test failed"
   exit 1
fi

echo "Tests passed"
exit 0
