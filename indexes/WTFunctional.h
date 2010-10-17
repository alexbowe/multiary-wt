#ifndef WTFUNCTIONAL_H
#define WTFUNCTIONAL_H

#include <algorithm>
#include "common.h"

using namespace std;

namespace indexes
{    

template <class T>
inline size_type encode(T symbol, const basic_string<T> & alpha, size_type left,
    size_type right, size_type arity)
{
    //size_type low = 1;
    //size_type high = arity - 1;
    size_type sigma = right - left + 1;
    size_type step = (sigma + 1) / arity;
    
    // if alphabet is smaller than arity
    if (sigma <= arity)
    {
        for (size_type i = 0; i < arity; i++)
        {
            if (alpha[i] == symbol)
                return i;
        }
        // should only get here if we are encoding symbols that aren't in
        // the alphabet
        myAssert(false);
    }
    
    // this could be done with binary search for i
    // making this O(log(arity)), currently it is O(arity)
    // arity wont get very big though... so leave it as linear for now
    // was previously O(log(sigma))
    for (size_type i = 1; i < arity; i++)
    {
        if (symbol < alpha[i * step])
            return i - 1;
    }
    
    // last segment
    return arity - 1;
}

/** Function object used for encoding symbols */
template <class T>
class SymbolEncoder
{
    typedef basic_string<T> enc_sequence_t;
    typedef typename enc_sequence_t::const_iterator enc_sequence_iterator;
    const basic_string<T> & ALPHABET;
    const size_type ARITY;
    const size_type SIGMA;
    
public:
    SymbolEncoder(const enc_sequence_t & alphabet, size_type arity);
    symbol_t operator()(T input) const;
};

template <class T>
SymbolEncoder<T>::SymbolEncoder(const enc_sequence_t & alphabet,
    size_type arity): ALPHABET(alphabet), ARITY(arity), SIGMA(alphabet.length())
{ }

template <class T>
symbol_t SymbolEncoder<T>::operator()(T input) const
{
    //enc_sequence_iterator result = lower_bound(ALPHABET.begin(),
    //    ALPHABET.end(), input);
    //size_type index = result - ALPHABET.begin();
    //myAssert(ALPHABET[index] == input);
    
    return encode(input, ALPHABET, 0, ALPHABET.size() - 1, ARITY);
}

/** Filters symbols based on expected encoding value */
template <class T>
class SymbolFilter
{
private:
    typedef SymbolEncoder<T> Encoder;
    const Encoder & ENC;
    const symbol_t VAL;
public:
    SymbolFilter(const Encoder & enc, size_type value);
    inline bool operator()(T input) const
    {   return ENC(input) == VAL;   }
};

template<class T>
SymbolFilter<T>::SymbolFilter(const Encoder & enc, size_type value) :
    ENC(enc), VAL(static_cast<symbol_t>(value)) { }


//FREE FUNCTIONS

template < class outT, class inT, class Functor >
inline basic_string<outT> map_func(const Functor & f,
    const basic_string<inT> & input)
{
    basic_string<outT> output;
    for (size_type i = 0; i < input.length(); i++)
    {
        output += f(input[i]);
    }
    return output;
}

template < class T, class Functor >
inline basic_string<T> filter_func(const Functor & f,
    const basic_string<T> & input)
{
    basic_string<T> output;
    for (size_type i = 0; i < input.length(); i++)
    {
        if ( f(input[i]) == true )
            output += input[i];
    }
    return output;
}

}

#endif
