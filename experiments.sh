#!/bin/bash
set -e
RUN=./wt_test
DATA_DIR=data
OUTPUT_DIR=results
SWIT_EXT=.swit.bwt
TEXT_EXT=MB.bwt
RUNS=3
MAX_ARITY=16 # only goes up to 8 for my RRR
STRUCTS=('ab-rrr' 'fc-rrr' 'simple' 'multi-01rrr', 'multi-sdarray')

#attempt to recompile, ensure we aren't using debug=1
scons debug=0

for t in {0..4}
do
    struct=${STRUCTS[$t]}

    for (( arity=2; arity<=MAX_ARITY; arity*=2 ))
    do
        # don't want to test at arity 16 for my rrr, as it uses too much mem
        if [ $t = 0 ] && [ $arity = $MAX_ARITY ]; then
            break
        fi
        mkdir -p $OUTPUT_DIR/$struct/arity$arity/
        
        # Ascii files
        for file in $DATA_DIR/*$TEXT_EXT
        do
            OUT_FILE=`echo $file | sed 's/data\///'`
            OUT_FILE=$OUTPUT_DIR/$struct/arity$arity/$OUT_FILE
            
            for (( i=1; i<=RUNS; i++ ))
            do
                echo $struct - $file - arity: $arity - RUN NUMBER $i...
                touch $OUT_FILE.$i
                $RUN -f $file -a $arity -t $t >> $OUT_FILE.$i
            done
        done

        # Integer Files
        for file in $DATA_DIR/*$SWIT_EXT
        do
            OUT_FILE=`echo $file | sed 's/data\///'`
            OUT_FILE=$OUTPUT_DIR/$struct/arity$arity/$OUT_FILE
            for (( i=1; i<=RUNS; i++ ))
            do
                echo $struct - $file - arity: $arity - RUN NUMBER $i...
                touch $OUT_FILE.$i
                $RUN -f $file -a $arity -i -t $t >> $OUT_FILE.$i
            done
        done
        
        # Don't alter arity for Claude's
        if [ $t = 1 ]; then
            break
        fi
    done

done