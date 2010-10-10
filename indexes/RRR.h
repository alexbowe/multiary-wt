#ifndef RRR_H
#define RRR_H

#include <vector>

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
    typedef unsigned int symbol_t;
    typedef unsigned long size_type;
    
private:
    // Pad blocks with 0 if they don't allign correctly
    static const symbol_t PAD_VALUE = 0;
    const size_type ARITY;
    // These should be const but may be initialised dynamically later
    // so may change
    const size_type BLOCK_SIZE;
    const size_type SUPER_BLOCK_FACTOR;
    
    // CountCube dims: class, offset, (symbol), position
    // Mappers for offset and classes
    
public:
    // these will have default values...
    // Could create hash functions from the first 2...
    RRR(size_type arity, size_type block_size, size_type s_block_factor);
    
    // RRR should take bools, chars, unsigned ints 32, and unsigned longs
    // as input... maybe iterators...
    RRRSequence build(const vector< symbol_t > & v);
    size_type rank(symbol_t symbol, size_type position, RRRSequence & seq)
        const;
};

} // end of namespace

#endif // end of include guard
