/* Remember to comment this with auto name tag and auto version tag,
   and maybe compilation instructions and auto license tag */

#pragma once
#ifndef NANOTIME_WRAPPER_H
#define NANOTIME_WRAPPER_H

/* System Header Inclusion */
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represents time in nanoseconds
 */
typedef uint64_t nanotime_t;

/**
 * Wrapper to get time (in ns) since an arbitrary time
 */
nanotime_t get_nanotime(void);

#ifdef __cplusplus
}
#endif

#endif /* NANOTIME_WRAPPER_H */
