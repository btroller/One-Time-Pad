#!/bin/bash

./otp -g -p pad
./otp -e -o enc -p pad < test05.in
./otp -d -i enc -o dec -p pad

diff -q test05.in dec

if [ $? != 0 ]; then
   exit 1
fi

rm pad enc dec
