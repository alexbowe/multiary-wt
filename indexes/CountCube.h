#ifndef COUNTCUBE_H
#define COUNTCUBE_H

#include <memory>
#include <vector>
#include "boost/shared_ptr.hpp"
#include "common.h"
#include "CountEntry.h"
#include "IndexMapper.h"

using namespace std;
using namespace boost;

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
    
    typedef IndexMapper<sequence_t> Mapper;
    // maybe should pass these in so don't need to seal() it after...
    // inside a { } block so their destructors are called...
    // not a good idea since it makes calling it from the client 
    // really hard...
    vector< shared_ptr<Mapper> > blockMappers;
    auto_ptr<Mapper> classMapper;
    
public:
    CountCube(size_type arity, size_type blocksize);
    void seal(); // delete mappers
    bool add(const sequence_t block, size_type & classNum,
        size_type & offset);
    size_type rank(size_type classNum, size_type offset, symbol_t symbol,
        size_type position);
};

} // end of namespace

#endif // end of include guard
