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
