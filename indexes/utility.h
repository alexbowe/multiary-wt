#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <set>
#include <cmath>
#include "common.h"

using namespace std;

namespace indexes
{
    /** Returns required index of 3D array */
    inline size_type get3DIdx(const size_type X, const size_type Y,
        const size_type x_i, const size_type y_i, const size_type z_i)
    {
        return z_i * Y * X + y_i * X + x_i;
    }
    
    /** Gets string of unique symbols */
    template <class T>
    basic_string<T> getAlphabet(const basic_string<T> & seq)
    {
        set<T> alphabet_set;
        basic_string<T> alphabet_string;

        size_type seq_length = seq.length();
        
        for (size_type i = 0; i < seq_length; i++)
        {
            alphabet_set.insert(seq[i]);
        }
        
        typedef typename set<T>::iterator set_iterator;
        set_iterator end = alphabet_set.end();
        
        for ( set_iterator it = alphabet_set.begin() ; it != end; it++)
            alphabet_string += *it;
        
        return alphabet_string;
    }
    
    /**
     * Returns the number of unique symbols (bits or otherwise) between
     * 0..arity-1 required to represent the specified number of elements.
     */
    inline size_t getNumSymbolsRequired(size_type numElements,
        size_type arity)
    {
        // log_arity ( numElements ) rounded up
        return ceil(log(numElements)/log(arity));
    }
    
    /** 
     * Gets number of tree nodes that would be in a balanced tree of this level
     * and arity
     */
    inline size_t getNumBalancedTreeNodes(size_type levels, size_type arity)
    {
        // Could lose precision? can't call with unsigned ints unfortunately
        return (pow(static_cast<int>(arity),
            static_cast<double>(levels)) - 1)/(arity - 1);
    }
}

#endif