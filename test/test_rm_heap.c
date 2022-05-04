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
#include <stdio.h>
#include "munit.h"
#include "rm_heap.h"

int test_rm_heap_in_heap() {
  rm_heap_t H;
  rm_heap_init_heap(&H, 100);
  for (int i = 0; i < 100; i++) {
    munit_assert_false(rm_heap_in_heap(&H, i));
  }
  rm_heap_push(&H, 2, 1.1);
  munit_assert_true(rm_heap_in_heap(&H, 2));
  rm_heap_push(&H, 3, 1.6);
  munit_assert_true(rm_heap_in_heap(&H, 3));
  rm_heap_push(&H, 4, 1.3);
  munit_assert_true(rm_heap_in_heap(&H, 4));
  munit_assert_false(rm_heap_in_heap(&H, 5));
  int idx;
  munit_assert_double_equal(rm_heap_pop(&H, &idx), 1.6, 5);
  munit_assert_true(rm_heap_in_heap(&H, 2));
  munit_assert_true(rm_heap_in_heap(&H, 4));
  munit_assert_false(rm_heap_in_heap(&H, 3));
  munit_assert_double_equal(rm_heap_pop(&H, &idx), 1.3, 5);
  munit_assert_true(rm_heap_in_heap(&H, 2));
  munit_assert_false(rm_heap_in_heap(&H, 4));
  munit_assert_false(rm_heap_in_heap(&H, 3));
  munit_assert_double_equal(rm_heap_pop(&H, &idx), 1.1, 5);
  munit_assert_false(rm_heap_in_heap(&H, 2));
  munit_assert_false(rm_heap_in_heap(&H, 4));
  munit_assert_false(rm_heap_in_heap(&H, 3));
  rm_heap_destroy_heap(&H);
  printf("Test rm_heap_in_heap() OK.\n");
}

void test_rm_heap_push_pop() {
  rm_heap_t H;
  rm_heap_init_heap(&H, 100);
  float vals[10] = {1.1, 14.6, 11.6, 0.0, 17.2,
		    85.2, 9.18, 5.42, 2.3, 16.2};
  int order[10] = {6, 5, 10, 2, 3, 7, 8, 9, 1, 4};
  for (int i = 0; i < 10; i++) {
    rm_heap_push(&H, i+1, vals[i]);
  }
  int idx = 0;
  for (int i = 0; i < 10; i++) {
    munit_assert_double_equal(rm_heap_pop(&H, &idx), vals[order[i]-1], 5);
    munit_assert_int(idx, ==, order[i]);
  }
  rm_heap_destroy_heap(&H);
  printf("Test rm_heap_pop() and rm_heap_push() OK.\n");
}

void test_rm_heap_update() {
  rm_heap_t H;
  rm_heap_init_heap(&H, 100);
  float vals[10] = {1.1, 14.6, 11.6, 0.0, 17.2,
		    85.2, 9.18, 5.42, 2.3, 16.2};
  int order[10] = {6, 5, 10, 2, 3, 7, 8, 9, 1, 4};
  for (int i = 0; i < 10; i++) {
    rm_heap_push(&H, i+1, vals[i]);
  }
  // Increase the value
  rm_heap_update(&H, 7, 120.9);
  int idx;
  munit_assert_double_equal(rm_heap_pop(&H, &idx), 120.9, 5);
  munit_assert_int(idx, ==, 7);
  // Decrease the value
  rm_heap_update(&H, 6, 0.1);
  munit_assert_double_equal(rm_heap_pop(&H, &idx), 17.2, 5);
  munit_assert_int(idx, ==, 5);
  rm_heap_destroy_heap(&H);
  printf("Test rm_heap_update() OK.\n");
}

int main() {
  printf("Initializing test.\n");
  test_rm_heap_in_heap();
  test_rm_heap_push_pop();
  test_rm_heap_update();
}
