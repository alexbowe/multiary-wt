#include "CppUnitLite/TestHarness.h"
#include "indexes/MultiSDWaveletTree.h"
#include <vector>
#include <map>
#include "TestHelpers.h"
#include "MultiSDWTTestHelpers.h"

using namespace std;

TEST( MultiSDWaveletTree, charRank )
{
    basic_string<char> input = "this isnt fun";
    
    basic_string<char> alphabet = getAlphabet(input);
    map<char, CountVect> counters = prepareCountMap(input, alphabet);
    
    for (int arity = 2; arity <= 8; arity++)
        CHECK(MSD::doCountTest(input, counters, arity));
}

TEST( MultiSDWaveletTree, intRank )
{
    int ints[] = {1, 1, 3, 1, 0, 2, 4};
    basic_string<int> input(ints, ARRAY_SIZE(ints, int));
    
    basic_string<int> alphabet = getAlphabet(input);
    map<int, CountVect> counters = prepareCountMap(input, alphabet);
    
    for (int arity = 2; arity <= 8; arity*=2)
        CHECK(MSD::doCountTest(input, counters, arity));
}
