#!/bin/bash

re=''

for file in $1*/arity*/*
do
    RUN=`echo $file | sed -E 's/.*([0-9])$/\1/'`
    # SKIP other runs, since we process multiples with awk
    if [ $RUN -ge 2 ]; then
        continue
    fi
    STRUCT=`echo $file | sed -E \
    's/[^/]+\/([^/]+)\/arity([0-9]+)\/(.*)\.([0-9][0-9]MB).*/\1/'`
    ARITY=`echo $file | sed -E \
    's/[^/]+\/([^/]+)\/arity([0-9]+)\/(.*)\.([0-9][0-9]MB).*/\2/'`
    TYPE=`echo $file | sed -E \
    's/[^/]+\/([^/]+)\/arity([0-9]+)\/(.*)\.([0-9][0-9]MB).*/\3/'`
    SIZE=`echo $file | sed -E \
    's/[^/]+\/([^/]+)\/arity([0-9]+)\/(.*)\.([0-9][0-9]MB).*/\4/'`
    INTS=`grep -o swit $file`
    if [ "$INTS" != "" ]; then
        TYPE=$TYPE.ints
    fi
    
    BASE=`echo $file | sed -E 's/(.*)[0-9]$/\1/'`
    MIN_TIME=`grep 'Time' $BASE* \
        | awk 'NR==1 { min=$6 }; $6 < min{min=$6}; END{print min}'`
    SEQ_SIZE=`grep 'Seq Size' $file | awk '{print $6}'`
    TAB_SIZE=`grep 'Table Size' $file | awk '{print $6}'`
    WT_SIZE=`grep 'WT Size' $file | awk '{print $5}'`
    # I made an error printing the results in the test harness
    if [ "$STRUCT" != "fc-rrr" ]; then
        WT_SIZE=$(( $WT_SIZE + $SEQ_SIZE ))
    fi
    
    echo $STRUCT $ARITY $TYPE $SIZE $MIN_TIME $WT_SIZE $TAB_SIZE
done
