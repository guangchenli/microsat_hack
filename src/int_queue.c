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

#include <stdlib.h>
#include <assert.h>

#include "int_queue.h"

void int_queue_init(int_queue_t* Q, int size) {
  Q->store = malloc(sizeof(int) * size);
  Q->head = 0;
  Q->size = 0;
  Q->sum = 0;
  Q->max_size = size;
}

void int_queue_destroy(int_queue_t* Q) {
  free(Q->store);
}

void int_queue_push(int_queue_t* Q, int e) {
  assert(Q->size < Q->max_size);
  Q->store[Q->head] = e;
  if (Q->head >= Q->max_size - 1) {
    Q->head = 0;
  } else {
    Q->head++;
  }
  Q->sum += e;
  Q->size++;
}

int int_queue_pop(int_queue_t* Q) {
  assert(Q->size > 0);
  int tail = Q->store[(Q->head + Q->max_size - Q->size) % Q->max_size];
  Q->size--;
  Q->sum -= tail;
  return tail;
}

inline int int_queue_size(int_queue_t* Q) {
  return Q->size;
}

void int_queue_clean(int_queue_t* Q) {
  Q->head = 0;
  Q->size = 0;
  Q->sum = 0;
}

float int_queue_avg(int_queue_t* Q) {
  return Q->sum / (float)Q->size;
}
