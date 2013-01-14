#!/bin/bash

# usage: ./scripts/getstats.sh results/fc-rrr/ > results/filestats.csv

echo 'type,size,length,sigma'
for file in $1*/*MB.bwt.1
do
    TYPE=`echo $file | sed 's/.*\/\(.*\)\.\([0-9][0-9]MB\)\.bwt\.1/\1/'`
    SIZE=`echo $file | sed 's/.*\/\(.*\)\.\([0-9][0-9]MB\)\.bwt\.1/\2/'`
    LENGTH=`grep 'Text Length' $file | awk '{print $4}'`
    SIGMA=`grep 'Sigma' $file | awk '{print $3}'`
    echo $TYPE,$SIZE,$LENGTH,$SIGMA
done

for file in $1*/*MB.swit.bwt.1
do
    TYPE=`echo $file | sed 's/.*\/\(.*\)\.\([0-9][0-9]MB\)\.swit\.bwt\.1/\1/'`
    SIZE=`echo $file | sed 's/.*\/\(.*\)\.\([0-9][0-9]MB\)\.swit\.bwt\.1/\2/'`
    LENGTH=`grep 'Text Length' $file | awk '{print $4}'`
    SIGMA=`grep 'Sigma' $file | awk '{print $3}'`
    echo $TYPE.ints,$SIZE,$LENGTH,$SIGMA
done
