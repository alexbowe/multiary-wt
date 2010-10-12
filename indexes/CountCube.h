#ifndef COUNTCUBE_H
#define COUNTCUBE_H

#include <vector>
#include "common.h"
#include "CountEntry.h"
#include "IndexMapper.h"

using namespace std;

namespace indexes
{

class CountCube
{
private:
    const size_type ARITY;
    const size_type BLOCK_SIZE;
    
    // Dimensions: [Class][Offset]([Symbol][Position])
    typedef vector<CountEntry> count_table_t;
    vector<count_table_t> class_table;
    
    
    IndexMapper< sequence_t > mapClass;
    IndexMapper< sequence_t > mapBlock;
    
public:
    CountCube(size_type arity, size_type blocksize);
    bool add(const sequence_t & block, size_type & classNum,
        size_type & offset);
    // void seal(); // delete mappers
};

} // end of namespace

#endif // end of include guard
