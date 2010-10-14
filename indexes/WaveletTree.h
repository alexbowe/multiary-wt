#ifndef WAVELETTREE_H
#define WAVELETTREE_H

#include <string>
#include "boost/shared_ptr.hpp"
#include "common.h"
#include "utility.h"
#include "debug.h"
#include "WTFunctional.h"
#include "RRR.h"

using namespace std;
using namespace boost;

namespace indexes
{

// int or char... determines the type of basic_string we use, and rank query
// we accept
template <class T>
class WaveletTree
{
private:
    typedef basic_string<T> wt_sequence_t;
    typedef RRRSequence encoding_node_t;
    typedef vector<encoding_node_t> encoding_heap_t;
    
    const size_type ARITY;
    
    RRR rrr;
    wt_sequence_t alphabet;
    encoding_heap_t encoding;
    
    void encodeNodeRecursive(const wt_sequence_t & sequence,
        const wt_sequence_t & alphabet, size_type nodeIdx);
    
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
    size_type numLevels = getNumSymbolsRequired(alphabet.length(), ARITY);
    size_type numNodes = getNumBalancedTreeNodes(numLevels, ARITY);
    TRACE(("[WaveletTree.CTOR] numLevels: %d\n", numLevels));
    TRACE(("[WaveletTree.CTOR] numNodes: %d\n", numNodes));
    
    encoding = encoding_heap_t(numNodes);
    
    encodeNodeRecursive(sequence, alphabet);
}

template <class T>
void WaveletTree<T>::encodeNodeRecursive(const wt_sequence_t & sequence,
    const wt_sequence_t & alphabet, size_type nodeIdx = 0)
{   
    SymbolEncoder<T> enc(alphabet, ARITY);
    
    // for our baseline this will be binary and stored in bitvectors...
    sequence_t mapped_sequence = map_func<symbol_t>(enc, sequence);
    TRACE(("[WaveletTree.CTOR] Mapped: "));
    TRACE_SEQ((mapped_sequence));
    encoding[nodeIdx] = rrr.build(mapped_sequence);
    
    if (alphabet.length() <= ARITY)
        return;
    
    for (size_type child = 0; child < ARITY; child++)
    {
        SymbolFilter<T> f(enc, child);
        wt_sequence_t childText = filter_func(f, sequence);
        wt_sequence_t childAlpha = filter_func(f, alphabet);
        
        size_type childIdx = getHeapChildIndex(nodeIdx, child + 1, ARITY);
        
        TRACE(("[WaveletTree.CTOR] Child (%d): ", childIdx));
        TRACE_SEQ((childText));
        TRACE(("[WaveletTree.CTOR] Child Alpha: "));
        TRACE_SEQ((childAlpha));
        
        encodeNodeRecursive(childText, childAlpha, childIdx);
    }
}

template <class T>
size_type WaveletTree<T>::rank(T symbol, size_type index)
{
    return 0;
}

}

#endif