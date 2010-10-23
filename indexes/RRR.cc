#include "RRR.h"

#include <vector>
#include <cmath>
#include <iostream>

#include "debug.h"
#include "utility.h"

using namespace indexes;

// Pad blocks with 0 if they don't allign correctly
const symbol_t RRR::PAD_VALUE = 0;

RRRSequence::RRRSequence(): _size(0), num_super_blocks(0),
    BITS_PER_CLASS(0) { }

/** Constructs a RRR of specified arity, block size and super block factor. */
RRR::RRR(size_type arity, size_type block_size, size_type s_block_factor) :
    ARITY(arity),
    BLOCK_SIZE(block_size),
    SUPER_BLOCK_FACTOR(s_block_factor),
    MAX_BITS_PER_CLASS(ceil(ARITY * log(BLOCK_SIZE + 1)/log(2))),
    countCube(arity, block_size)
{ }

/** Builds RRR Sequence from input vector. */
RRRSequence RRR::build(const sequence_t & seq)
{
    size_type maxClass = 0;
    size_type classNum, offset;
    sequence_t block(BLOCK_SIZE, PAD_VALUE);
    size_type numBlocks = ceil(seq.size() / (float)BLOCK_SIZE);
    
    size_type numUints = numBlocks * MAX_BITS_PER_CLASS;
    boost::shared_array<uint> classes =
        boost::shared_array<uint>(new uint[numUints]);
    vector<int> offsets = vector<int>(numBlocks);
    
    // loop over in multiples of BLOCK_SIZE (padded with zeros)
    size_type block_ind = 0;
    size_type block_ind_total = 0;
    sequence_t::const_iterator it;
    
    size_type offset_bits_total = 0;
    
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
            size_type bits = countCube.getNumOffsetBits(classNum);
            offset_bits_total += bits;
            myAssert(bits > 0);
            
            if (classNum > maxClass) maxClass = classNum;
            
            // add to the classes and offsets vectors...
            myAssert(block_ind_total < numBlocks);
            set_field(classes.get(), MAX_BITS_PER_CLASS, block_ind_total,
                classNum);
            offsets[block_ind_total++] = offset;
            
            // Reset block to 0
            block.assign(BLOCK_SIZE, PAD_VALUE);
            block_ind = 0;
        }
    }
    
    // Should have filled the whole thing up...
    myAssert(block_ind_total == numBlocks);
    
    size_type bitsPerClass = getBitsRequired(maxClass);
    
    // shrink class array and offsets
    size_type o_pos = 0;
    size_type offsetUints = getUintsRequired(1, offset_bits_total);
    boost::shared_array<uint> offsetsShrunk = 
        boost::shared_array<uint>(new uint[offsetUints]);
    boost::shared_array<uint> classesShrunk = 
        boost::shared_array<uint>(new uint[numBlocks * bitsPerClass]);
        
    for (size_type i = 0; i < numBlocks; i++)
    {
        // shrink class
        size_type c = get_field(classes.get(), MAX_BITS_PER_CLASS, i);
        set_field(classesShrunk.get(), bitsPerClass, i, c);
        myAssert(c == get_field(classesShrunk.get(), bitsPerClass, i));
        
        // shrink offset
        size_type o_bits = countCube.getNumOffsetBits(c);
        set_var_field(offsetsShrunk.get(), o_pos, o_pos + o_bits - 1,
            (uint)offsets[i]);
        myAssert((unsigned long)offsets[i] == 
            get_var_field(offsetsShrunk.get(), o_pos, o_pos + o_bits - 1));
        myAssert(o_pos < offset_bits_total);
            
        o_pos += o_bits;
    }
    
    #ifdef DEBUG
    o_pos = 0;
    for (size_type i = 0; i < numBlocks; i++)
    {
        size_type c = get_field(classes.get(), MAX_BITS_PER_CLASS, i);
        size_type o_bits = countCube.getNumOffsetBits(c);
        myAssert((unsigned long)offsets[i] == 
            get_var_field(offsetsShrunk.get(), o_pos, o_pos + o_bits - 1));
        o_pos += o_bits;
    }
    #endif
    
    return RRRSequence(classesShrunk, offsetsShrunk, offsets.size(),
        ARITY, BLOCK_SIZE, SUPER_BLOCK_FACTOR, bitsPerClass, offsetUints,
        offset_bits_total, countCube);
}

