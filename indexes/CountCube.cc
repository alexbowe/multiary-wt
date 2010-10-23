#include "CountCube.h"
#include "debug.h"

using namespace std;
using namespace boost;
using namespace indexes;

CountCube::CountCube(size_type arity, size_type blocksize) : ARITY(arity),
    BLOCK_SIZE(blocksize), _size(0), class_table(), offset_size_table()
{
    classMapper = Mapper_ptr(new Mapper());
    blockMappers = vector<Mapper_ptr>();
}

void CountCube::add(const sequence_t & block, size_type & classNum,
    size_type & offset)
{
    // have we sealed the countcube?
    // if ( classMapper.get() == 0 )
    //    return false;
    
    sequence_t c = getClass(block, ARITY);
    
    classNum = (*classMapper)(c);
    
    // If we don't currently have a blockMapper for this class, add one
    if ( classNum >= blockMappers.size())
    {
        //TRACE(("New Block Mapper\n"));
        blockMappers.push_back(boost::shared_ptr<Mapper>(new Mapper()));
        
        // also calculate the max offset size in bits
        // number of possible blocks for this class
        size_type nOffsets = num_offsets(c, ARITY, BLOCK_SIZE);
        size_type bits = getBitsRequired(nOffsets);
        offset_size_table.push_back(bits);
        _size += sizeof(bits);
    }

    offset = (*blockMappers[classNum])(block);
    #ifdef DEBUG
    if (offset >= num_offsets(c, ARITY, BLOCK_SIZE))
    {
        TRACE(("FAILURE:\n"));
        TRACE(("CLASS (%d): ", classNum));
        TRACE_SEQ((c));
        TRACE(("OFFSET:     %d\n", offset));
        TRACE(("MAX_OFFSET: %d\n", num_offsets(c, ARITY, BLOCK_SIZE)));
    }
    #endif
    myAssert(offset < num_offsets(c, ARITY, BLOCK_SIZE));
    //TRACE(("[CountCube.add] Offset: %d\n", offset));
    
    // represent the block in the cube...
    // if the class isn't already represented in the class_table...
    if ( classNum >= class_table.size() )
    {
        //TRACE(("New Count Table\n"));
        count_table_ptr ct(new count_table_t());
        class_table.push_back(ct);
        _size += sizeof(ct);
    }
    
    count_table_ptr & count_table = class_table[classNum];
    if ( offset >= count_table->size())
    {
        CountEntry ce = CountEntry(block, ARITY);
        count_table->push_back(ce);
        _size += ce.size();
    }
    
    // by this point it's already in the table...
}
