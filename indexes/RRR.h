#ifndef RRR_H
#define RRR_H

#include <vector>

#include "common.h"
#include "CountCube.h"

using namespace std;

namespace indexes
{

class RRRSequence
{
private:
    //friend class RRR;
    // orrrr dynamic_bitset
    boost::shared_array<int> classes;
    // prefix sum (on bits) for offsets
    boost::shared_array<int> prefix_sum;
    boost::shared_array<int> offsets;
    // shared ptr of 2D array: intermediate counts per block (but do some
    boost::shared_array<int> count_block; // per symbol per block per superblock
    // math to get superblock limit ones...)
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
    size_type rank(symbol_t symbol, size_type position, RRRSequence & seq)
        const;
    inline void seal() { countCube.seal(); }
};

} // end of namespace

#endif // end of include guard
