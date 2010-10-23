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
    BITS_PER_CLASS(bitsPerClass)
{
    // these really must be the same length...
    // myAssert(classes.size() == offsets.size());
    //const unsigned int NUM_BLOCKS(offsets.size());
    num_super_blocks = ceil(NUM_BLOCKS / (float) s_block_factor);
    //TRACE(( "[RRRSequence.CTOR] Num Super Blocks: %d\n", num_super_blocks ));
    
    // Z = sym (arity)
    // Y = super block (num_super_blocks)
    // X = block (s_block_factor) - arranged this way for caching
    // TODO: COMPRESS THIS and add it to size count
    size_type inter_bits = getBitsRequired(s_block_factor * blocksize);
    size_type inter_uints = getUintsRequired(inter_bits,
        arity * num_super_blocks * s_block_factor);
    intermediates = inter_t(new uint[inter_uints]);
    
    // how many bits required to point to every possible offset
    // in the offset array?
    size_type O_REF_BITS(getBitsRequired(TOTAL_OFFSET_BITS));
    size_type num_o_samples = (num_super_blocks - 1);
    size_type num_o_uints = getUintsRequired(O_REF_BITS, num_o_samples);
    
    _size += inter_uints * sizeof(int);
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
        offset_pos += offset_bits;
        
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
            /** Retrieve a given index from array A where every value uses len bits
             * @param A Array
             * @param len Length in bits of each field
             * @param index Position to be retrieved
             */
            set_field(intermediates.get(), inter_bits, intermediate_idx,
            totals[sym]);
            //intermediates[intermediate_idx] = totals[sym];
            //TRACE(("S%d b%d %d: %d\n", super_block_idx, block_idx, sym,
            //    totals[sym]));
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
    size_type inter_bits = getBitsRequired(s_block_factor * blocksize);
    
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
        count += get_field(intermediates.get(), inter_bits, inter_idx);
    }
    //TRACE(("Count(1): %d\n", count));

    // look up prev block pos
    if (block_idx > 0)
    {
        //TRACE(("DOING SOMETHING DANGEROUS!\n"));
        inter_idx = get3DIdx(s_block_factor, num_super_blocks,
            block_idx - 1, super_block_idx, sym);
        count += get_field(intermediates.get(), inter_bits, inter_idx);
    }
    //TRACE(("Count(2): %d\n", count));
    
    size_type classNum = get_field(classes.get(), BITS_PER_CLASS,
        global_block_idx);
    
    size_type o_pos = 0;
    size_type o_size = 0;
    // sample offsets
    for (size_type i = 0; i <= global_block_idx; i++)
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