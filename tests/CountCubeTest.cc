#include "CppUnitLite/TestHarness.h"
#include "indexes/CountCube.h"

using namespace std;
using namespace indexes;

TEST( CountCube, rank )
{
    size_type arity = 4;
    size_type blocksize = 5;
    
    int ints0[] = {0, 1, 2, 3, 0};
    sequence_t b_0(ints0, blocksize);
    int ints1[] = {1, 1, 2, 3, 0}; // diff class
    sequence_t b_1(ints1, blocksize);
    int ints2[] = {1, 0, 2, 3, 0}; // same class as ints0
    sequence_t b_2(ints2, blocksize);
    int ints3[] = {1, 0, 2, 3, 1}; // same class as ints1
    sequence_t b_3(ints3, blocksize);
    
    CountCube cc(arity, blocksize);
    size_type classNum = 5;
    size_type offset = 7;
    cc.add(b_0, classNum, offset);
    CHECK(classNum == 0 && offset == 0);
    cc.add(b_1, classNum, offset);
    CHECK(classNum == 1 && offset == 0);
    cc.add(b_2, classNum, offset);
    CHECK(classNum == 0 && offset == 1);
    cc.add(b_3, classNum, offset);
    CHECK(classNum == 1 && offset == 1);
    
    // Need to seal the deal...
    //cc.seal();
    //CHECK(cc.add(b_0, classNum, offset) == false);
}
