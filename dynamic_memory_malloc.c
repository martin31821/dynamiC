#include "dynamic_memory.h"
/**
 * @file dynamic_memory_malloc.c
 * @author Martin Koppehel
 * @date 04.12.2017
 * @brief dynamiC memory allocation library
 *
 * @license MIT
 *
 * @see https://ess-ovgu.github.io/dynamiC
 */ 

#if MEMORY_ALLOCATION_STRATEGY == STRATEGY_MALLOC
#include <stdlib.h>

#pragma message "Using malloc, realloc, free as memory allocator"

_trace_fct_t   trace_mem_usage = NULL;
_alloc_fct_t   alloc_mem = malloc;
_realloc_fct_t realloc_mem = realloc;
_free_fct_t    free_mem = free;
#endif
