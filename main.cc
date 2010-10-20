#include <iostream>
#include <istream>
#include <string>
#include <cstdlib>
#include <ctime>


#include "tclap/CmdLine.h"
#include "nanotime_wrapper/nanotime_wrapper.h"

#include "indexes/debug.h"
#include "indexes/WaveletTree.h"
#include "indexes/SimpleWaveletTree.h"
#include "indexes/MultiRRRWaveletTree.h"

// LIBCDS
#include "WaveletTree.h"
#include "Sequence.h"
#include "Mapper.h"
#include "BitSequenceBuilder.h"
#define DEFAULT_SAMPLING 32
namespace cds = cds_static;

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
        T sym = ALPHABET[rand() % ALPHABET.length()];
        Query<T> q = {pos, sym};
        return q;
    }
};

typedef struct p
{
    bool intSwitch;
    unsigned int queries;
    unsigned int arity;
    unsigned int blocksize;
    unsigned int sbsize;
    unsigned int structure;
    std::string filename;
} params_t;

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

enum structure { AB_RRR, FC_RRR, SIMPLE, N_01RRR };
static const string STRUCTURE_STRINGS[] = { "ab-rrr",
                                            "fc-rrr",
                                            "simple",
                                            "n-01rrr" };
static const int DEFAULT_STRUCTURE = AB_RRR;

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
        
        TCLAP::ValueArg<int> structureArg("t", "type",
            "Data structure type to test", false, DEFAULT_STRUCTURE,
            "Int between 0 and 3", cmd);
        
        cmd.parse( argc, argv );
        
        // Get the value parsed by each arg. 
        params.filename = filenameArg.getValue();
        params.intSwitch = intSwitchArg.getValue();
        params.queries = queriesArg.getValue();
        params.arity = arityArg.getValue();
        params.blocksize = blocksizeArg.getValue();
        params.sbsize = sbsizeArg.getValue();
        params.structure = structureArg.getValue();
        
        if (params.filename == "")
        {
            std::cerr << "Please specify a file..." << std::endl;
            exit(1);
        }
        
        if (params.structure >= 4)
        {
            std::cerr << "Please specify a structure as such:" << std::endl;
            std::cerr << "\tab-rrr  : 0" << endl;
            std::cerr << "\tfc-rrr  : 1" << endl;
            std::cerr << "\tsimple  : 2" << endl;
            std::cerr << "\tn-01rrr : 3" << endl;
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

template <class T, class WT>
stats_t timeQuery(WT & wt, basic_string<T> & alpha,
    params_t & params, stats_t & result)
{   
    cerr << "Generating " << params.queries << " Queries..." << endl;
    QueryGenerator<T> qgen(result.text_length, alpha);
    
    vector< Query<T> > queries(params.queries);
    for (unsigned int i = 0; i < params.queries; i++)
        queries[i] = qgen.next();
    cerr << "Done!" << endl;
    
    
    cerr << "Running Queries..." << endl;
    nanotime_t t0 = get_nanotime();
    
    size_t r;
    for (unsigned int i = 0; i < params.queries; i++)
    {
        if (params.structure == FC_RRR)
            r = wt.rank(queries[i].symbol, queries[i].position);
        else
            r = wt.rank(queries[i].symbol, queries[i].position);
    }
        
    nanotime_t t1 = get_nanotime();
    cerr << "Done!" << endl;
    
    result.time = (t1 - t0);
    
    return result;
}

template <class T>
stats_t doStuff(params_t & params)
{
    stats_t result;
    basic_string<T> alpha;
    
    cerr << "Structure: " << STRUCTURE_STRINGS[params.structure] << endl;
    
    // Clear input buffer after creating WT
    basic_string<T> input = readFile<T>(params.filename.c_str());

     // libcds doesnt provide alphabet accessor
    if(params.structure == FC_RRR)
    {
        alpha = getAlphabet(input);
        result.sigma = alpha.length();
    }
    
    result.text_length = input.length();
    
    cerr << "Building Wavelet Tree..." << endl;
    
    if (params.structure == FC_RRR)
    {
        T * input_ptr = const_cast<T*>(input.c_str());
        cds::MapperNone * map;
        cds::wt_coder_binary * wc;
        cds::BitSequenceBuilder * bsb;
        cds::Sequence * wt;
        // Adapted from Claude's example: http://libcds.recoded.cl/node/9
        map = new cds::MapperNone();
        wc = new cds::wt_coder_binary(unsignedCast(input_ptr), 
            result.text_length, map);
        // Default sampling taken from RRR code...
        bsb = new cds::BitSequenceBuilderRRR(DEFAULT_SAMPLING);
        wt = new cds::WaveletTree(unsignedCast(input_ptr),
            result.text_length, wc, bsb, map);
        cerr << "Done!" << endl;
        
        result = timeQuery(*wt, alpha, params, result);
        
        delete wc;
    }
    else
    {
        if ( params.structure == AB_RRR )
        {
            WaveletTree<T> wt(input, params.arity,
                params.blocksize, params.sbsize);
            alpha = wt.getAlpha();
            result.sigma = alpha.length();
            cerr << "Done!" << endl;

            result = timeQuery(wt, alpha, params, result);
        }
        else if ( params.structure == SIMPLE )
        {
            SimpleWaveletTree<T> wt(input, params.arity);
            alpha = wt.getAlpha();
            result.sigma = alpha.length();
            cerr << "Done!" << endl;

            result = timeQuery(wt, alpha, params, result);
        }
        else if ( params.structure == N_01RRR )
        {
            MultiRRRWaveletTree<T> wt(input, params.arity);
            alpha = wt.getAlpha();
            result.sigma = alpha.length();
            cerr << "Done!" << endl;

            result = timeQuery(wt, alpha, params, result);
        }
        
    }
    
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
