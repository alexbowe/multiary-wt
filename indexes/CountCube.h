#ifndef COUNTCUBE_H
#define COUNTCUBE_H

#include <vector>
#include "CountEntry.h"

using namespace std;

namespace indexes
{

class CountCube
{
private:
    // Dimensions: [Class][Offset]([Symbol][Position])
    typedef vector<CountEntry> count_table_t;
    vector<count_table_t> class_table;
    
    const size_type arity;
    const size_type blocksize;
public:
    CountCube();
    void add(const vector< symbol_t > & block, size_type & classNum,
        size_type & offset);
    // void seal(); // delete mappers
};

} // end of namespace

#endif // end of include guard
