#ifndef RRR_H
#define RRR_H

#include <vector>

#include "boost/shared_array.hpp"
#include "common.h"
#include "utility.h"
#include "CountCube.h"

using namespace std;

namespace indexes
{

class RRRSequence
{
private:
    friend class RRR;
    size_type _size;
    boost::shared_array<uint> classes;
    // prefix sum (on bits) for offsets
    // boost::shared_array<int> prefix_sum;
    vector<int> offsets;
    typedef boost::shared_array<int> inter_t;
    inter_t intermediates;
    
    size_type num_super_blocks;
    
    RRRSequence(const boost::shared_array<uint> & classes_in,
        const vector<int> & offsets_in,
        const size_type arity, const size_type blocksize,
        const size_type s_block_factor,
        const size_type BITS_PER_CLASS, const CountCube & cc);
        
    size_type rank(symbol_t sym, size_type pos, size_type blocksize,
        size_type s_block_factor, size_type NUM_BITS_PER_CLASS,
        const CountCube & cc) const;
public:
    // empty constructor for us to hold in our outer vector
    RRRSequence();
    inline size_type size() { return sizeof(*this) + _size; }
};

class RRR
{
public:
    // Pad blocks with this symbol if they don't align perfectly
    static const symbol_t PAD_VALUE;
    
private:
    const size_type ARITY;
    const size_type BLOCK_SIZE;
    const size_type SUPER_BLOCK_FACTOR;
    const size_type BITS_PER_CLASS;
    size_type sequence_sizes;
    
    CountCube countCube;
public:
    RRR(size_type arity, size_type block_size, size_type s_block_factor);
    
    RRRSequence build(const sequence_t & seq);
    inline size_type rank(symbol_t symbol, size_type position,
        const RRRSequence & seq) const
    {
        return seq.rank(symbol, position, BLOCK_SIZE, SUPER_BLOCK_FACTOR,
            BITS_PER_CLASS, countCube);
    }
    inline void seal() { countCube.seal(); }
    inline size_type size() { return sizeof(*this) + 
        countCube.size(); }
    inline size_type seqSize() { return sequence_sizes; }
};

} // end of namespace

#endif // end of include guard
