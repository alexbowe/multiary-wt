#include "CppUnitLite/TestHarness.h"
#include "indexes/WaveletTree.h"
#include <vector>
#include <map>
#include "WTTestHelpers.h"

using namespace std;

// TESTS BEGIN BEBE

TEST( WaveletTree, charRank )
{
    basic_string<char> input = "this is a test string!";
    CHECK( doCountTest(input) );
}

TEST( WaveletTree, intRank )
{
    int ints[] = {1, 1, 3, 1, 0, 2, 4};
    basic_string<int> input(ints, 7);
    CHECK( doCountTest(input) );
}
