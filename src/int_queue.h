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

#ifndef INT_QUEUE
#define INT_QUEUE

typedef struct int_queue {
  int* store;
  int head;
  int size;
  int max_size;
  long int sum;
} int_queue_t;

void int_queue_init(int_queue_t* Q, int size);

void int_queue_destroy(int_queue_t* Q);

void int_queue_push(int_queue_t* Q, int e);

int int_queue_pop(int_queue_t* Q);

int int_queue_size(int_queue_t* Q);

void int_queue_clean(int_queue_t* Q);

float int_queue_avg(int_queue_t* Q);

#endif
