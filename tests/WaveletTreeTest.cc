#include "CppUnitLite/TestHarness.h"
#include "indexes/WaveletTree.h"
#include <vector>
#include <map>
#include "TestHelpers.h"
#include "WTTestHelpers.h"

using namespace std;

// TESTS BEGIN BEBE

TEST( WaveletTree, charRank )
{
    basic_string<char> input = "this is a test string!";
    CHECK( doCountTest(input, 4, 5, 2) );
}

TEST( WaveletTree, intRank )
{
    int ints[] = {1, 1, 3, 1, 0, 2, 4};
    basic_string<int> input(ints, ARRAY_SIZE(ints, int));
    CHECK( doCountTest(input, 4, 5, 2) );
}

TEST( WaveletTree, charRankHardcore )
{
    basic_string<char> input = "this is a test string! It's pretty long so "
        " I hope it works. Anyway, how was your day? Maybe I should make a"
        " test text generator too, because this is nothing compared to the"
        " actual documents I want to use this on. Oh well, here goes..."
        " Yay! It passed... now fingers crossed for the experiments! :)" ;
    CHECK( doCountTest(input, 4, 5, 2) );
}