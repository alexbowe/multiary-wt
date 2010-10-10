#ifndef COUNTCUBE_H
#define COUNTCUBE_H

#include <vector>
#include "boost/multi_array.hpp"

using namespace std;

namespace indexes
{

// Mappers for offset and classes
// Dimensions: [Class][Offset][Symbol][Position]
class CountCube
{
private:
    // array_type::extent_gen extents;
    // array_type A(extents[3][3][3][3]);
    
    // vector<ptrs to offset tables> class_table;
    // offset table:
    //  an array of counts...
    //  for each symbol...
    //  for each position...
    //  Requires RESIZE along first axis... 
    //  use 3D boost multi-array: offset (resized), symbol, position
public:
    //CountCube();
    //bool add(const vector< symbol_t > & block, RRR::size_type & classNum,
    //    RRR::size_type & offset);
    // void seal(); // delete mappers
};

} // end of namespace

#endif // end of include guard
