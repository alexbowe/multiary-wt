#include "CountCube.h"

using namespace std;
using namespace indexes;

CountCube::CountCube(size_type arity, size_type blocksize) : ARITY(arity),
    BLOCK_SIZE(blocksize), class_table()
{
}
