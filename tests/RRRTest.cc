#include "CppUnitLite/TestHarness.h"
#include "indexes/RRR.h"

#include "indexes/debug.h"

using namespace std;
using namespace indexes;

TEST( RRR, popcount )
{
    unsigned int arity = 4;
    unsigned int blocksize = 5;
    unsigned int superblocksize = 2;
    
    RRR rrr(arity, blocksize, superblocksize);
    
    #define N 14
    
    // not a perfect multiple :) so there will be 0-padding in the final block
                           //|              |              |              |
    symbol_t values[N]    =  { 0, 1, 2, 0, 3, 0, 1, 2, 0, 1, 3, 3, 2, 1,  };
    size_type counts[][N] ={ { 1, 1, 1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 4, 4,  },//0
                             { 0, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 4,  },//1
                             { 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3,  },//2
                       /*3*/ { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 3, 3, 3,  } };
    sequence_t v(values, N);
    
    TRACE(("Building\n"));
    RRRSequence rrrseq = rrr.build(v);
    TRACE(("Finished\n"));
    
    for (symbol_t sym = 0; sym < static_cast<symbol_t>(arity); sym++)
    {
        for (size_type pos = 0; pos < N; pos++)
        {
            size_type result = rrr.rank(sym, pos, rrrseq);
            size_type expected = counts[sym][pos];
            TRACE(("***[RRRTest] rank(%d, %d) -> %d (expected: %d)\n",
                sym, pos, result, expected));
            CHECK(result == expected);
        }
    }
}
