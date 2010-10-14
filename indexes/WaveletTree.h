#ifndef WAVELETTREE_H
#define WAVELETTREE_H

#include <string>
#include <iostream>
#include "common.h"
#include "utility.h"
#include "debug.h"
#include "RRR.h"

namespace indexes
{

template <class T>
class WaveletTree
{
private:
    typedef basic_string<T> wt_sequence_t;
    
    const size_type ARITY;
    
    RRR rrr;
    wt_sequence_t alphabet;
    vector<RRRSequence> encoding;
    
public:
    WaveletTree(const wt_sequence_t & sequence, size_type arity,
        size_type block_size, size_type s_block_factor);
    size_type rank(T symbol, size_type index);
};

template <class T>
WaveletTree<T>::WaveletTree(const wt_sequence_t & sequence, size_type arity,
    size_type block_size, size_type s_block_factor) : 
    ARITY(arity),
    rrr(arity, block_size, s_block_factor)
{
    alphabet = getAlphabet(sequence);
    
    TRACE(("[WaveletTree.CTOR] Input:    "));
    TRACE_SEQ((sequence));
    TRACE(("[WaveletTree.CTOR] Alphabet: "));
    TRACE_SEQ((alphabet));
    
    // WT should always be balanced by definition
    int numLevels = getNumSymbolsRequired(alphabet.length(), ARITY);
    int numNodes = getNumBalancedTreeNodes(numLevels, ARITY);
    TRACE(("[WaveletTree.CTOR] numLevels: %d\n", numLevels));
    TRACE(("[WaveletTree.CTOR] numNodes: %d\n", numNodes));
    
    
}
    
}

#endif