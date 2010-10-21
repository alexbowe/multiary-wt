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
    const wt_sequence_t ALPHABET;
    encoding_heap_t encoding;
    
    size_type seq_size;
    
    void encodeNodeRecursive(const wt_sequence_t & sequence,
        size_type left, size_type right, size_type nodeIdx);
    
    size_type rankRecursive(T symbol, size_type index,
        size_type left, size_type right, size_type nodeIdx) const;
public:
    WaveletTree(const wt_sequence_t & sequence, size_type arity,
        size_type block_size, size_type s_block_factor);
    inline size_type rank(T symbol, size_type index) const
    {
        // could check that symbol is in alphabet too at runtime...
        return rankRecursive(symbol, index, 0, ALPHABET.size() - 1);
    }
    inline const basic_string<T> & getAlpha()
    { return ALPHABET; }
    inline size_type size()
    {
        return sizeof(encoding) + sizeof(ALPHABET)
            + ALPHABET.size() * sizeof(T);
    }
    inline size_type seqSize() { 
        TRACE(("SEQ_SIZE: %d\n", seq_size));
        return seq_size; }
    inline size_type rrrSize() { return rrr.size(); }
    inline size_type numNodes() { return encoding.size(); }
};

template <class T>
WaveletTree<T>::WaveletTree(const wt_sequence_t & sequence, size_type arity,
    size_type block_size, size_type s_block_factor) : 
    ARITY(arity),
    rrr(arity, block_size, s_block_factor),
    ALPHABET(getAlphabet(sequence)), seq_size(0)
{
    myAssert(ARITY >= 2);

    // WT should always be balanced by definition
    size_type numLevels = getNumSymbolsRequired(ALPHABET.length(), ARITY);
    size_type numNodes = getNumBalancedTreeNodes(numLevels, ARITY);
    
    encoding = encoding_heap_t(numNodes);
    
    encodeNodeRecursive(sequence, 0, ALPHABET.size() - 1);
    
    rrr.seal();
}

template <class T>
void WaveletTree<T>::encodeNodeRecursive(const wt_sequence_t & sequence,
    size_type left, size_type right, size_type nodeIdx = 0)
{
    SymbolEncoder<T> enc(ALPHABET, ARITY, left, right);
    // for our baseline this will be binary and stored in bitvectors...
    sequence_t mapped_sequence = map_func<symbol_t>(enc, sequence);
    
    myAssert(nodeIdx < encoding.size());
    
    encoding[nodeIdx] = rrr.build(mapped_sequence);
    seq_size += encoding[nodeIdx].size();
    
    // If we have an alphabet of sigma = arity, we won't gain any more
    // information by encoding sub-levels... it is represented in the same
    // amount of symbols here
    if (getSigma(left, right) <= ARITY)
        return;
    
    size_type step = getEncodingStep(getSigma(left, right), ARITY);
    size_type childLeft = left;
    size_type childRight = childLeft + step - 1;
    
    for (size_type child = 0; child < ARITY; child++)
    {
        wt_sequence_t childText;
        
        // last child
        if (child == ARITY - 1)
            childRight = right;
        
        {
            SymbolFilter<T> f(enc, child);
            childText = filter_func(f, sequence);
        }
        
        size_type childIdx = getHeapChildIndex(nodeIdx, child + 1, ARITY);
        
        encodeNodeRecursive(childText, childLeft, childRight,
            childIdx);
        
        childLeft += step;
        childRight += step;
    }
}

template <class T>
size_type WaveletTree<T>::rankRecursive(T symbol, size_type pos,
    size_type left, size_type right, size_type nodeIdx = 0) const
{
    myAssert(nodeIdx < encoding.size());
    
    size_type child;
    // Don't need this functor after this step, so create in a block
    {
        SymbolEncoder<T> enc(ALPHABET, ARITY, left, right);
        child = enc(symbol);
    }
    
    // Query RRR
    size_type next_ind = rrr.rank(child, pos, encoding[nodeIdx]);
        
    // If there were none at all of this encoding...
    if (next_ind == 0)
    {
        return 0;
    }
            
    // Leaf node?
    if ( getSigma(left, right) <= ARITY )
    {
        return next_ind;
    }
    
    // Find next child index
    size_type childIdx = getHeapChildIndex(nodeIdx, child + 1, ARITY);
    
    // Find new alphabet range
    size_type step = getEncodingStep(getSigma(left, right), ARITY);
    size_type childLeft = left + child * step;
    size_type childRight = (child == ARITY - 1)? right: childLeft + step - 1;
    
    return rankRecursive(symbol, next_ind - 1, 
        childLeft, childRight, childIdx);
}

}

#endif