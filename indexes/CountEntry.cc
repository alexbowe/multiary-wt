#include "CountEntry.h"
#include <cstring>
#include "utility.h"
#include "debug.h"

using namespace std;
using namespace indexes;

CountEntry::CountEntry(const sequence_t & block, size_type arity)
{
    const size_type BLOCKSIZE(block.size());
    const size_type FIELDSIZE(getBitsRequired(BLOCKSIZE));
    const size_type NUM_INTS(getUintsRequired(arity * BLOCKSIZE, FIELDSIZE));
    
    counts = boost::shared_array<uint>(new uint[NUM_INTS]);
    
    // cumulative sum for each symbol
    uint count_index = 0;
    // for each position
    for ( size_type pos = 0; pos < BLOCKSIZE; pos++ )
    {
        symbol_t block_symbol = block[pos];
        
        // for each symbol
        for ( symbol_t sym = 0; sym < static_cast<symbol_t>(arity); sym++)
        {
            // this is the index in the 1D version of the array
            count_index = sym * BLOCKSIZE + pos;
            if (pos == 0)
            {
                //default to 0
                set_field(counts.get(), FIELDSIZE, count_index, 0);
            }
            if (pos > 0)
            {
                // Carry count across
                uint prev = get_field(counts.get(), FIELDSIZE, count_index - 1);
                set_field(counts.get(), FIELDSIZE, count_index, prev);
            }
            // update relevant symbol count
            if (block_symbol == sym)
            {
                uint temp = get_field(counts.get(), FIELDSIZE, count_index);
                set_field(counts.get(), FIELDSIZE, count_index, temp + 1);
            }
        }
    }
}

