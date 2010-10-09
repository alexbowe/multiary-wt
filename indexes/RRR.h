#ifndef RRR_H
#define RRR_H

#include "Debug.h"

using namespace std;

namespace indexes
{
// namespace RRR...
// RRR_utilities.h
// GTable
// typedef tuple<size_t bits, bla *>
// typedef vector<> row;
// vector<row>

class RRR
{
public:
    // Might change to char, bool, int, long, etc...
    typedef uint32_t symbol_t;
    
private:
    const size_t ARITY;
    const size_t BLOCK_SIZE;
    const size_t SUPER_BLOCK_FACTOR;
    
public:
    // these will have default values...
    // Could create hash functions from the first 2...
    RRR(size_t arity, size_t block_size, size_t superBlockFactor);
    
    // NOTE: Tomorrow: RRR.c, build without packing bits first...
    // CountCube dims: class, offset, (symbol), position
    
    // RRR should take bools, chars, unsigned ints 32, and unsigned longs
    // as input... maybe iterators...
    SuperBlock build(vector<symbol_t> & sequence);
    size_t rank(symbol_t symbol, size_t position, RRRSequence & seq) const;
    // void seal(); // to get rid of the space we use to map the blocks/classes
};

class RRRSequence
{
private:
    friend class RRR;
    // array of c... int...
    // array of o int*
    // prefix sum = int[]?
};

} // end of namespace

#endif // end of include guard
