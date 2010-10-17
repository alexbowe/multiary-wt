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
    
    size_type fnumNodes;
    
    RRR rrr;
    const wt_sequence_t ALPHABET;
    encoding_heap_t encoding;
    
    void encodeNodeRecursive(const wt_sequence_t & sequence,
        const wt_sequence_t & alphabet, size_type nodeIdx);
    
    size_type rankRecursive(T symbol, size_type index,
        const wt_sequence_t & alphabet, size_type nodeIdx) const;
public:
    WaveletTree(const wt_sequence_t & sequence, size_type arity,
        size_type block_size, size_type s_block_factor);
    size_type rank(T symbol, size_type index) const;
};

template <class T>
WaveletTree<T>::WaveletTree(const wt_sequence_t & sequence, size_type arity,
    size_type block_size, size_type s_block_factor) : 
    ARITY(arity),
    rrr(arity, block_size, s_block_factor),
    ALPHABET(getAlphabet(sequence)), fnumNodes(0)
{
    myAssert(ARITY >= 2);

    // WT should always be balanced by definition
    size_type numLevels = getNumSymbolsRequired(ALPHABET.length(), ARITY);
    size_type numNodes = getNumBalancedTreeNodes(numLevels, ARITY);
    
    encoding = encoding_heap_t(numNodes);
    
    // Trace some stuff
    TRACE(("[WaveletTree.CTOR] Input: "));
    TRACE_SEQ((sequence));
    TRACE(("[WaveletTree.CTOR] Alphabet:  "));
    TRACE_SEQ((ALPHABET));
    TRACE(("[WaveletTree.CTOR] Sigma:     %d\n", ALPHABET.size()));
    TRACE(("[WaveletTree.CTOR] Arity:     %d\n", ARITY));
    TRACE(("[WaveletTree.CTOR] numLevels: %d\n", numLevels));
    TRACE(("[WaveletTree.CTOR] numNodes:  %d\n", numNodes));
    TRACE(("[WaveletTree.CTOR] Encoding Recursively...\n\n"));
    
    encodeNodeRecursive(sequence, ALPHABET);
    
    TRACE(("NumNodes: %d/%d\n", fnumNodes, numNodes ));
    rrr.seal();
}

template <class T>
void WaveletTree<T>::encodeNodeRecursive(const wt_sequence_t & sequence,
    const wt_sequence_t & alphabet, size_type nodeIdx = 0)
{   
    // if we have shot past our estimated tree size
    

    SymbolEncoder<T> enc(alphabet, ARITY);
    // for our baseline this will be binary and stored in bitvectors...
    sequence_t mapped_sequence = map_func<symbol_t>(enc, sequence);
      
    /* 
    TRACE(("[WaveletTree.encodeNodeRecursive] Node:  %d/%d\n", nodeIdx, 
        encoding.size() - 1));
    TRACE(("[WaveletTree.encodeNodeRecursive] Input: "));
    TRACE_SEQ((sequence));
    TRACE(("[WaveletTree.encodeNodeRecursive] Alpha: "));
    TRACE_SEQ((alphabet));
    TRACE(("[WaveletTree.encodeNodeRecursive]        "));
    TRACE_SEQ((map_func<symbol_t>(enc, alphabet)));
    TRACE(("[WaveletTree.encodeNodeRecursive] Sigma: %d\n", alphabet.size()));
    TRACE(("[WaveletTree.encodeNodeRecursive] Mapped: "));
    TRACE_SEQ((mapped_sequence));
    TRACE(("\n"));
    */
    
    myAssert(nodeIdx < encoding.size());
    
    encoding[nodeIdx] = rrr.build(mapped_sequence);
    fnumNodes++;
    // If we have an alphabet of sigma = arity, we won't gain any more
    // information by encoding sub-levels... it is represented in the same
    // amount of symbols here
    if (alphabet.length() <= ARITY)
        return;
    
    for (size_type child = 0; child < ARITY; child++)
    {
        wt_sequence_t childText;
        wt_sequence_t childAlpha;
        
        {
            SymbolFilter<T> f(enc, child);
            childText = filter_func(f, sequence);
            childAlpha = filter_func(f, alphabet);
        }
        
        size_type childIdx = getHeapChildIndex(nodeIdx, child + 1, ARITY);
        
        encodeNodeRecursive(childText, childAlpha, childIdx);
    }
}

template <class T>
inline size_type WaveletTree<T>::rank(T symbol, size_type index) const
{
    // could check that symbol is in alphabet too at runtime...
    return rankRecursive(symbol, index, ALPHABET);
}

template <class T>
size_type WaveletTree<T>::rankRecursive(T symbol, size_type pos,
    const wt_sequence_t & alphabet, size_type nodeIdx = 0) const
{        
    myAssert(nodeIdx < encoding.size());
    size_type next_ind;
    size_type child;
    wt_sequence_t childAlpha;
    
    //TRACE(("ENTER\n"));
    //TRACE(("POS: %d\n", pos));
    //myAssert(nodeIdx < encoding.size());
    
    //TRACE(("\n"));
    //TRACE(("NodeIdx: %d\n", nodeIdx));
    //TRACE(("[WaveletTree.rank] Symbol, Pos: '%d', %d\n", symbol, pos));
    //TRACE(("[WaveletTree.rank] Alpha: "));
    //TRACE_SEQ((alphabet));
    
    // Don't need these functors after this step, so create them in a block
    {
        SymbolEncoder<T> enc(alphabet, ARITY);
        child = enc(symbol);
        SymbolFilter<T> f(enc, child);
        
        //TRACE(("enc() -> %d\n", enc(symbol)));
        next_ind = rrr.rank(enc(symbol), pos, encoding[nodeIdx]);
        //TRACE(("next_ind = %d\n", next_ind));
        if (next_ind == 0)
            return 0;
        
        childAlpha = filter_func(f, alphabet);
    }
    
    // Leaf node?
    if ( alphabet.length() <= ARITY )
        return next_ind;
    
    size_type childIdx = getHeapChildIndex(nodeIdx, child + 1, ARITY);
    
    return rankRecursive(symbol, next_ind - 1, childAlpha, childIdx);
}

}

#endif