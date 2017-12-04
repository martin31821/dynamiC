#pragma once
/**
 * @file dynamic_memory.h
 * @author Martin Koppehel
 * @date 04.12.2017
 * @brief dynamiC memory allocation library
 *
 * @license MIT
 *
 * @see https://ess-ovgu.github.io/dynamiC
 */ 

#include "stddef.h"
// The semantics of the following three functions are the same as defined in `man malloc`
// The functions are defined to allow the interception of memory allocations to 
// collect metrics and/or implement other memory allocation strategies
typedef void* (*_alloc_fct_t)(size_t);
typedef void* (*_realloc_fct_t)(void*, size_t);
typedef void  (*_free_fct_t)(void*);
extern _alloc_fct_t alloc_mem;
extern _realloc_fct_t realloc_mem;
extern _free_fct_t free_mem;

// This function should print a trace of the current memory usage
typedef void  (*_trace_fct_t)();
extern _trace_fct_t trace_mem_usage;

// A list of available strategies
#define STRATEGY_MALLOC (1)

// defines the used memory allocation strategy
#define MEMORY_ALLOCATION_STRATEGY (STRATEGY_MALLOC)
