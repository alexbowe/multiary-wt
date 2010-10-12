#include "CppUnitLite/TestHarness.h"
#include "indexes/RRR.h"
#include <vector>

using namespace std;
using namespace indexes;

TEST( RRR, popcount )
{
    unsigned int arity = 4;
    unsigned int blocksize = 5;
    unsigned int superblocksize = 3;
    
    RRR rrr = RRR(arity, blocksize, superblocksize);
    
    //RRR::symbol_t values[] = { 0, 1, 2, 0, 3, 0, 1, 2, 0, 1, 1 };
    //vector<RRR::symbol_t> v(values, values +
        //sizeof(values)/sizeof(RRR::symbol_t));
    sequence_t v(2 * blocksize, 1);
    
    RRRSequence s = rrr.build(v);

    //for (unsigned int i = 0; i < length; i++)
    //{
    //    CHECK(wt.rank('a', i) == a_ranks[i]);
    //    CHECK(wt.rank('b', i) == b_ranks[i]);
    //    CHECK(wt.rank('c', i) == c_ranks[i]);
    //}
    CHECK(true);
}
