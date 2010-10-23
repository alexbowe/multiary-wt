#ifndef M_RRR_WAVELETTREE_H
#define M_RRR_WAVELETTREE_H

#include <string>
#include <vector>
#include "boost/dynamic_bitset.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/shared_array.hpp"
#include "common.h"
#include "utility.h"
#include "debug.h"
#include "WTFunctional.h"
#include "BitSequenceRRR.h"

using namespace std;
using namespace boost;

namespace cds = cds_static;

namespace indexes
{

typedef struct node_struct
{
    typedef boost::shared_array<uint> count_t;
    typedef boost::shared_ptr<cds::BitSequenceRRR> rrrseq_ptr;
    size_type length;
    count_t sym_counts;
    rrrseq_ptr rrrseq;
} m_rrr_encoding_node_t;

// int or char... determines the type of basic_string we use, and rank query
// we accept
template <class T>
class MultiRRRWaveletTree
{
private:
    typedef basic_string<T> wt_sequence_t;
    typedef vector<m_rrr_encoding_node_t> encoding_heap_t;
    
    const size_type ARITY;
    
    const wt_sequence_t ALPHABET;
    encoding_heap_t encoding;
    size_type seq_size;
    
    void encodeNodeRecursive(const wt_sequence_t & sequence,
        size_type left, size_type right, size_type nodeIdx);
    
    size_type rankRecursive(T symbol, size_type index,
        size_type left, size_type right, size_type nodeIdx) const;
public:
    MultiRRRWaveletTree(const wt_sequence_t & sequence, size_type arity);
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
    inline size_type numNodes() { return encoding.size(); }
};

template <class T>
MultiRRRWaveletTree<T>::MultiRRRWaveletTree(const wt_sequence_t & sequence, size_type arity) : 
    ARITY(arity),
    ALPHABET(getAlphabet(sequence)), seq_size(0)
{
    myAssert(ARITY >= 2);

    // WT should always be balanced by definition
    size_type numLevels = getNumSymbolsRequired(ALPHABET.length(), ARITY) + 1;
    size_type numNodes = getNumBalancedTreeNodes(numLevels, ARITY);
    
    encoding = encoding_heap_t(numNodes);
    
    encodeNodeRecursive(sequence, 0, ALPHABET.size() - 1);
}

template <class T>
m_rrr_encoding_node_t makeMultiRRRNode(const basic_string<T> & seq, size_type arity, size_type & seq_size)
{
    size_type length = seq.length();
    size_t num_bits = arity * length;
    size_type num_uints = getUintsRequired(1, num_bits);
    
    uint * temp = (uint*) calloc(num_uints, sizeof(uint));
    
    boost::shared_array<uint> counts = boost::shared_array<uint>(
        new uint[arity - 1]);
    seq_size += (arity - 1) * sizeof(uint);
    // X axis = pos
    // Y axis = sym
    
    for (size_type sym = 0; sym < arity - 1; sym++)
        counts[sym] = 0;
        
    for ( size_type pos = 0; pos < length; pos++)
    {
        for (size_type sym = 0; sym < arity; sym++)
        {
            size_type index = sym * length + pos;
            if ((unsigned int)seq[pos] == sym)
            {
                counts[sym]++;
                bitset(temp, index);
            }
        }
    }
    
    // accumulate
    for (size_type sym = 1; sym < arity - 1; sym++)
        counts[sym] += counts[sym-1];
    
    m_rrr_encoding_node_t node;
    node.length = num_bits;
    node.sym_counts = counts;
    node.rrrseq = m_rrr_encoding_node_t::rrrseq_ptr(
        new cds::BitSequenceRRR(temp, num_bits));
    free(temp);
    
    return node;
}

inline size_type multiRRRRank(size_type sym, size_type pos,
    const m_rrr_encoding_node_t &
    node, size_type arity)
{
    size_type length = node.length / arity;
    
    // first position for this symbol, since we want the count before that
    // (which may be other symbols)
    // if sym is 0, dont need to do this -> there is nothing before it
    //size_type rank_0 = (sym == 0)? 0 : 
    //    node.rrrseq->rank1((sym - 1) * length + length - 1);
    size_type rank_0 = (sym == 0)? 0 : node.sym_counts[sym - 1];
    // position that we actually want to query
    size_type real_pos = sym * length + pos;
    
    size_type rank_1 = node.rrrseq->rank1(real_pos);
    
    return rank_1 - rank_0;
}

template <class T>
void MultiRRRWaveletTree<T>::encodeNodeRecursive(const wt_sequence_t & sequence,
    size_type left, size_type right, size_type nodeIdx = 0)
{
    SymbolEncoder<T> enc(ALPHABET, ARITY, left, right);
    
    {
        sequence_t mapped_sequence = map_func<symbol_t>(enc, sequence);
        myAssert(nodeIdx < encoding.size());

        //encoding[nodeIdx] = rrr.build(mapped_sequence);
        encoding[nodeIdx] = makeMultiRRRNode(mapped_sequence, ARITY, seq_size);
        seq_size += encoding[nodeIdx].rrrseq->getSize() + 
            sizeof(m_rrr_encoding_node_t);
    }
    
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
size_type MultiRRRWaveletTree<T>::rankRecursive(T symbol, size_type pos,
    size_type left, size_type right, size_type nodeIdx = 0) const
{
    myAssert(nodeIdx < encoding.size());
    
    size_type child;
    // Don't need this functor after this step, so create in a block
    {
        SymbolEncoder<T> enc(ALPHABET, ARITY, left, right);
        child = enc(symbol);
    }
    
    size_type next_ind = multiRRRRank(child, pos, encoding[nodeIdx], ARITY);
    
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