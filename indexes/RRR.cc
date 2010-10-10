#include "RRR.h"
#include "Debug.h"

#include <vector>

using namespace indexes;

/** Constructs a RRR of specified arity, block size and super block factor. */
RRR::RRR(size_type arity, size_type block_size, size_type s_block_factor) :
    ARITY(arity), BLOCK_SIZE(block_size), SUPER_BLOCK_FACTOR(s_block_factor)
{
    // TODO: Init CountCube
}

RRRSequence RRR::build(const vector<symbol_t> & v)
{
    size_type classNum, offset;
    vector<symbol_t> block(BLOCK_SIZE, 0);
    
    // loop over in multiples of BLOCK_SIZE (pad with zeros)
    int block_ind = 0;
    vector<symbol_t>::const_iterator it;
    for (it = v::begin(); it != v::end(); it++)
    {
        // build buffer (vector)
        block[block_ind] = *it;
        
        block_ind++;
        if (element_counter == BLOCK_SIZE)
        {
            // insert block into CountCube
            // counts.add(block, &classNum, &offset);
            // add class to class sequence, offset to offset sequence
        }
    }
    
    return RRRSequence();
}
