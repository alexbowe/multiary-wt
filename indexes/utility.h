#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <set>
#include <cmath>
#include "common.h"
#include "debug.h"

using namespace std;

namespace indexes
{
    typedef unsigned int uint;
    const static unsigned int W_LEN = sizeof(uint) * 8; // bits in an int
    
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
    inline size_type getNumSymbolsRequired(size_type numElements,
        size_type arity)
    {
        // log_arity ( numElements ) rounded up
        return ceil(log(numElements)/log(arity));
    }
    
    /** bits needed to represent a number between 0 and n */
    // NOTE: adapted from libcds
    inline size_type getBitsRequired(size_type n)
    {
        size_type b = 0;
        while ( n )
        {
            b++; n >>= 1;
        }
        return b;
    }
    
    /** uints required to represent num integers of bits each */
    inline size_type getUintsRequired(size_type bits, size_type num)
    {
        // + W_LEN - 1 to make it consider part-thereof
        return ((size_type) bits * num + W_LEN - 1) / W_LEN;
    }
    
    /** Retrieve a given index from array A where every value uses len bits
     * @param A Array
     * @param len Length in bits of each field
     * @param index Position to be retrieved
     */
    template < class Array_t >
    inline uint get_field(const Array_t A, const size_t len, const size_t index)
    {
        if ( len==0 ) return 0;
        
        size_t i = index * len / W_LEN;
        size_t j = index * len - W_LEN * i;
        
        uint result;
        if ( j + len <= W_LEN)
            result = (A[i] << (W_LEN - j - len)) >> (W_LEN - len);
        else {
            result = A[i] >> j;
            result = result | (A[i+1] << (W_LEN*2 - j - len)) >> (W_LEN - len);
        }
        return result;
    }

    /** Store a given value in index into array A where every value uses len
        bits
     * @param A Array
     * @param len Length in bits of each field
     * @param index Position to store in
     * @param x Value to be stored
     */
    template < class Array_t >
    inline void set_field(Array_t A, const size_t len,
        const size_t index, const uint x)
    {
        if ( len == 0) return;
        size_type i = index * len / W_LEN, j=index*len-i*W_LEN;
        uint mask = ((j+len) < W_LEN ? ~0u << (j+len) : 0)
            | ((W_LEN-j) < W_LEN ? ~0u >> (W_LEN-j) : 0);
        A[i] = (A[i] & mask) | x << j;
        if (j+len>W_LEN) {
            mask = ((~0u) << (len+j-W_LEN));
            A[i+1] = (A[i+1] & mask)| x >> (W_LEN-j);
        }
    }
    
    /** Retrieve a given bitsequence from array A
     * @param A Array
     * @param ini Starting position
     * @param fin Retrieve until end-1
     */
    template < class Array_t >
    inline uint get_var_field(const Array_t A, const size_t ini, const size_t fin) {
        if (ini == fin+1)
            return 0;
        
        // find starting int
        size_t i = ini / W_LEN;
        size_t j = ini - W_LEN * i;
        uint result;
        uint len = (uint)( fin - ini + 1 );
        
        if (j + len <= W_LEN)
        {
            result = (A[i] << (W_LEN - j - len)) >> (W_LEN - len);
        }
        else
        {
            result = A[i] >> j;
            result = result | (A[i+1] << (W_LEN*2-j-len)) >> (W_LEN-len);
        }
        return result;
    }

    /** Stores a given bitsequence into array A
     * @param A Array
     * @param ini Starting position
     * @param fin Store until end-1
     * @param x Value to be stored
     */
    template < class Array_t >
    inline void set_var_field(Array_t A, const size_t ini, const size_t fin,
        const uint x)
    {
        if (ini == fin + 1) return;
        
        uint i = ini/W_LEN, j = ini - i * W_LEN;
        uint len = (fin - ini + 1);
        uint mask = ((j+len) < W_LEN ? ~0u << (j+len) : 0)
            | ((W_LEN-j) < W_LEN ? ~0u >> (W_LEN-j) : 0);
        A[i] = (A[i] & mask) | x << j;
        if (j+len>W_LEN) {
            mask = ((~0u) << (len+j-W_LEN));
            A[i+1] = (A[i+1] & mask)| x >> (W_LEN-j);
        }
    }
    
    /** 
     * Gets number of tree nodes that would be in a balanced tree of this level
     * and arity
     */
    inline size_type getNumBalancedTreeNodes(size_type levels, size_type arity)
    {
        // Could lose precision? can't call with unsigned ints unfortunately
        return (pow(static_cast<int>(arity),
            static_cast<double>(levels)) - 1)/(arity - 1);
    }
    
    inline size_type getHeapChildIndex(size_type position, size_type child,
        size_type arity)
    {
        return arity * position + child;
    }
    
    /** reads bit p from e */
    inline size_t bitget(uint * e, size_t p)
    {
        return ((((e)[(p)/W_LEN] >> ((p)%W_LEN))) & 1);
    }

    /** sets bit p in e */
    inline void bitset(uint * e, size_t p) {
        e[p/W_LEN] |= (1<<(p%W_LEN));
    }

    /** cleans bit p in e */
    inline void bitclean(uint * e, size_t p) {
        e[p/W_LEN] &= ~(1<<(p%W_LEN));
    }
    
    inline size_type factorial(size_type n)
    {
        size_type answer = 1;
        for (size_type i = 0; i < n; i++)
            answer *= (n - i);
        return answer;
    }
    
    inline size_type binomial(size_type n, size_type k)
    {
        size_type numerator = n;

        if (0 == k) return 1;

        for (size_type i = 1; i < k; i++)
            numerator *= (n - i);

        return numerator/factorial(k);
    }
}

#endif