#include "CppUnitLite/TestHarness.h"
#include "indexes/WaveletTree.h"

using namespace indexes;

TEST( WaveletTree, charRank )
{
    // need to write the test
    WaveletTree<char> wt("cdfadsfabffazdsaxsafsa", 4, 5, 2);
    CHECK(false);
}

TEST( WaveletTree, intRank )
{
    int ints[] = {0, 1, 4, 3, 1, 0, 2};
    basic_string<int> v(ints, 7);
    WaveletTree<int> wt2(v, 4, 5, 2);
    CHECK(false);
}
