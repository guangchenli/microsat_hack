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
#include <string.h>

#include "microsat.h"
#include "utils.h"

#define MAX_LINE_LEN 80

// Print the model if satisfiable
void print_model(solver_t *S) {
  char buffer[32];
  size_t current_lit_len;
  // Count the number of characters in the line
  unsigned int line_count = 2;
  printf("v ");
  for (int i = 0; i < S->nVars; i++) {
    itoa((S->model[i] ? i + 1 : -i - 1), buffer, 10);
    current_lit_len = strlen(buffer);
    // If max length is exceeded
    if (current_lit_len + line_count > MAX_LINE_LEN) {
      // Including the 'v ' and space after literal
      line_count = current_lit_len + 3;
      printf("\nv %s ", buffer);
    } else {
      line_count += current_lit_len + 1;
      printf("%s", buffer);
      // If there is remaining length for a space
      if (current_lit_len + line_count + 1 <= MAX_LINE_LEN) {
        printf(" ");
      }
    }
  }
  printf("\n");
}

// The main procedure for a STANDALONE solver
int main(int argc, char **argv) {
  // Create the solver datastructure
  solver_t S;
  // Parse the DIMACS file in argv[1]
  if (parse(&S, argv[1]) == UNSAT) {
    printf("s UNSATISFIABLE\n");
    // Solve without limit (number of conflicts)
  } else if (solve(&S) == UNSAT) {
    printf("s UNSATISFIABLE\n");
  } else {
    // And print whether the formula has a solution
    printf("s SATISFIABLE\nc \n");
    // Print model
    print_model(&S);
  }
  printf("c \nc statistics of %s:\nc mem: %i conflicts: %i max_lemmas: %i\n",
         argv[1], S.mem_used, S.nConflicts, S.maxLemmas);
}
