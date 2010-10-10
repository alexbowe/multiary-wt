#include "CppUnitLite/TestHarness.h"
//#include "indexes/RRR.h"
#include <vector>

using namespace std;
//using namespace indexes;

TEST( RRR, popcount )
{
    int arity = 4;
    int blocksize = 5;
    int superblocksize = 3;
    //RRR rrr = RRR(arity, blocksize, superblocksize);
    
    int values[] = {0, 1, 2, 0, 3, 0, 1, 2, 0, 1, 1};
    
    //rrr.build();

    //for (unsigned int i = 0; i < length; i++)
    //{
    //    CHECK(wt.rank('a', i) == a_ranks[i]);
    //    CHECK(wt.rank('b', i) == b_ranks[i]);
    //    CHECK(wt.rank('c', i) == c_ranks[i]);
    //}
}
