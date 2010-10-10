#ifndef RRR_H
#define RRR_H

#include <vector>

#include "common.h"

using namespace std;

namespace indexes
{

class RRRSequence
{
private:
    friend class RRR;
    // array of c... int...
    // array of o int*
    // prefix sum = int[]?
    int a;
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
    
public:
    RRR(size_type arity, size_type block_size, size_type s_block_factor);
    
    RRRSequence build(const vector<symbol_t> & v);
    size_type rank(symbol_t symbol, size_type position, RRRSequence & seq)
        const;
    // seal(); // delegates seal to countcube
};

} // end of namespace

#endif // end of include guard
