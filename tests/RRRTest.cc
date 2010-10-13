#include "CppUnitLite/TestHarness.h"
#include "indexes/RRR.h"

#include "indexes/debug.h"

using namespace std;
using namespace indexes;

TEST( RRR, popcount )
{
    unsigned int arity = 4;
    unsigned int blocksize = 5;
    unsigned int superblocksize = 3;
    
    RRR rrr(arity, blocksize, superblocksize);
    
    #define N 10
    
    symbol_t values[N]    =  { 0, 1, 2, 0, 3, 0, 1, 2, 0, 1 };
    size_type counts[][N] ={ { 1, 1, 1, 2, 2, 3, 3, 3, 4, 4 },
                             { 0, 1, 1, 1, 1, 1, 2, 2, 2, 3 },
                             { 0, 0, 1, 1, 1, 1, 1, 2, 2, 2 },
                             { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 } };
    sequence_t v(values, N);
    
    TRACE(("Building\n"));
    RRRSequence rrrseq = rrr.build(v);
    TRACE(("Finished\n"));
    
    for (symbol_t sym = 0; sym < static_cast<symbol_t>(arity); sym++)
    {
        for (size_type pos = 0; pos < N; pos++)
            CHECK(rrr.rank(sym, pos, rrrseq) == counts[sym][pos]);
    }
}
