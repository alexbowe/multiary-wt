#include "CppUnitLite/TestHarness.h"
#include "indexes/CountEntry.h"
#include "indexes/debug.h"
#include <vector>

using namespace std;
using namespace indexes;

TEST( CountEntry, rank )
{
    unsigned int arity = 4;
    unsigned int blocksize = 5;
    
    //RRR::symbol_t values[] = { 0, 1, 2, 0, 3, 0, 1, 2, 0, 1, 1 };
    //vector<RRR::symbol_t> v(values, values +
        //sizeof(values)/sizeof(RRR::symbol_t));
    int ints[] = {1, 1, 1, 1, 1};
    sequence_t v = sequence_t(ints, blocksize);
    
    CountEntry ce(v, arity);

    unsigned int i = 0;
    for (i = 0; i < blocksize; i++)
    {
        CHECK(ce.rank(0, i, blocksize) == 0);
        CHECK(ce.rank(1, i, blocksize) == i+1);
        CHECK(ce.rank(2, i, blocksize) == 0);
        CHECK(ce.rank(3, i, blocksize) == 0);
    }
}
