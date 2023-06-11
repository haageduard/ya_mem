#!/bin/bash
TEST_NAME=test_ya_mem
INC_PATH=../inc
LIB_PATH=../build-x86_64
LIB_NAME=ya_mem

if [ -e $TEST_NAME ]
then
rm $TEST_NAME
fi

if ! [ -e $LIB_PATH/lib$LIB_NAME.a ]
then
echo "library not found (not build yet?)"
exit
fi

gcc -std=c99 -Og -g main.c -I$INC_PATH -L$LIB_PATH -l$LIB_NAME -o $TEST_NAME

if [ $? == 0 ]
then
chmod +x $TEST_NAME
./$TEST_NAME
fi

if [ $? == 1 ]
then
echo "test success"
else
echo "test failed"
fi
