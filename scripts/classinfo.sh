#!/bin/bash

DIR=`echo $0 | sed -E 's/\/classinfo.sh$//'`

for file in $1ab-rrr/arity*/*MB.bwt.1
do
    ARITY=`echo $file | sed -E 's/results\/ab-rrr\/arity([0-9]+).*/\1/'`
    TYPE=`echo $file | sed 's/.*\/\(.*\)\.\([0-9][0-9]MB\)\.bwt\.1/\1/'`
    SIZE=`echo $file | sed 's/.*\/\(.*\)\.\([0-9][0-9]MB\)\.bwt\.1/\2/'`
    CLASSES=`grep 'Classes' $file | awk '{print $4}'`
    OFFSETS=`grep 'Offsets' $file | awk '{print $4}'`
    BLOCKSIZE=`grep 'Blocksize' $file | awk '{print $3}'`
    MAX_CLASSES=`$DIR/maxclass.py $ARITY $BLOCKSIZE`
    MAX_OFFSETS=`$DIR/maxoffset.py $ARITY $BLOCKSIZE`
    echo $ARITY $TYPE $SIZE $CLASSES $OFFSETS $MAX_CLASSES $MAX_OFFSETS
done

for file in $1ab-rrr/arity*/*swit.bwt.1
do
    ARITY=`echo $file | sed -E 's/results\/ab-rrr\/arity([0-9]+).*/\1/'`
    TYPE=`echo $file | sed 's/.*\/\(.*\)\.\([0-9][0-9]MB\)\.swit\.bwt\.1/\1/'`
    TYPE=$TYPE.ints
    SIZE=`echo $file | sed 's/.*\/\(.*\)\.\([0-9][0-9]MB\)\.swit\.bwt\.1/\2/'`
    CLASSES=`grep 'Classes' $file | awk '{print $4}'`
    OFFSETS=`grep 'Offsets' $file | awk '{print $4}'`
    MAX_CLASSES=`$DIR/maxclass.py $ARITY $BLOCKSIZE`
    MAX_OFFSETS=`$DIR/maxoffset.py $ARITY $BLOCKSIZE`
    echo $ARITY $TYPE $SIZE $CLASSES $OFFSETS $MAX_CLASSES $MAX_OFFSETS
done