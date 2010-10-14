#include "CppUnitLite/TestHarness.h"
#include "indexes/WaveletTree.h"
#include <vector>
#include <map>

using namespace std;
using namespace indexes;

typedef vector<int> CountVect;
typedef map<char, CountVect > CharCounters;
typedef map<int, CountVect > IntCounters;

TEST( WaveletTree, charRank )
{
    string input = "this is a test string!";
    string alphabet = getAlphabet(input);
    
    CharCounters counters;
    
    // for each character
    for (unsigned int i = 0; i < alphabet.length(); i++)
    {
        unsigned int count = 0;
        char sym = alphabet[i];
        counters[sym] = CountVect(input.length(), 0);
        // in each position
        for (int j = 0; j < input.length(); j++)
        {
            if (input[j] == sym) count++;
            counters[sym][j] = count;
        }
    }
    
    WaveletTree<char> wt(input, 4, 5, 2);
    
    // for each character
    for (unsigned int i = 0; i < alphabet.length(); i++)
    {
        unsigned int count = 0;
        char sym = alphabet[i];
        counters[sym] = CountVect();
        // in each position
        for (int j = 0; j < input.length(); j++)
        {
            size_type result = wt.rank(sym, j);
            size_type expected = counters[sym][j];
            TRACE(("***[WTTest] rank('%c', %d) -> %d (expected: %d)\n",
                sym, j, result, expected));
            CHECK(result == expected);
        }
    }
}

/*
TEST( WaveletTree, intRank )
{
    int ints[] = {0, 1, 3, 1, 0, 2, 4};
    basic_string<int> v(ints, 7);
    WaveletTree<int> wt2(v, 4, 5, 2);
}*/
