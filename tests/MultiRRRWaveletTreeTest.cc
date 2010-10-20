#include "CppUnitLite/TestHarness.h"
#include "indexes/MultiRRRWaveletTree.h"
#include <vector>
#include <map>
#include "TestHelpers.h"
#include "MultiRRRWTTestHelpers.h"

using namespace std;

TEST( MultiRRRWaveletTree, charRank )
{
    basic_string<char> input = "this isnt fun";
    
    basic_string<char> alphabet = getAlphabet(input);
    map<char, CountVect> counters = prepareCountMap(input, alphabet);
    
    for (int arity = 2; arity <= 8; arity++)
        CHECK(MRRR::doCountTest(input, counters, arity));
}

TEST( MultiRRRWaveletTree, intRank )
{
    int ints[] = {1, 1, 3, 1, 0, 2, 4};
    basic_string<int> input(ints, ARRAY_SIZE(ints, int));
    
    basic_string<int> alphabet = getAlphabet(input);
    map<int, CountVect> counters = prepareCountMap(input, alphabet);
    
    for (int arity = 2; arity <= 8; arity*=2)
        CHECK(MRRR::doCountTest(input, counters, arity));
}
