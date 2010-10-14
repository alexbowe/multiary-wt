#ifndef TESTHELPERS_H
#define TESTHELPERS_H

#include <vector>
#include <map>
#include "indexes/WaveletTree.h"

using namespace std;
using namespace indexes;

typedef vector<int> CountVect;

#define ARRAY_SIZE(a, type) (sizeof(a) / sizeof(type))

template <class T>
map<T, CountVect > prepareCountMap(const basic_string<T> & input,
    const basic_string<T> & alphabet)
{
    map<T, CountVect > counters;
    
    // Prepare Counts
    // for each character
    for (size_type i = 0; i < alphabet.length(); i++)
    {
        unsigned int count = 0;
        T sym = alphabet[i];
        counters.insert( pair<T, CountVect>(sym, CountVect(input.length(), 0)));
        // in each position
        for (unsigned int j = 0; j < input.length(); j++)
        {
            if (input[j] == sym) count++;
            counters[sym][j] = count;
        }
    }
    return counters;
}

#endif