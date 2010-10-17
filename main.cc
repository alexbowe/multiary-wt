#include <iostream>
#include <istream>
#include <string>
#include <cstdlib>
#include <ctime>
//#include "boost/random.hpp"
//#include "boost/generator_iterator.hpp"

#include "indexes/WaveletTree.h"

using namespace std;
using namespace indexes;

inline void flushis(std::istream& in)
{
    char discard;
    while (in.get(discard) && discard != '\n') {}
    in.get();
}

template <class T>
struct query_struct
{
    size_t position;
    T symbol;
};

template <class T>
class QueryGenerator
{
private:
    typedef struct query_struct<T> query;
    size_t TEXT_LENGTH;
    const basic_string<T> & ALPHABET;
    
public:
    QueryGenerator(size_t text_length, const basic_string<T> & alphabet);
    inline query next()
    {
        size_t pos = rand() % TEXT_LENGTH;
        T sym = ALPHABET[rand()%ALPHABET.length()];
        query q = {pos, sym};
        return q;
    }
};

template <class T>
QueryGenerator<T>::QueryGenerator(size_t text_length,
    const basic_string<T> & alphabet): 
    TEXT_LENGTH(text_length),
    ALPHABET(alphabet)
{
    srand(static_cast<unsigned int>(clock( )));
}

int main(int argc, char *argv[])
{
    char * filename;
    FILE * input_fp;
    if (argc != 2)
    {
        fprintf(stderr, "Usage: wtree <inputfile>\n");
        exit(1);
    }
    else
    {
        filename = argv[1];
        input_fp = fopen(filename, "r");
        if (input_fp == NULL)
        {
            fprintf(stderr, "Unable to open file %s\n", filename);
        }
    }

    //Get file length
    fseek(input_fp, 0, SEEK_END);
    unsigned long file_length = ftell(input_fp);
    fseek(input_fp, 0, SEEK_SET);

    unsigned int n = file_length/(sizeof(char));

    char * buffer = (char*)calloc(n, sizeof(char));
    fread(buffer, sizeof(char), n, input_fp);
    fclose(input_fp);

    basic_string<char> input(buffer, n);
    free(buffer);
    
    cerr << "Building Wavelet Tree..." << endl;
    WaveletTree<char> wt(input, 2, 15, 3);
    cerr << "Done!" << endl;
    
    //basic_string<char> alpha = wt.getAlpha();
    //cerr << "ALPHABET: " << alpha << endl;
    
    QueryGenerator<char> qgen(input.length(), wt.getAlpha());
    
    flushis(cin);
    
    int numQueries = 1e3; // a thousand queries
    typedef struct query_struct<char> query;
    vector<query> queries = vector<query>(numQueries);
    for (int i = 0; i < numQueries; i++)
        queries[i] = qgen.next();
    
    cerr << "querying..." << endl;
    for (int i = 0; i < numQueries; i++)
        wt.rank(queries[i].symbol, queries[i].position);
    cerr << "done" << endl;
    
    return 0;
}
