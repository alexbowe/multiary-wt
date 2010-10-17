#include "CppUnitLite/TestHarness.h"
#include "indexes/WaveletTree.h"
#include <vector>
#include <map>
#include "TestHelpers.h"
#include "WTTestHelpers.h"

using namespace std;

/*
TEST( WaveletTree, charRank )
{
    basic_string<char> input = "this isnt fun";
    
    basic_string<char> alphabet = getAlphabet(input);
    map<char, CountVect> counters = prepareCountMap(input, alphabet);
    
    for (int arity = 2; arity <= 8; arity++)
        for (int blocksize = 5; blocksize <= 15; blocksize+= 5)
            for (int sb_fact = 2; sb_fact <= 6; sb_fact+=2)
                CHECK(doCountTest(input, counters, arity, blocksize, sb_fact));
}

TEST( WaveletTree, intRank )
{
    int ints[] = {1, 1, 3, 1, 0, 2, 4};
    basic_string<int> input(ints, ARRAY_SIZE(ints, int));
    
    basic_string<int> alphabet = getAlphabet(input);
    map<int, CountVect> counters = prepareCountMap(input, alphabet);
    
    for (int arity = 2; arity <= 8; arity*=2)
        for (int blocksize = 5; blocksize <= 15; blocksize+= 5)
            for (int sb_fact = 1; sb_fact <= 5; sb_fact+=2)
                CHECK(doCountTest(input, counters, arity, blocksize, sb_fact));
}*/

TEST( WaveletTree, charRankHardcore )
{
    basic_string<char> input = "this is a test string! It's pretty long so "
        " I hope it works. Maybe I should make a test text generator too, "
        " because this is nothing compared to the actual documents I want to"
        " use this on. Oh well, here goes... Yay! It passed... now fingers"
        " crossed for the experiments! :) Anyway, how was your day?" ;
        
    basic_string<char> alphabet = getAlphabet(input);
    map<char, CountVect> counters = prepareCountMap(input, alphabet);
    
    
    /*for (int arity = 2; arity <= 8; arity*=2)
        for (int blocksize = 5; blocksize <= 10; blocksize+= 5)
            for (int sb_fact = 1; sb_fact <= 5; sb_fact+=2)
                CHECK(doCountTest(input, counters, arity, blocksize, sb_fact));
                */
    CHECK(doCountTest(input, counters, 4, 5, 3));
}