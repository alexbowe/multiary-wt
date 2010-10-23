#ifndef COUNTENTRY_H
#define COUNTENTRY_H

#include <vector>

#include "boost/shared_array.hpp"
#include "utility.h"
#include "common.h"

using namespace std;

namespace indexes
{

class CountEntry
{
private:
    boost::shared_array<uint> counts;
    size_type _size;
public:
    CountEntry(const sequence_t & block, size_type arity);
    inline size_type rank(symbol_t sym, size_type pos, size_type blocksize)
    {
        const size_type FIELDSIZE(getBitsRequired(blocksize));
        int counts_index = sym * blocksize + pos;
        return get_field(counts.get(), FIELDSIZE, counts_index);
    }
    inline size_type size()
    {
        return _size;
    }
};

} // end of namespace

#endif // end of include guard
