#include "CppUnitLite/TestHarness.h"
#include "indexes/RRR.h"

#include "indexes/debug.h"
#include "TestHelpers.h"

using namespace std;
using namespace indexes;

TEST( RRR, popcount )
{
    unsigned int arity = 4;
    unsigned int blocksize = 5;
    unsigned int superblocksize = 2;
    
    RRR rrr(arity, blocksize, superblocksize);
    
    // not a perfect multiple :) so there will be 0-padding in the final block
                        //|              |              |              |
    symbol_t values[] =  { 0, 1, 2, 0, 3, 0, 1, 2, 0, 1, 3, 3, 2, 1,  };
    sequence_t input(values, ARRAY_SIZE(values, int));
    sequence_t alphabet = getAlphabet(input);
    map<int, CountVect> counts = prepareCountMap(input, alphabet);
    
    RRRSequence rrrseq = rrr.build(input);
    
    for (symbol_t sym = 0; sym < static_cast<symbol_t>(arity); sym++)
    {
        for (size_type pos = 0; pos < input.length(); pos++)
        {
            size_type result = rrr.rank(sym, pos, rrrseq);
            size_type expected = counts[sym][pos];
            #if DEBUG
            if (result != expected)
                TRACE(("***[RRRTest] rank(%d, %d) -> %d (expected: %d)\n",
                    sym, pos, result, expected));
            #endif
            CHECK(result == expected);
        }
    }
}

TEST( RRR, popcount2 )
{
    unsigned int arity = 4;
    unsigned int blocksize = 5;
    unsigned int superblocksize = 2;
    
    RRR rrr(arity, blocksize, superblocksize);
    
    symbol_t values[] =  {  // sb0
                            3, 1, 2, 3, 0,
                            2, 3, 0, 0, 0, // sb1
                            3, 1, 3, 3, 0, 
                            3, 3, 2, 2, 2, // sb2
                            1, 0
                         };
    sequence_t input(values, ARRAY_SIZE(values, int));
    sequence_t alphabet = getAlphabet(input);
    map<int, CountVect> counts = prepareCountMap(input, alphabet);
    
    RRRSequence rrrseq = rrr.build(input);
    
    for (size_type pos = 0; pos < input.length(); pos++)
    {
        for (symbol_t sym = 0; sym < static_cast<symbol_t>(arity); sym++)
        {
            size_type result = rrr.rank(sym, pos, rrrseq);
            size_type expected = counts[sym][pos];
            #if DEBUG
            if (result != expected)
                TRACE(("***[RRRTest] rank(%d, %d) -> %d (expected: %d)\n",
                    sym, pos, result, expected));
            #endif
            CHECK(result == expected);
        }
    }
}