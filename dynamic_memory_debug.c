#include "dynamic_memory.h"
#include "dynamic_memory_debug.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"

#define INTERCEPTOR_ALLOCATION_NUM (128)

typedef struct {
    void* ptr;
    size_t length;
    uint32_t flags;
    uint32_t num_reallocs;
} _allocation_t;

typedef struct {
    unsigned established;
    _alloc_fct_t allocator;
    _realloc_fct_t reallocator;
    _free_fct_t free;
    size_t num_allocs;
    size_t used_allocs;
    _allocation_t* allocations;
    
} interceptor_t;

static interceptor_t _interceptor;

static void add_alloc(void* ptr, size_t size) {
    if (_interceptor.num_allocs == _interceptor.used_allocs) {
        _interceptor.allocations = realloc(_interceptor.allocations, _interceptor.num_allocs + INTERCEPTOR_ALLOCATION_NUM);
        _interceptor.num_allocs += INTERCEPTOR_ALLOCATION_NUM;
    }
    size_t allocid = _interceptor.used_allocs;
    _interceptor.allocations[allocid].ptr = ptr;
    _interceptor.allocations[allocid].length = size;
    _interceptor.allocations[allocid].flags = 0;
    _interceptor.allocations[allocid].num_reallocs = 0;
}

static void add_realloc(void* oldptr, void* newptr, size_t size) {
    if (!oldptr) {
        add_alloc(newptr, size);
        return;
    }
    if (!size) {
        // todo: 
        // call add_free
        return;
    }
    
    for (size_t i = 0; i < _interceptor.used_allocs; i++) {
        if (_interceptor.allocations[i].ptr == oldptr) {
            _interceptor.allocations[i].num_reallocs++;
            _interceptor.allocations[i].ptr = newptr;
            _interceptor.allocations[i].length = size;
            break;
        }
    }
    // not found, what should we do now?
}

static void add_free(void* ptr) {
    if (!ptr) return; // ignore free(NULL)
    size_t i = 0;
    unsigned found = 0;
    for (; i < _interceptor.used_allocs; i++) {
        if (_interceptor.allocations[i].ptr == ptr) {
            found = 1;
            break;
        }
    }
    if (!found) {
        // TODO: Print a warning about double free here
        return;
    }
    if (i < _interceptor.used_allocs - 1) {
       memmove(
           &_interceptor.allocations[i], 
           &_interceptor.allocations[i+1], 
           sizeof(_allocation_t)*(_interceptor.used_allocs - (i + 1))
        );
    }
    _interceptor.used_allocs--;
}

static void* _intercept_malloc(size_t size) {
    void* ptr = _interceptor.allocator(size);
    add_alloc(ptr, size);
    return ptr;
}

static void* _intercept_realloc(void* ptr, size_t size) {
    void* newptr = _interceptor.reallocator(ptr, size);
    add_realloc(ptr, newptr, size);
    return newptr;
}

static void _intercept_free(void* ptr) {
    add_free(ptr);
    _interceptor.free(ptr);
}

void intercept_memory_allocation () {
    if (_interceptor.established) return;
    
    _interceptor.allocator = alloc_mem;
    _interceptor.reallocator = realloc_mem;
    _interceptor.free = free_mem;
    alloc_mem = _intercept_malloc;
    realloc_mem = _intercept_realloc;
    free_mem = _intercept_free;
    
    if (_interceptor.allocations)
        free(_interceptor.allocations);
    
    _interceptor.allocations = (_allocation_t*)malloc(INTERCEPTOR_ALLOCATION_NUM * sizeof(_allocation_t));
    _interceptor.used_allocs = 0;
    _interceptor.num_allocs = INTERCEPTOR_ALLOCATION_NUM;
    
    _interceptor.established = 1;
}

void undo_memory_interception() {
    if (!_interceptor.established) return;
    
    alloc_mem = _interceptor.allocator;
    realloc_mem = _interceptor.reallocator;
    free_mem = _interceptor.free;
    _interceptor.established = 0;
}
