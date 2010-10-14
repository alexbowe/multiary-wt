#ifndef DEBUG_H
#define DEBUG_H

// Assert
#if DEBUG
    #include <cassert>
    #define myAssert(expression) assert(expression)
#else
    #define myAssert(expression)
#endif

// Tracing
// NOTE: Needs to be called using double brackets:
//                  TRACE( ("%d", variable) );
#if DEBUG
    #include <cstdarg>
    #include <cstdio>
    
    inline void trace_impl(const char * format, ...)
    {
        const static size_t BUFFER_SIZE = 1024;
        char buffer[BUFFER_SIZE];
        
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, BUFFER_SIZE, format, args);
        va_end (args);
        
        fprintf(stderr, "%s", buffer);
    }
    
    #define TRACE(msg) trace_impl msg
#else
    #define TRACE(msg)
#endif

#if DEBUG
    #include <iostream>
    #include <string>
    template <class T>
    inline void trace_seq_impl(const basic_string<T> & seq, bool commas)
    {
        for (unsigned long i = 0; i< seq.length(); i++)
        {
            std::cerr << seq[i];
            if (commas && i < seq.length() - 1) std::cerr << ", ";
        }
        std::cerr << std::endl;
    }
    // don't print commas for chars
    inline void trace_seq_impl(const basic_string<char> & seq)
    { trace_seq_impl(seq, false); }
    inline void trace_seq_impl(const basic_string<int> & seq)
    { trace_seq_impl(seq, true); }
    
    #define TRACE_SEQ(seq) trace_seq_impl seq
#else
    #define TRACE_SEQ(seq)
#endif

#endif /* end of include guard */
