#include "RRR.h"
#include "Debug.h"

#include <vector>
#include <iostream>

using namespace indexes;

/** Constructs a RRR of specified arity, block size and super block factor. */
RRR::RRR(size_type arity, size_type block_size, size_type s_block_factor) :
    ARITY(arity), BLOCK_SIZE(block_size), SUPER_BLOCK_FACTOR(s_block_factor)
{
    // TODO: Init CountCube
}

/** Builds RRR Sequence from input vector. */
RRRSequence RRR::build(const vector<symbol_t> & seq)
{
    size_type classNum, offset;
    vector<symbol_t> block(BLOCK_SIZE, 0);
    
    // loop over in multiples of BLOCK_SIZE (padded with zeros)
    size_type block_ind = 0;
    vector<symbol_t>::const_iterator it;
    for (it = seq.begin(); it != seq.end(); it++)
    {
        // build buffer
        block[block_ind++] = *it;
        
        // Finished block?
        // if block filled or if we got to the end of the vector...
        if ( (block_ind == BLOCK_SIZE) || (it == seq.end()) )
        {
            // insert block into CountCube
            // counts.add(block, &classNum, &offset);
            
            // print it
            vector<symbol_t>::iterator block_it;
            for (block_it = block.begin(); block_it < block.end(); block_it++)
                cout << *block_it << ", ";
            cout << endl;
            
            // add class to class sequence, offset to offset sequence
            if ( it != seq.end() )
            {
                block_ind = 0;
                // reset block to 0s...
                block.assign(BLOCK_SIZE, 0);
            }
        }
    }
    
    return RRRSequence();
}
