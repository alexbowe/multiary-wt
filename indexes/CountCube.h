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
    
    // Dimensions: [Class][Offset]([Symbol][Position])
    // maybe should use boost ptr_vector
    typedef std::vector<CountEntry> count_table_t;
    typedef boost::shared_ptr<count_table_t> count_table_ptr;
    std::vector<count_table_ptr> class_table;
    
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
};

} // end of namespace

#endif // end of include guard
