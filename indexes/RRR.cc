#include <vector>
#include <cmath>
#include <iostream>

#include "RRR.h"
#include "debug.h"

#include "utility.h"

using namespace indexes;

// Pad blocks with 0 if they don't allign correctly
const symbol_t RRR::PAD_VALUE = 0;

RRRSequence::RRRSequence(): num_super_blocks(0) { }

/** Constructs a RRR of specified arity, block size and super block factor. */
RRR::RRR(size_type arity, size_type block_size, size_type s_block_factor) :
    ARITY(arity),
    BLOCK_SIZE(block_size),
    SUPER_BLOCK_FACTOR(s_block_factor),
    countCube(arity, block_size) {}

/** Builds RRR Sequence from input vector. */
RRRSequence RRR::build(const sequence_t & seq)
{
    size_type classNum, offset;
    sequence_t block(BLOCK_SIZE, PAD_VALUE);
    
    vector<int> classes = vector<int>();
    vector<int> offsets = vector<int>();
    
    // loop over in multiples of BLOCK_SIZE (padded with zeros)
    size_type block_ind = 0;
    sequence_t::const_iterator it;
    
    // shouldn't be calling with an empty sequence
    myAssert(seq.begin() != seq.end());
    for (it = seq.begin(); it != seq.end(); )
    {
        // build buffer
        block[block_ind++] = *it++;
        
        // Finished block?
        // if block filled or if we got to the end of the vector...
        if ( (block_ind == BLOCK_SIZE) || (it == seq.end()) )
        {
            // insert block into CountCube
            countCube.add(block, classNum, offset);
            
            // add to the classes and offsets vectors...
            classes.push_back(classNum);
            offsets.push_back(offset);
            
            // Reset block to 0
            block.assign(BLOCK_SIZE, PAD_VALUE);
            block_ind = 0;
        }
    }
    
    return RRRSequence(classes, offsets, ARITY, BLOCK_SIZE,
        SUPER_BLOCK_FACTOR, countCube);
}

RRRSequence::RRRSequence(const vector<int> & classes_in,
    const vector<int> & offsets_in, const size_type arity,
    const size_type blocksize, const size_type s_block_factor, const CountCube
    & cc) :
    // these will have to be constructed in smarter ways :)
    // like, store a number to say how many bit are required for the classes?
    // and packing the offsets
    classes(classes_in), offsets(offsets_in)
{
    // these really must be the same length...
    myAssert(classes.size() == offsets.size());
    
    const unsigned int NUM_BLOCKS(classes.size());
    num_super_blocks = ceil(NUM_BLOCKS / (float) s_block_factor);
    //TRACE(( "[RRRSequence.CTOR] Num Super Blocks: %d\n", num_super_blocks ));
    
    // Z = sym (arity)
    // Y = super block (num_super_blocks)
    // X = block (s_block_factor) - arranged this way for caching
    intermediates = inter_t(new int[arity * num_super_blocks * s_block_factor]);
    
    // Populate intermediates table
    
    // declare running total vector
    vector<int> totals(arity, 0);
    // for each (C, O) at i:
    for (size_type i = 0; i < NUM_BLOCKS; i++)
    {
        // superblock i/s_block_factor
        size_type super_block_idx = i / s_block_factor;
        size_type block_idx  = i % s_block_factor;
        size_type classNum = classes[i];
        size_type offset = offsets[i];
        
        // Super block boundary:
        if (i > 0 && block_idx == 0)
        {
            //TRACE(("RESETING\n"));
            // reset running total
            totals.assign(arity, 0);
        }
        
        // get last value in block, for each symbol
        for (size_type sym = 0; sym < arity; sym++)
        {
            size_type rank = cc.rank(classNum, offset, sym, 
                blocksize - 1);
            
            // update running totals for super-block
            totals[sym] += rank;
            size_type intermediate_idx = get3DIdx(s_block_factor,
                num_super_blocks, block_idx, super_block_idx, sym);
            intermediates[intermediate_idx] = totals[sym];
            //TRACE(("S%d b%d %d: %d\n", super_block_idx, block_idx, sym,
            //    totals[sym]));
        }
    }
}

size_type RRRSequence::rank(symbol_t sym, size_type pos, size_type blocksize,
    const size_type s_block_factor, const CountCube & cc) const
{
    size_type sym_idx = pos % blocksize;
    size_type global_block_idx = pos / blocksize;
    size_type super_block_idx = global_block_idx / s_block_factor;
    size_type block_idx = global_block_idx % s_block_factor;
    
    //TRACE(("[RRRSequence.rank] Symbol Idx: %d\n", sym_idx));
    //TRACE(("[RRRSequence.rank] Block: %d\n", block_idx));
    //TRACE(("[RRRSequence.rank] Super Block: %d\n", super_block_idx));
    
    size_type count = 0;
    
    size_type inter_idx = 0;
    for (size_type i = 0; i < super_block_idx; i++)
    {
        //TRACE(("[RRRSequence.rank] i: %d\n", i));
        // last block for every super block previous to this one
        inter_idx = get3DIdx(s_block_factor, num_super_blocks,
            s_block_factor - 1, i, sym);
        count += intermediates[inter_idx];
    }
    //TRACE(("Count(1): %d\n", count));
    
    // look up prev block pos
    if (block_idx > 0)
    {
        //TRACE(("DOING SOMETHING DANGEROUS!\n"));
        inter_idx = get3DIdx(s_block_factor, num_super_blocks,
            block_idx - 1, super_block_idx, sym);
        count += intermediates[inter_idx];
    }
    //TRACE(("Count(2): %d\n", count));
    
    size_type cc_count = cc.rank(classes[global_block_idx],
        offsets[global_block_idx], sym, sym_idx);
    //TRACE(("FROM CC: %d\n", cc_count));
    // look up current block pos using CountCube
    count += cc_count;
    
    //TRACE(("[RRRSequence.rank] Count: %d\n", count));
    return count;
}