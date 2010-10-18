#include <iostream>
#include <istream>
#include <string>
#include <cstdlib>
#include <ctime>

#include "indexes/WaveletTree.h"
#include "tclap/CmdLine.h"
#include "indexes/debug.h"
#include "nanotime_wrapper/nanotime_wrapper.h"

//#define LIBCDS

#ifdef LIBCDS
#include "WaveletTree.h"
#include "Sequence.h"
#include "Mapper.h"
#include "BitSequenceBuilder.h"
#define DEFAULT_SAMPLING 32
namespace cds = cds_static;
#endif

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
static const size_t DEFAULT_SBSIZE = 32;

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
        TCLAP::CmdLine cmd("WaveletTree Test Bed by Alex Bowe", ' ', "0.1.5");
        
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

inline unsigned int * unsignedCast(int * p)
{ return (unsigned int*) p; }

inline unsigned char * unsignedCast(char * p)
{ return (unsigned char*) p; }


template <class T>
stats_t doStuff(params_t & params)
{
    stats_t result;
    
    // Clear input buffer after creating WT
    basic_string<T> input = readFile<T>(params.filename.c_str());
    
    #ifdef LIBCDS // libcds doesnt provide alphabet accessor
    basic_string<T> alpha = getAlphabet(input);
    #endif
    
    result.text_length = input.length();
    
    cerr << "Building Wavelet Tree..." << endl;
    
    #ifdef LIBCDS
        T * input_ptr = const_cast<T*>(input.c_str());
        // Adapted from Claude's example: http://libcds.recoded.cl/node/9
        cds::MapperNone * map = new cds::MapperNone();
        cds::wt_coder_binary * wc = new
            cds::wt_coder_binary(unsignedCast(input_ptr),
            result.text_length, map);
        // Default sampling taken from RRR code...
        cds::BitSequenceBuilder * bsb = new
            cds::BitSequenceBuilderRRR(DEFAULT_SAMPLING);
        cds::Sequence * wt = new cds::WaveletTree(unsignedCast(input_ptr),
            result.text_length, wc, bsb, map);
    #else
        WaveletTree<T> wt(input, params.arity,
            params.blocksize, params.sbsize);
        basic_string<T> alpha = wt.getAlpha();
    #endif
    
    cerr << "Done!" << endl;
    
    result.sigma = alpha.length();
    
    cerr << "Generating " << params.queries << " Queries..." << endl;
    QueryGenerator<T> qgen(result.text_length, alpha);
    
    vector< Query<T> > queries(params.queries);
    for (unsigned int i = 0; i < params.queries; i++)
        queries[i] = qgen.next();
    cerr << "Done!" << endl;
    
    
    cerr << "Running Queries..." << endl;
    nanotime_t t0 = get_nanotime();
    for (unsigned int i = 0; i < params.queries; i++)
    {
        #ifdef LIBCDS
        wt->rank(queries[i].symbol, queries[i].position);
        #else
        wt.rank(queries[i].symbol, queries[i].position);
        #endif
    }
        
    nanotime_t t1 = get_nanotime();
    cerr << "Done!" << endl;
    
    result.time = (t1 - t0);
    
    #ifdef LIBCDS
    delete wc;
    #endif
    
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
