#ifndef WTFUNCTIONAL_H
#define WTFUNCTIONAL_H

#include <algorithm>
#include "common.h"

using namespace std;

namespace indexes
{    

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
    enc_sequence_iterator result = lower_bound(ALPHABET.begin(),
        ALPHABET.end(), input);
    size_type index = result - ALPHABET.begin();
    myAssert(ALPHABET[index] == input);
    
    return (index * ARITY) / SIGMA;
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
