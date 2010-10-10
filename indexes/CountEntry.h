#ifndef COUNTENTRY_H
#define COUNTENTRY_H

#include <vector>
#include "boost/shared_array.hpp"
#include "common.h"

using namespace std;

namespace indexes
{

class CountEntry
{
private:
    boost::shared_array<size_type> counts;
public:
    CountEntry(vector<symbol_t> & block, size_type arity);
    size_type rank(symbol_t sym, size_type pos, size_type blocksize,
        size_type arity);
};

} // end of namespace

#endif // end of include guard
