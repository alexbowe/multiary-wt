#include "CppUnitLite/TestHarness.h"
#include "indexes/IndexMapper.h"
#include <iostream>

using namespace std;
using namespace indexes;

TEST( IndexMapper, map )
{
    int ints0[] = {0, 1, 3, 2};
    int ints1[] = {0, 1, 3, 0};
    
    sequence_t b_0(ints0, 4);
    sequence_t b_1(ints1, 4);
    sequence_t b_2(b_0);
    
    IndexMapper<sequence_t> mymap = IndexMapper<sequence_t>();
    
    CHECK(mymap(b_0) == 0);
    CHECK(mymap(b_1) == 1);
    CHECK(mymap(b_2) == mymap(b_0));
    CHECK(mymap(b_2) != mymap(b_1));
}
