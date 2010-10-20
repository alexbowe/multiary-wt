#ifndef M_RRR_WTTESTHELPERS_H
#define M_RRR_WTTESTHELPERS_H

#include <vector>
#include <map>
#include "TestHelpers.h"
#include "indexes/MultiRRRWaveletTree.h"

using namespace std;
using namespace indexes;

namespace MRRR
{
    template <class T>
    bool testCounts(const MultiRRRWaveletTree<T> & wt,
        map<T, CountVect> counters,
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
                if (result != expected)
                {
                    if (isChar)
                    {
                        TRACE(("***[MRRRWTTest] rank('%c', %d) "
                               "-> %d (expected: %d)\n",
                               sym, j, result, expected));
                    }
                    else
                    {
                        TRACE(("***[MRRRWTTest] rank('%d', %d) "
                               "-> %d (expected: %d)\n",
                               sym, j, result, expected));
                    }
                }
                if (result != expected)
                    return false;
            }
        }
        return true;
    }

    bool testCounts(const MultiRRRWaveletTree<int> & wt, const map<int,
        CountVect> & counters, const basic_string<int> & alphabet,
        int seq_length)
    {
        return testCounts(wt, counters, alphabet, seq_length, false);
    }

    bool testCounts(const MultiRRRWaveletTree<char> & wt,
        const map<char, CountVect> & counters, const basic_string<char> & alphabet,
        int seq_length)
    {
        return testCounts(wt, counters, alphabet, seq_length, true);
    }

    template <class T>
    bool doCountTest(const basic_string<T> & input, map<T, CountVect> & counters,
        size_type arity)
    {
        basic_string<T> alphabet = getAlphabet(input);

        MultiRRRWaveletTree<T> wt(input, arity);

        return testCounts(wt, counters, alphabet, input.length());
    }
}

#endif