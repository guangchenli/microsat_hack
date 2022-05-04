/* 
 * Copyright (c) 2022 Guangchen Li (guangchenli96@gmail.com)
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* A non-generic datastructure for implementing a priority queue that
 * supports random variable modification by their indices.
 */

#ifndef RM_HEAP
#define RM_HEAP

#include <stddef.h>
#include <stdbool.h>

struct rm_heap {
  // indices[lit] is the offset of literal 'lit' is in heap
  // Zero means the lit is not in heap.
  int* indices;
  // lits[offset] is the literal heap[offset]'s value corresponds to
  int* lits;
  // The heap storing float values;
  float* heap;
  // Current size of the heap
  size_t size;
  // Max size of the heap
  size_t max_size;
};

typedef struct rm_heap rm_heap_t;

void rm_heap_init_heap(rm_heap_t* H, size_t s);

// Free memory allocated by the heap
void rm_heap_destroy_heap(rm_heap_t* H);

// Determine whether a literal is in heap
bool rm_heap_in_heap(rm_heap_t* H, int lit);

// Decrease the value of a literal
void rm_heap_decrease(rm_heap_t* H, int lit, float new_val);

// Increase the value of a literal
void rm_heap_increase(rm_heap_t* H, int lit, float new_val);

// A safe version of decrease/increase
void rm_heap_update(rm_heap_t* H, int lit, float new_val);

// Push a value into the heap
void rm_heap_push(rm_heap_t* H, int lit, float val);

// Pop the largest value from the heap
float rm_heap_pop(rm_heap_t* H, int* lit);

// Peek the largest value
float rm_heap_peek(rm_heap_t* H, int* lit);

// Clear all values in the heap
void rm_heap_clear(rm_heap_t* H);

#endif
