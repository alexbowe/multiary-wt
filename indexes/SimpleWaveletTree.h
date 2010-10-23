#ifndef SIMPLE_WAVELETTREE_H
#define SIMPLE_WAVELETTREE_H

#include <string>
#include <vector>
#include "boost/dynamic_bitset.hpp"
#include "common.h"
#include "utility.h"
#include "debug.h"
#include "WTFunctional.h"

using namespace std;
using namespace boost;

namespace indexes
{

// int or char... determines the type of basic_string we use, and rank query
// we accept
template <class T>
class SimpleWaveletTree
{
private:
    typedef basic_string<T> wt_sequence_t;
    typedef dynamic_bitset<> encoding_node_t;
    typedef vector<encoding_node_t> encoding_heap_t;
    
    const size_type ARITY;
    
    const wt_sequence_t ALPHABET;
    encoding_heap_t encoding;
    size_type seq_size;
    
    void encodeNodeRecursive(const wt_sequence_t & sequence,
        size_type left, size_type right, size_type nodeIdx);
    
    size_type rankRecursive(T symbol, size_type index,
        size_type left, size_type right, size_type nodeIdx) const;
public:
    SimpleWaveletTree(const wt_sequence_t & sequence, size_type arity);
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
    inline size_type seqSize() { return seq_size; }
    //inline size_type rrrSize() { return rrr.size(); }
    inline size_type numNodes() { return encoding.size(); }
};

template <class T>
SimpleWaveletTree<T>::SimpleWaveletTree(const wt_sequence_t & sequence, size_type arity) : 
    ARITY(arity),
    ALPHABET(getAlphabet(sequence)),
    seq_size(0)
{
    myAssert(ARITY >= 2);

    // WT should always be balanced by definition
    size_type numLevels = getNumSymbolsRequired(ALPHABET.length(), ARITY);
    size_type numNodes = getNumBalancedTreeNodes(numLevels, ARITY);
    
    encoding = encoding_heap_t(numNodes);
    
    encodeNodeRecursive(sequence, 0, ALPHABET.size() - 1);
}

template <class T>
dynamic_bitset<> makeSimpleNode(const basic_string<T> & seq, size_type arity)
{
    size_type length = seq.length();
    size_type num_vals = arity * length;
    
    dynamic_bitset<> node(num_vals);
    
    // X axis = pos
    // Y axis = sym
    for ( size_type pos = 0; pos < length; pos++)
    {
        for (size_type sym = 0; sym < arity; sym++)
        {
            size_type index = sym * length + pos;
            node[index] = ((unsigned int)seq[pos] == sym);
        }
    }
    return node;
}

inline size_type simpleRank(size_type sym, size_type pos, const dynamic_bitset<> &
    node, size_type arity)
{
    size_type length = node.size() / arity;
    size_type rank = 0;
    
    size_type pos_0 = sym * length;
    size_type pos_1 = sym * length + pos;
    for ( size_type i = pos_0; i <= pos_1; i++)
    {
        rank += (unsigned int)node[i];
    }
    return rank;
}


template <class T>
void SimpleWaveletTree<T>::encodeNodeRecursive(const wt_sequence_t & sequence,
    size_type left, size_type right, size_type nodeIdx = 0)
{
    SymbolEncoder<T> enc(ALPHABET, ARITY, left, right);
    // for our baseline this will be binary and stored in bitvectors...
    sequence_t mapped_sequence = map_func<symbol_t>(enc, sequence);
    
    myAssert(nodeIdx < encoding.size());
    
    //encoding[nodeIdx] = rrr.build(mapped_sequence);
    encoding[nodeIdx] = makeSimpleNode(mapped_sequence, ARITY);
    seq_size += encoding[nodeIdx].num_blocks() *
        sizeof(encoding_node_t::block_type) + sizeof(encoding_node_t);
    
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
size_type SimpleWaveletTree<T>::rankRecursive(T symbol, size_type pos,
    size_type left, size_type right, size_type nodeIdx = 0) const
{
    myAssert(nodeIdx < encoding.size());
    
    size_type child;
    // Don't need this functor after this step, so create in a block
    {
        SymbolEncoder<T> enc(ALPHABET, ARITY, left, right);
        child = enc(symbol);
    }
    
    size_type next_ind = simpleRank(child, pos, encoding[nodeIdx], ARITY);
    
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