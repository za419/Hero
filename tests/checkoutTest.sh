#!/bin/bash

# Tests the checkout function

../x64/Debug/VersionControl.exe init
git checkout eb2cef163bffc4f3f38663843e5a35a0a4f5744a
../x64/Debug/VersionControl.exe commit checkoutTest.txt
git checkout add-checkout
../x64/Debug/VersionControl.exe commit checkoutTest.txt

i=0
hash=""
../x64/Debug/VersionControl.exe log | while read line
do
    if [ "$line" == "commit *" ]; then
        if [ $i == 1 ]; then
            hash = echo $line | sed -ne 's/^commit //p'
        else
            i=i+1
        fi
    fi
done

../x64/Debug/VersionControl.exe checkout $hash
cat checkoutTest.txt
sleep 10
../x64/Debug/VersionControl.exe checkout HEAD
cat checkoutTest.txt
