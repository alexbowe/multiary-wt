#ifndef INDEXMAPPER_H
#define INDEXMAPPER_H

#include <map>
#include "common.h"

namespace indexes
{
    template <class T>
    class IndexMapper
    {
    private:
        size_type count;
        map<T, size_type> hashes;
    public:
        IndexMapper() : count(0), hashes() {}
        size_type operator ()(T item)
        {
            if (hashes.count(item) > 0)
            {
                return hashes[item];
            }
            hashes[item] = count++;
            return hashes[item];
        }
    };
}

#endif