#include "CountCube.h"

#include "debug.h"

using namespace std;
using namespace indexes;

CountCube::CountCube(size_type arity, size_type blocksize) : ARITY(arity),
    BLOCK_SIZE(blocksize), class_table()
{
    classMapper = Mapper_ptr(new Mapper());
    blockMappers = vector<Mapper_ptr>();
}

void CountCube::seal()
{
    classMapper.reset();
    blockMappers.clear();
    vector< shared_ptr<Mapper> >().swap(blockMappers);
}

bool CountCube::add(const sequence_t & block, size_type & classNum,
    size_type & offset)
{
    // have we sealed the countcube?
    if ( classMapper.get() == 0 )
        return false;
    
    sequence_t c = getClass(block, ARITY);
    
    classNum = (*classMapper)(c);
    //TRACE(("[CountCube.add] Class: %d\n", classNum));
    
    // If we don't currently have a blockMapper for this class, add one
    if ( classNum >= blockMappers.size())
        blockMappers.push_back(shared_ptr<Mapper>(new Mapper()));

    offset = (*blockMappers[classNum])(block);
    //TRACE(("[CountCube.add] Offset: %d\n", offset));
    
    // represent the block in the cube...
    // if the class isn't already represented in the class_table...
    if ( classNum >= class_table.size() )
        class_table.push_back(count_table_ptr(new count_table_t()));
    
    count_table_ptr & count_table = class_table[classNum];
    if ( offset >= count_table->size())
        count_table->push_back(CountEntry(block, ARITY));
    
    // by this point it's already in the table...
    
    return true;
}

size_type CountCube::rank(size_type classNum, size_type offset,
    symbol_t symbol, size_type position) const
{
    return class_table.at(classNum)->at(offset).rank(symbol, position,
        BLOCK_SIZE);
}
