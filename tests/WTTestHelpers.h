#ifndef WTTESTHELPERS_H
#define WTTESTHELPERS_H

#include <vector>
#include <map>
#include "indexes/WaveletTree.h"

using namespace std;
using namespace indexes;

typedef vector<int> CountVect;

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

template <class T>
bool testCounts(const WaveletTree<T> & wt, map<T, CountVect> counters,
    const basic_string<T> & alphabet, int seq_length, bool isChar)
{
    for (unsigned int i = 0; i < alphabet.length(); i++)
    {
        T sym = alphabet[i];
        // in each position
        for (int j = 0; j < seq_length; j++)
        {
            size_type result = wt.rank(sym, j);
            size_type expected = counters[sym][j];
            if (isChar)
            {
                TRACE(("***[WTTest] rank('%c', %d) -> %d (expected: %d)\n",
                    sym, j, result, expected));
            }
            else
            {
                TRACE(("***[WTTest] rank('%d', %d) -> %d (expected: %d)\n",
                    sym, j, result, expected));
            }
            if (result != expected)
                return false;
        }
    }
    return true;
}

bool testCounts(const WaveletTree<int> & wt, const map<int,
    CountVect> & counters, const basic_string<int> & alphabet,
    int seq_length)
{
    return testCounts(wt, counters, alphabet, seq_length, false);
}

bool testCounts(const WaveletTree<char> & wt,
    const map<char, CountVect> & counters, const basic_string<char> & alphabet,
    int seq_length)
{
    return testCounts(wt, counters, alphabet, seq_length, true);
}

template <class T>
bool doCountTest(const basic_string<T> & input)
{
    basic_string<T> alphabet = getAlphabet(input);
    
    map<T, CountVect> counters = prepareCountMap(input, alphabet);
    
    WaveletTree<T> wt(input, 4, 5, 2);
    
    return testCounts(wt, counters, alphabet, input.length());
}

#endif