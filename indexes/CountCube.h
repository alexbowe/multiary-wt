#ifndef COUNTCUBE_H
#define COUNTCUBE_H

#include <memory>
#include <vector>
#include "boost/shared_ptr.hpp"
#include "common.h"
#include "CountEntry.h"
#include "IndexMapper.h"

namespace indexes
{

class CountCube
{
private:
    const size_type ARITY;
    const size_type BLOCK_SIZE;
    
    size_type _size;
    size_type numClasses;
    size_type numOffsets;
    
    // Dimensions: [Class][Offset]([Symbol][Position])
    // maybe should use boost ptr_vector
    typedef std::vector<CountEntry> count_table_t;
    typedef boost::shared_ptr<count_table_t> count_table_ptr;
    std::vector<count_table_ptr> class_table;
    std::vector<uint> offset_size_table;
    
    typedef IndexMapper<sequence_t> Mapper;
    typedef boost::shared_ptr<Mapper> Mapper_ptr;
    // maybe should pass these in so don't need to seal() it after...
    // inside a { } block so their destructors are called...
    // not a good idea since it makes calling it from the client 
    // harder...
    std::vector< Mapper_ptr > blockMappers;
    Mapper_ptr classMapper;
    
public:
    CountCube(size_type arity, size_type blocksize);
    void add(const sequence_t & block, size_type & classNum,
        size_type & offset);
    inline void seal() // delete mappers
    {
        classMapper.reset();
        blockMappers.clear();
        std::vector< boost::shared_ptr<Mapper> >().swap(blockMappers);
    }
    inline size_type rank(size_type classNum, size_type offset,
        symbol_t symbol, size_type position) const
    {
        return (*class_table[classNum])[offset].rank(symbol, position,
            BLOCK_SIZE);
    }
    inline size_type size() const
    {
        return sizeof(*this) + _size;
    }
    inline size_type getNumOffsetBits(size_type classNum) const
    {
        return offset_size_table[classNum];
    }
    inline size_type getNumClasses()
    { return numClasses; }
    inline size_type getNumOffsets()
    { return numOffsets; }
};

inline size_type num_offsets(const sequence_t & classNums, size_type arity, size_type blocksize)
{
    size_type result = 1;
    size_type remaining = blocksize;
    
    for (size_type i = 0; i < arity; i++)
    {
        result *= binomial(remaining, classNums[i]);
        remaining -= classNums[i];
    }
    
    return result;
}

} // end of namespace

#endif // end of include guard
