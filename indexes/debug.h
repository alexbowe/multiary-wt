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
    #include <cstdio>
    #include <string>
    #include <cstring>
    template <class T>
    inline void trace_seq_impl(const std::basic_string<T> & seq, char* fmt)
    {
        unsigned long length = seq.length();
        for (unsigned long i = 0; i< length; i++)
        {
            fprintf(stderr, fmt, seq[i]);
            if (strcmp(fmt, "%d") == 0 && i < length - 1)
                fprintf(stderr, ", ");;
        }
        fprintf(stderr, "\n");
    }
    // don't print commas for chars
    inline void trace_seq_impl(const std::basic_string<char> & seq)
    {
        char fmt[] = {'%', 'c', '\0'};
        trace_seq_impl(seq, fmt);
    }
    inline void trace_seq_impl(const std::basic_string<int> & seq)
    {
        char fmt[] = {'%', 'd', '\0'};
        trace_seq_impl(seq, fmt);
    }
    
    #define TRACE_SEQ(seq) trace_seq_impl seq
#else
    #define TRACE_SEQ(seq)
#endif

#endif /* end of include guard */
