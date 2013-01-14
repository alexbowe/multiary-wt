#!/usr/bin/python

import sys
from binomial import *

def usage():
    sys.stderr.write("Usage: python maxclass.py <arity> <blocksize>\n")

def getMaxClass(arity, blocksize):
    return binomial(arity + blocksize - 1, arity - 1)

if __name__ == '__main__':
    if len(sys.argv) is not 3:
        usage()
        exit()
    try:
        arity = int(sys.argv[1])
        blocksize = int(sys.argv[2])
        
        print getMaxClass(arity, blocksize)
        
    except:
        usage()
