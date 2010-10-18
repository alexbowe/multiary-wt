#include <iostream>
#include <istream>
#include <string>
#include <cstdlib>
#include <ctime>
#include "indexes/WaveletTree.h"
#include "tclap/CmdLine.h"
#include "indexes/debug.h"
#include "nanotime_wrapper/nanotime_wrapper.h"

using namespace std;
using namespace indexes;

template <class T>
class Query
{
public:
    size_t position;
    T symbol;
};

template <class T>
class QueryGenerator
{
private:
    size_t TEXT_LENGTH;
    const basic_string<T> & ALPHABET;
    
public:
    QueryGenerator(size_t text_length, const basic_string<T> & alphabet);
    inline Query<T> next()
    {
        size_t pos = rand() % TEXT_LENGTH;
        T sym = ALPHABET[rand()%ALPHABET.length()];
        Query<T> q = {pos, sym};
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

static const size_t DEFAULT_QUERIES = 1e3;
static const size_t DEFAULT_ARITY = 2;
static const size_t DEFAULT_BLOCKSIZE = 15;
static const size_t DEFAULT_SBSIZE = 6;

typedef struct p
{
    bool intSwitch;
    unsigned int queries;
    unsigned int arity;
    unsigned int blocksize;
    unsigned int sbsize;
    std::string filename;
} params_t;

void parseArgs(int argc, char **argv, params_t & params)
{
    // TClap throws exceptions when invalid params are passed...
    try
    {
        TCLAP::CmdLine cmd("wt", ' ', "0.2");
        
        TCLAP::SwitchArg intSwitchArg("i", "intparse",
            "treats input file as integers instead of chars",
            cmd, false);
        
        TCLAP::ValueArg<std::string> filenameArg("f", "file", 
            "File to create WT from", false, "", "filename", cmd);
        
        TCLAP::ValueArg<unsigned int> queriesArg("q", "queries", 
            "number of queries to test", false, DEFAULT_QUERIES,
            "positive integer", cmd);
        
        TCLAP::ValueArg<unsigned int> arityArg("a", "arity", 
            "arity for WT", false, DEFAULT_ARITY,
            "positive integer > 2", cmd);
        
        TCLAP::ValueArg<unsigned int> blocksizeArg("b", "blocksize", 
            "blocksize for RRR", false, DEFAULT_BLOCKSIZE,
            "positive integer >= 1", cmd);
        
        TCLAP::ValueArg<unsigned int> sbsizeArg("s", "sbfactor", 
            "how many blocks are in a super block for RRR", false,
            DEFAULT_SBSIZE, "positive integer >= 1", cmd);
        
        cmd.parse( argc, argv );
        
        // Get the value parsed by each arg. 
        params.filename = filenameArg.getValue();
        params.intSwitch = intSwitchArg.getValue();
        params.queries = queriesArg.getValue();
        params.arity = arityArg.getValue();
        params.blocksize = blocksizeArg.getValue();
        params.sbsize = sbsizeArg.getValue();
        
        if (params.filename == "")
        {
            std::cerr << "Please specify a file..." << std::endl;
            exit(1);
        }
    }
    catch (TCLAP::ArgException &e)
    {
        std::cerr << "error: " << e.error();
        std::cerr << " for arg " << e.argId() << std::endl;
    }
}

template <class T>
basic_string<T> readFile(const char * filename)
{
    FILE * input_fp = fopen(filename, "rb");
    
    if (input_fp == NULL)
    {
        fprintf(stderr, "Unable to open file %s. ", filename);
        fprintf(stderr, "Run with --help flag for info.\n");
        exit(1);
    }

    //Get file length
    fseek(input_fp, 0, SEEK_END);
    unsigned long file_length = ftell(input_fp);
    fseek(input_fp, 0, SEEK_SET);

    unsigned int n = file_length/(sizeof(T));

    T * buffer = (T*)calloc(n, sizeof(T));
    fread(buffer, sizeof(T), n, input_fp);
    fclose(input_fp);

    basic_string<T> input(buffer, n);
    free(buffer);
    return input;
}

typedef struct stats
{
    nanotime_t time;
    size_t text_length;
    size_t sigma;
    size_t table_size;
    size_t seq_size;
    size_t wt_size;
} stats_t;

template <class T>
stats_t doStuff(params_t & params)
{
    stats_t result;
    
    // Clear input buffer after creating WT
    basic_string<T> input = readFile<T>(params.filename.c_str());
    result.text_length = input.length();
    
    cerr << "Building Wavelet Tree..." << endl;
    WaveletTree<T> wt(input, params.arity, params.blocksize, params.sbsize);
    cerr << "Done!" << endl;
    
    result.sigma = wt.getAlpha().length();
    
    cerr << "Generating " << params.queries << " Queries..." << endl;
    QueryGenerator<T> qgen(input.length(), wt.getAlpha());
    
    vector< Query<T> > queries(params.queries);
    for (unsigned int i = 0; i < params.queries; i++)
        queries[i] = qgen.next();
    cerr << "Done!" << endl;
    
    
    cerr << "Running Queries..." << endl;
    nanotime_t t0 = get_nanotime();
    for (unsigned int i = 0; i < params.queries; i++)
        wt.rank(queries[i].symbol, queries[i].position);
    nanotime_t t1 = get_nanotime();
    cerr << "Done!" << endl;
    
    result.time = (t1 - t0);
    
    return result;
}

int main(int argc, char **argv)
{
    // Get the value parsed by each arg. 
    params_t params;
    
    parseArgs(argc, argv, params);
    
    stats_t result;
    
    if (params.intSwitch)
    {
        result = doStuff<int>(params);
    }
    else
        result = doStuff<char>(params);
    
    cout << "Sigma            : " << result.sigma << endl;
    cout << "Mean Time   (ms) : " << ((float)result.time/params.queries)/1e6
         << endl;
    cout << "Seq Size (bytes) : " << endl;
    cout << "RRR Size (bytes) : " << endl;
    cout << "WT Size  (bytes) : " << endl;
    
    return 0;
}

/*



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
*/
