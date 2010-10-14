#include "CountEntry.h"
#include <cstring>

using namespace std;
using namespace indexes;

CountEntry::CountEntry(const sequence_t & block, size_type arity)
{
    const size_type blocksize(block.size());
    
    counts = boost::shared_array<size_type>(new size_type[arity * blocksize]);
    
    for (size_type i = 0; i < blocksize * arity; i++)
        counts[i] = 0;
    
    // cumulative sum for each symbol
    size_type count_index = 0;
    for ( size_type pos = 0; pos < blocksize; pos++ )
    {
        symbol_t block_symbol = block[pos];
        
        for ( symbol_t sym = 0; sym < static_cast<symbol_t>(arity); sym++)
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