RRRSequence::RRRSequence(const boost::shared_array<uint> & classes_in,
    const boost::shared_array<uint> & offsets_in,
    const size_type NUM_BLOCKS,
    const size_type arity,
    const size_type blocksize, const size_type s_block_factor,
    const size_type bitsPerClass,
    const size_type TOTAL_OFFSET_UINTS,
    const size_type TOTAL_OFFSET_BITS, const CountCube & cc) :
    // these will have to be constructed in smarter ways :)
    // like, store a number to say how many bit are required for the classes?
    // and packing the offsets
    _size(0), classes(classes_in), offsets(offsets_in),
    BITS_PER_CLASS(bitsPerClass), O_REF_BITS(getBitsRequired(TOTAL_OFFSET_BITS))
{
    // these really must be the same length...
    // myAssert(classes.size() == offsets.size());
    //const unsigned int NUM_BLOCKS(offsets.size());
    num_super_blocks = ceil(NUM_BLOCKS / (float) s_block_factor);
    //TRACE(( "[RRRSequence.CTOR] Num Super Blocks: %d\n", num_super_blocks ));
    
    // Y = sym (arity)
    // X = super block (num_super_blocks)
    size_type inter_bits = getBitsRequired(
        s_block_factor * blocksize * num_super_blocks);
    size_type inter_uints = getUintsRequired(inter_bits, 
        arity * (num_super_blocks - 1));
    intermediates = inter_t(new uint[inter_uints]);
    
    // O_SAMPLES: fixed-width array for pre-computed positions for offsets
    // at super block boundary
    // how many bits required to point to every possible offset
    // in the offset array?
    size_type num_o_samples = (num_super_blocks - 1); // dont need for SB0
    size_type num_o_uints = getUintsRequired(O_REF_BITS, num_o_samples);
    o_samples = inter_t(new uint[num_o_uints]);
    
    _size += num_o_uints * sizeof(uint);
    _size += inter_uints * sizeof(uint);
    _size += NUM_BLOCKS * ceil(BITS_PER_CLASS / sizeof(uint));
    _size += TOTAL_OFFSET_UINTS * sizeof(uint);
    
    // declare running total vector
    vector<int> totals(arity, 0);
    // for each (C, O) at i:
    size_type offset_pos = 0;
    for (size_type i = 0; i < NUM_BLOCKS; i++)
    {
        // superblock i/s_block_factor
        size_type super_block_idx = i / s_block_factor;
        size_type block_idx  = i % s_block_factor;
        const size_type classNum = get_field(classes.get(), BITS_PER_CLASS, i);
        const size_type offset_bits = cc.getNumOffsetBits(classNum);
        const size_type offset = get_var_field(offsets.get(), offset_pos,
            offset_pos + offset_bits - 1);
        
        
        // Super block boundary:
        if (i > 0 && block_idx == 0)
        {
            // update offset samples
            myAssert(super_block_idx > 0);
            set_field(o_samples.get(), O_REF_BITS,
                super_block_idx - 1, offset_pos);
            
            // update intermediate count
            // Y = sym (arity)
            // X = super block (num_super_blocks)
            for (size_type sym = 0; sym < arity; sym++)
            {
                myAssert(super_block_idx > 0);
                size_type intermediate_idx = get3DIdx(num_super_blocks,
                    arity, super_block_idx - 1, sym, 0);
                set_field(intermediates.get(), inter_bits, intermediate_idx,
                    totals[sym]);
            }
        }
        
        offset_pos += offset_bits;
        
        // get last value in block, for each symbol
        for (size_type sym = 0; sym < arity; sym++)
        {
            size_type rank = cc.rank(classNum, offset, sym, 
                blocksize - 1);
            
            // update running totals for super-block
            totals[sym] += rank;
        }
    }
}

size_type RRRSequence::rank(symbol_t sym, size_type pos, size_type blocksize,
    size_type s_block_factor,
    const CountCube & cc) const
{
    size_type sym_idx = pos % blocksize;
    size_type global_block_idx = pos / blocksize;
    size_type super_block_idx = global_block_idx / s_block_factor;
    size_type block_idx = global_block_idx % s_block_factor;
    size_type inter_bits = getBitsRequired(
        s_block_factor * blocksize * num_super_blocks);
    
    size_type count = 0;
    
    // get count from previous super blocks
    if (super_block_idx > 0)
    {
        size_type inter_idx = get3DIdx(num_super_blocks,
            arity, super_block_idx - 1, sym, 0);
        count += get_field(intermediates.get(), inter_bits, inter_idx);
    }

    size_type o_pos = 0;
    size_type o_size = 0;
    
    // get previous block counts within this superblock
    for (size_type i = 0; i < block_idx && i < s_block_factor; i++)
    {
        size_type block_diff = block_idx - i;
        size_type g_pre_block_i = global_block_idx - block_diff;
        size_type classNum = get_field(classes.get(), BITS_PER_CLASS,
            g_pre_block_idx);
        
    }
    
    size_type classNum = get_field(classes.get(), BITS_PER_CLASS,
        global_block_idx);
    
    size_type o_pos = 0;
    if (super_block_idx > 0)
    {
        o_pos = get_field(o_samples.get(), O_REF_BITS, super_block_idx - 1);
    }
    size_type o_size = 0;

    // sample offsets
    size_type first_block = super_block_idx * s_block_factor;
    for (size_type i = first_block; i <= global_block_idx; i++)
    {
        size_type c = get_field(classes.get(), BITS_PER_CLASS, i);
        o_size = cc.getNumOffsetBits(c);
        if ( i < global_block_idx )
            o_pos += o_size;
    }
    
    size_type offset = get_var_field(offsets.get(), o_pos,
        o_pos + o_size - 1);
    size_type cc_count = cc.rank(classNum, offset, sym, sym_idx);
    //TRACE(("FROM CC: %d\n", cc_count));
    // look up current block pos using CountCube
    count += cc_count;
    
    //TRACE(("[RRRSequence.rank] Count: %d\n", count));
    return count;
}