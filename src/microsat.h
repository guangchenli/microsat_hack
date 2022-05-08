/**************************************************************[microsat.h]****

  The MIT License

  Copyright (c) 2022 Guangchen Li
  Copyright (c) 2014-2018 Marijn Heule

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*******************************************************************************/

#ifndef MICROSAT
#define MICROSAT

#include "int_queue.h"

// If a variable is marked as IMPLIED in falseMark, then it is
// a force assignment.
// END marks the end of a linked list
enum { END = -9, UNSAT = 0, SAT = 1, MARK = 2, IMPLIED = 6 };

// The variables in the struct are described in the initCDCL procedure
struct solver {
  int *DB, nVars, nClauses, mem_used, mem_fixed, mem_max, maxLemmas, nLemmas,
    *buffer, nConflicts, *model, *reason, *falseStack, *falseMark, *first,
    *forced, *processed, *assigned, *next, *prev, head, res, nAssigned; /* fast, slow */
  unsigned long int lbd_sum;
  int_queue_t lbd_queue;
  int_queue_t trail_queue;
};

typedef struct solver solver_t;

int solve(solver_t *);

int parse(solver_t *, char *);

#endif
