#!/bin/bash

#Search for .in files in the current directory
fileFound=false
for file in ./*.in
do
   fileFound=true;
   break
done

#Check if .in files exist in the current directory
#   If not, complain to user and exit
if [ $fileFound = false ]; then
   echo "Error: no files with .in extension present"
   exit 1
fi

make

#Check if otp file exists in the current directory
#   If not, complain to user and exit
if [ ! -f ./otp ]; then
   echo "Error: no otp executable present"
   exit 1
fi

echo
echo "Performing tests: "
for testInput in *.in; do

   # Run test 1
   ./otp -g -p EX_PAD
   ./otp -e -i $testInput -o $testInput.enc -p EX_PAD
   ./otp -d -i $testInput.enc -o $testInput.dec -p EX_PAD

   # diff the results
   diff -q $testInput $testInput.dec

   # remove temp files
   rm $testInput.enc $testInput.dec EX_PAD
 

   # Run test 2
   ./otp -p EX_PAD 7000000 -g
   ./otp -i $testInput -p EX_PAD -o $testInput.enc -e
   ./otp -p EX_PAD -i $testInput.enc -d -o $testInput.dec 

   # diff the results
   diff -q $testInput $testInput.dec

   # remove temp files
   rm $testInput.enc $testInput.dec EX_PAD

done

exit
