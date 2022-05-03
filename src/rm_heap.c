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
 * supports random internal variable modification by their index.
 */

#include <stdlib.h>
#include <assert.h>

#include "rm_heap.h"

void rm_heap_init_heap(rm_heap_t* H, size_t s){
  H->indices = (int*)malloc(s * sizeof(int));
  H->lits = (int*)malloc(s * sizeof(int));
  H->heap = (float*)malloc(s * sizeof(float));
  size = 0;
  // Initialize indices
  for (int i = 0; i < s; i++) {
    H->indices[i] = -1;
    H->lits[i] = -1;
  }
}


void rm_heap_destroy_heap(rm_heap_t* H) {
  free(H->indices);
  free(H->lits);
  free(H->heap);
}


// Determine whether a literal is in heap
inline bool rm_heap_in_heap(rm_heap_t* H, int lit) {
  return H->heap[lit] >= 0;
}

// Helper macros for getting left/right child or parent

#define GET_LEFT(x) (x * 2)
#define GET_RIGHT(x) (x * 2 + 1)
#define GET_PARENT(x) (x % 2 == 0 ? x / 2 : (x - 1) / 2)

// Swap two values
#define FSWAP(x,y) {float tmp = x; x = y; y = tmp;}
#define ISWAP(x,y) {int tmp = x; x = y; y = tmp;}

// Swap S->heap[offset1]. S->heap[offset2] and update lookup tables
inline void rm_heap_helper_swap_val(rm_heap_t* H,
				    size_t offset1, size_t offset2) {
  int lit1 = H->lits[offset1];
  int lit2 = H->lits[offset2];
  // First swap the value in heap
  FSWAP(H->heap[offset1], H->heap[offset2]);
  // Then update the lookup tables
  ISWAP(H->lits[offset1], H->lits[offset2]);
  ISWAP(H->indices[lit1], H->indices[lit2]);
}

// Let a particular element in heap swim up and return
// its new offset in heap
size_t rm_heap_swim(rm_heap_t* H, size_t offset) {
  assert(offset < H->size);
  size_t parent_offset;
  for(;;) {
    if (offset == 0) break;
    parent_offset = GET_PARENT(offset);
    // If we really need to swap value
    if (H->heap[parent_offset] < H->heap[offset]) {
      // Swap parent and child
      rm_heap_helper_swap_val(H, offset, parent_offset);
      // update offset
      offset = parent_offset;
    } else { break; }
  }
  return offset;
}

// Determine which one is the largest, also do boundary check
// 0 = parent, 1 = right, 2 = right
int rm_heap_helper_get_max(rm_heap_t* H, size_t parent,
			size_t left, size_t right) {
  float parent_val, left_val, right_val;
  float max = H->heap[parent];
  int max_idx = 0;
  if (left < H->size && H->heap[left] > max) {
    max = H->heap[left];
    max_idx = 1;
  }
  if (right < H->size && H->heap[right] > max) {
    max_idx = 2;
  }
  return max_idx;
}

size_t rm_heap_dive(rm_heap_t* H, size_t offset) {
  assert(offset < H->size);
  size_t child_offset, left, right;
  bool break_flag = false;
  for(;;) {
    left = GET_LEFT(offset);
    right = GET_RIGHT(offset);
    switch (rm_heap_helper_get_max(H, offset, left, right)) {
    case 0: // Parent is the largest, stop the loop
      break_flag = true;
      break;
    case 1: // Left child is the largest
      rm_heap_helper_swap_val(H, left, offset);
      offset = left;
      break;
    case 2: // Right child is the largest
      rm_heap_helper_swap_val(H, right, offset);
      offset = right;
      break;
    default:
      break;
    }
    if (break_flag)
      break;
  }
  return offset;
}


void rm_heap_decrease(rm_heap_t* H, int lit, float new_val) {
  size_t heap_offset = H->indices[lit];
  H->heap[heap_offset] = new_val;
  size_t new_offset = rm_heap_dive(H, heap_offset);
}


void rm_heap_increase(rm_heap_t* H, int lit, float new_val) {
  size_t heap_offset = H->indices[lit];
  H->heap[heap_offset] = new_val;
  size_t new_offset = rm_heap_swim(H, heap_offset);
}


void rm_heap_update(rm_heap_t* H, int lit, float new_val) {
  size_t heap_offset = H->indices[lit];
  float old_val = H->heap[heap_offset];
  if (new_val > old_val) {
    rm_heap_increase(H, lit, new_val);
  } else if (new_val < old_val) {
    rm_heap_decrease(H, lit, new_val);
  }
}


void rm_heap_push(rm_heap_t* H, int lit, float val) {
  // Put the value at the end of the heap
  H->heap[H->size] = val;
  H->indices[lit] = H->size;
  H->lits[H->size] = lit;
  // Increase heap size
  H->size++;
  // Let it swim up and restore heap invariant
  rm_heap_swim(H, H->size);
}


float rm_heap_pop(rm_heap_t* H, int* lit) {
  // First get the results
  float result = H->heap[0];
  *lit = H->lits[0]
  // Swap top and bot element
  rm_heap_helper_swap_val(H, 0, H->size - 1);
  // Unset indices assignment
  H->indices[*lit] = 0;
  // Shrink heap size and dive
  H->size--;
  rm_heap_dive(H, 0);
}


void rm_heap_clear(rm_heap_t* H) {
  rm_heap_destroy_heap(H);
  rm_heap_init_heap(H, H->size);
}
