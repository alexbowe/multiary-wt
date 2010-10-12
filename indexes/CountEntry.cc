#include "CountEntry.h"

using namespace std;
using namespace indexes;

CountEntry::CountEntry(const sequence_t & block, size_type arity)
{
    size_type blocksize = block.size();
    
    counts = boost::shared_array<size_type>(new size_type[arity * blocksize]);
    
    // cumulative sum for each symbol
    size_type count_index = 0;
    for ( size_type pos = 0; pos < blocksize; pos++ )
    {
        symbol_t block_symbol = block[pos];
        
        for ( symbol_t sym = 0; sym < arity; sym++)
        {
            count_index = sym * blocksize + pos;
            if (pos > 0)
                // move across
                counts[count_index] = counts[count_index - 1];
            // update relevant symbol count
            if (block_symbol == sym)
                counts[count_index]++;
        }
    }
}
