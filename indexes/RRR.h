#ifndef RRR_H
#define RRR_H

#include <vector>

#include "boost/shared_array.hpp"
#include "common.h"
#include "CountCube.h"

using namespace std;

namespace indexes
{

class RRRSequence
{
private:
    friend class RRR;
    //shared_array i manipulate myself (using the stuff in basics.h)
    // orrrr dynamic_bitset
    vector<int> classes;
    // prefix sum (on bits) for offsets
    //boost::shared_array<int> prefix_sum;
    vector<int> offsets;
    // shared ptr of 2D array: intermediate counts per block (but do some
    // math to get superblock limit ones...)
    typedef boost::shared_array<int> inter_t;
    inter_t intermediates;
    
    RRRSequence(const vector<int> & classes_in, const vector<int> & offsets_in,
        const size_type arity, const size_type blocksize,
        const size_type s_block_factor, const CountCube & cc);
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
    
    CountCube countCube;
public:
    RRR(size_type arity, size_type block_size, size_type s_block_factor);
    
    RRRSequence build(const sequence_t & seq);
    size_type rank(symbol_t symbol, size_type position,
        const RRRSequence & seq) const;
    inline void seal() { countCube.seal(); }
};

} // end of namespace

#endif // end of include guard
