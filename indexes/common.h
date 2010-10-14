#ifndef COMMON_H
#define COMMON_H

#include <string>

using namespace std;

namespace indexes
{
    typedef int symbol_t;
    typedef unsigned long size_type;
    typedef basic_string<symbol_t> sequence_t;
    
    inline sequence_t getClass(const sequence_t & seq, size_type arity)
    {
        sequence_t c(arity, 0);

        for (size_type i = 0; i < seq.size(); i++)
        {
            symbol_t sym = seq[i];
            c[sym]++;
        }

        return c;
    }
}

#endif