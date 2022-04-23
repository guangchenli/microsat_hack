/**************************************************************[main.c]********

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


#include <stdio.h>
#include "microsat.h"

// The main procedure for a STANDALONE solver
int main(int argc, char **argv) {
  // Create the solver datastructure
  struct solver S;
  // Parse the DIMACS file in argv[1]
  if (parse(&S, argv[1]) == UNSAT)
    printf("s UNSATISFIABLE\n");
  // Solve without limit (number of conflicts)
  else if (solve(&S) == UNSAT)
    printf("s UNSATISFIABLE\n");
  else
    // And print whether the formula has a solution
    printf("s SATISFIABLE\n");
  printf("c statistics of %s: mem: %i conflicts: %i max_lemmas: %i\n", argv[1],
         S.mem_used, S.nConflicts, S.maxLemmas);
}
