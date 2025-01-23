/* 
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

#ifndef _SECURE_STORAGE_LOG_H_
#define _SECURE_STORAGE_LOG_H_

#include <stdio.h>
#include <stdlib.h>

# ifdef __cplusplus
extern "C" {
# endif /* ifdef __cplusplus */


#if defined ( _WIN32 )
#define __func__ __FUNCTION__
#endif

// Log macros use printf function; only for Freertos target the printf function
// should be forwarded to the PRINTF function implemented in debug console
#if defined(SDK_OS_FREE_RTOS)
#include "fsl_debug_console.h"
#if defined(printf)
#undef printf
#endif
#define printf PRINTF
#endif	
	
/**
 * @brief Debug level logging macro.
 *
 * Macro to expose function, line number as well as desired log message.
 */
# if defined(ENABLE_SECURE_STORAGE_DEBUG) && ENABLE_SECURE_STORAGE_DEBUG
#  define LOG_DBG(...)                       \
  do {                                               \
    printf("DEBUG:   %s L#%d ", __func__, __LINE__); \
    printf(__VA_ARGS__);                             \
    printf("\n");                                    \
  } while(0)
# else  /* ifdef ENABLE_SECURE_STORAGE_DEBUG */
#  define LOG_DBG(...)
# endif /* ifdef ENABLE_SECURE_STORAGE_DEBUG */


# if defined(ENABLE_SECURE_STORAGE_WARN) && ENABLE_SECURE_STORAGE_WARN
#  define LOG_WRN(...)                       \
  do {                                               \
    printf("WARN:   %s L#%d ", __func__, __LINE__); \
    printf(__VA_ARGS__);                             \
    printf("\n");                                    \
  } while(0)
# else  /* ifdef ENABLE_SECURE_STORAGE_WARN*/
#  define LOG_WRN(...)
# endif /* ifdef ENABLE_SECURE_STORAGE_WARN*/
	
# if defined(ENABLE_SECURE_STORAGE_ERROR) && ENABLE_SECURE_STORAGE_ERROR
#  define LOG_ERR(...)                       \
  do {                                               \
    printf("ERROR:   %s L#%d ", __func__, __LINE__); \
    printf(__VA_ARGS__);                             \
    printf("\n");                                    \
  } while(0)
# else  /* ifdef ENABLE_SECURE_STORAGE_ERR*/
#  define LOG_ERR(...)
# endif /* ifdef ENABLE_SECURE_STORAGE_ERR*/

# ifdef __cplusplus
}
# endif /* ifdef __cplusplus */

#endif // _SECURE_STORAGE_LOG_H_
