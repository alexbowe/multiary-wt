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

#endif /* end of include guard */
