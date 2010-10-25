#!/usr/bin/python

import sys
from binomial import *

def usage():
    sys.stderr.write("Usage: python maxoffset.py <arity> <blocksize>\n")

def num_offsets(classes, arity, blocksize):
    result = 1
    remaining = blocksize
    for i in range(arity):
        result *= binomial(remaining, classes[i])
        remaining -= classes[i]
    return result

def genClasses(arity, blocksize):
    """
    Generates base classes with unique numbers that add up to blocksize.
    The results can then be used to calculate all permutations.
    
    >>> genClasses(2,2)
    [(2, 0), (1, 1), (0, 2)]
    """
    this_cell = blocksize
    remainder = 0
    results = []
    if blocksize is 0:
        return [tuple([0] * arity)]
    if arity is 1:
        return [(blocksize,)]
    for i in range(0, blocksize + 1):
        for r in genClasses(arity - 1, i):
            results += [tuple([this_cell - i]) + r]
    return results

def getMaxOffsets(arity, blocksize):
    cache = { 15:{ 2:32768,
                   4:1073741824,
                   8:35184372088832 } }
    
    if blocksize in cache and arity in cache[blocksize]:
        return cache[blocksize][arity]
        
    classes = genClasses(arity, blocksize)
    # for each one, calculate the num_offsets & accumulate it
    total = 0
    for c in classes:
        total += num_offsets(c, arity, blocksize)
    return total

if __name__ == '__main__':
    if len(sys.argv) is not 3:
        usage()
        exit()
    try:
        arity = int(sys.argv[1])
        blocksize = int(sys.argv[2])
        
        print getMaxOffsets(arity, blocksize)
        
    except:
        usage()