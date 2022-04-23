/**************************************************************[microsat.c]****

  The MIT License

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
#include <stdlib.h>
#include "microsat.h"

// Unassign the literal
void unassign(struct solver *S, int lit) { S->false[lit] = 0; }

// Perform a restart (i.e., unassign all variables)
void restart(struct solver *S) {
  // Remove all unforced false lits from falseStack
  while (S->assigned > S->forced)
    unassign(S, *(--S->assigned));
  // Reset the processed pointer
  S->processed = S->forced;
}

// Make the first literal of the reason true
void assign(struct solver *S, int *reason, int forced) {
  // Let lit be the first literal in the reason
  int lit = reason[0];
  // Mark lit as true and IMPLIED if forced
  S->false[-lit] = forced ? IMPLIED : 1;
  // Push it on the assignment stack
  *(S->assigned++) = -lit;
  // Set the reason clause of lit
  S->reason[abs(lit)] = 1 + (int)((reason)-S->DB);
  // Mark the literal as true in the model
  S->model[abs(lit)] = (lit > 0);
}

// Add a watch pointer to a clause containing lit
// By updating the database and the pointers
void addWatch(struct solver *S, int lit, int mem) {
  S->DB[mem] = S->first[lit];
  S->first[lit] = mem;
}

// Allocate memory of size mem_size
int *getMemory(struct solver *S, int mem_size) {
  // In case the code is used within a code base
  if (S->mem_used + mem_size > S->mem_max) {
    printf("c out of memory\n");
    exit(0);
  }
  // Compute a pointer to the new memory location
  int *store = (S->DB + S->mem_used);
  // Update the size of the used memory
  S->mem_used += mem_size;
  // Return the pointer
  return store;
}

// Adds a clause stored in *in of size size
int *addClause(struct solver *S, int *in, int size, int irr) {
  // Store a pointer to the beginning of the clause
  int i, used = S->mem_used;
  // Allocate memory for the clause in the database
  int *clause = getMemory(S, size + 3) + 2;
  // If the clause is not unit, then add
  // Two watch pointers to the datastructure
  if (size > 1) {

    addWatch(S, in[0], used);
    addWatch(S, in[1], used + 1);
  }
  // Copy the clause from the buffer to the database
  for (i = 0; i < size; i++)
    clause[i] = in[i];
  clause[i] = 0;
  // Update the statistics
  if (irr)
    S->mem_fixed = S->mem_used;
  else
    S->nLemmas++;
  // Return the pointer to the clause in the database
  return clause;
}

// Removes "less useful" lemmas from DB
void reduceDB(struct solver *S, int k) {
  while (S->nLemmas > S->maxLemmas) {
    // Allow more lemmas in the future
    S->maxLemmas += 300;
    // Reset the number of lemmas
    S->nLemmas = 0;
  }
  int i;
  // Loop over the variables
  for (i = -S->nVars; i <= S->nVars; i++) {
    if (i == 0)
      continue;
    // Get the pointer to the first watched clause
    int *watch = &S->first[i];
    // As long as there are watched clauses
    while (*watch != END) {
      // Remove the watch if it points to a lemma
      if (*watch < S->mem_fixed)
        watch = (S->DB + *watch);
      // Otherwise (meaning an input clause) go to next watch
      else
        *watch = S->DB[*watch];
    }
  }
  int old_used = S->mem_used;
  S->mem_used = S->mem_fixed;
  // Virtually remove all lemmas
  // While the old memory contains lemmas
  for (i = S->mem_fixed + 2; i < old_used; i += 3) {
    // Get the lemma to which the head is pointing
    int count = 0, head = i;
    // Count the number of literals
    // That are satisfied by the current model
    while (S->DB[i]) {
      int lit = S->DB[i++];
      if ((lit > 0) == S->model[abs(lit)])
        count++;
    }
    // If the latter is smaller than k, add it back
    if (count < k)
      addClause(S, S->DB + head, i - head, 0);
  }
}

// Move the variable to the front of the decision list
void bump(struct solver *S, int lit) {
  // MARK the literal as involved if not a top-level unit
  if (S->false[lit] != IMPLIED) {
    S->false[lit] = MARK;
    int var = abs(lit);
    // In case var is not already the head of the list
    if (var != S->head) {
      S->prev[S->next[var]] = S->prev[var]; // Update the prev link
      S->next[S->prev[var]] = S->next[var]; // Update the next link
      S->next[S->head] = var;               // Add a next link to the head
      S->prev[var] = S->head;               // Make var the new head
      S->head = var;
    }
  }
}

// Check if lit(eral) is implied by MARK literals
int implied(struct solver *S, int lit) {
  // If checked before return old result
  if (S->false[lit] > MARK)
    return (S->false[lit] & MARK);
  // In case lit is a decision, it is not implied
  if (!S->reason[abs(lit)])
    return 0;
  // Get the reason of lit(eral)
  int *p = (S->DB + S->reason[abs(lit)] - 1);
  // While there are literals in the reason
  while (*(++p))
    // Recursively check if non-MARK literals are implied
    if ((S->false[*p] ^ MARK) && !implied(S, *p)) {
      // Mark and return not implied (denoted by IMPLIED - 1)
      S->false[lit] = IMPLIED - 1;
      return 0;
    }
  // Mark and return that the literal is implied
  S->false[lit] = IMPLIED;
  return 1;
}

// Compute a resolvent from falsified clause
int *analyze(struct solver *S, int *clause) {
  // Bump restarts and update the statistic
  S->res++;
  S->nConflicts++;
  // MARK all literals in the falsified clause
  while (*clause)
    bump(S, *(clause++));
  // Loop on variables on falseStack
  // until the last decision
  while (S->reason[abs(*(--S->assigned))]) {
    // If the tail of the stack is MARK
    if (S->false[*S->assigned] == MARK) {
      // Pointer to check if first-UIP is reached
      int *check = S->assigned;
      // Check for a MARK literal before decision
      while (S->false[*(--check)] != MARK)
        // Otherwise it is the first-UIP so break
        if (!S->reason[abs(*check)])
          goto build;
      // Get the reason and
      // ignore first literal
      clause = S->DB + S->reason[abs(*S->assigned)];
      // MARK all literals in reason
      while (*clause)
        bump(S, *(clause++));
    }
    // Unassign the tail of the stack
    unassign(S, *S->assigned);
  }

build:;
  // Build conflict clause; Empty the clause buffer
  int size = 0, lbd = 0, flag = 0;

  int *p = S->processed = S->assigned;
  // Loop from tail to front
  while (p >= S->forced) {
    // Only literals on the stack can be MARKed
    // If MARKed and not implied
    if ((S->false[*p] == MARK) && !implied(S, *p)) {
      // Add literal to conflict clause buffer
      S->buffer[size++] = *p;
      flag = 1;
    }
    if (!S->reason[abs(*p)]) {
      // Increase LBD for a decision with a true flag
      lbd += flag;
      flag = 0;
      // And update the processed pointer
      if (size == 1)
        S->processed = p;
    }
    // Reset the MARK flag for all variables on the stack
    S->false[*(p--)] = 1;
  }

  // Update the fast moving average
  S->fast -= S->fast >> 5;
  S->fast += lbd << 15;
  // Update the slow moving average
  S->slow -= S->slow >> 15;
  S->slow += lbd << 5;

  // Loop over all unprocessed literals
  while (S->assigned > S->processed)
    // Unassign all lits between tail & head
    unassign(S, *(S->assigned--));
  // Assigned now equal to processed
  unassign(S, *S->assigned);
  // Terminate the buffer (and potentially print clause)
  S->buffer[size] = 0;
  // Add new conflict clause to redundant DB
  return addClause(S, S->buffer, size, 0);
}

// Performs unit propagation
int propagate(struct solver *S) {
  // Initialize forced flag
  int forced = S->reason[abs(*S->processed)];
  // While unprocessed false literals
  while (S->processed < S->assigned) {
    // Get first unprocessed literal
    int lit = *(S->processed++);
    // Obtain the first watch pointer
    int *watch = &S->first[lit];
    // While there are watched clauses (watched by lit)
    while (*watch != END) {
      // Let's assume that the clause is unit
      int i, unit = 1;
      // Get the clause from DB
      int *clause = (S->DB + *watch + 1);
      // Set the pointer to the first literal in the clause
      if (clause[-2] == 0)
        clause++;
      // Ensure that the other watched literal is in front
      if (clause[0] == lit)
        clause[0] = clause[1];
      // Scan the non-watched literals
      for (i = 2; unit && clause[i]; i++)
        // When clause[i] is not false, it is either true or unset
        if (!S->false[clause[i]]) {
          // Swap literals
          clause[1] = clause[i];
          clause[i] = lit;
          // Store the old watch
          int store = *watch;
          unit = 0;
          // Remove the watch from the list of lit
          *watch = S->DB[*watch];
          // Add the watch to the list of clause[1]
          addWatch(S, clause[1], store);
        }
      // If the clause is indeed unit
      if (unit) {
        // Place lit at clause[1] and update next watch
        clause[1] = lit;
        watch = (S->DB + *watch);
        // If the other watched literal is satisfied continue
        if (S->false[-clause[0]])
          continue;
        // If the other watched literal is falsified,
        if (!S->false[clause[0]]) {
          // A unit clause is found, and the reason is set
          assign(S, clause, forced);
        } else {
          // Found a root level conflict -> UNSAT
          if (forced)
            return UNSAT;
          // Analyze the conflict return a conflict clause
          int *lemma = analyze(S, clause);
          // In case a unit clause is found, set forced flag
          if (!lemma[1])
            forced = 1;
          // Assign the conflict clause as a unit
          assign(S, lemma, forced);
          break;
        }
      }
    }
  }
  // Set S->forced if applicable
  if (forced)
    S->forced = S->processed;
  // Finally, no conflict was found
  return SAT;
}

// Determine satisfiability
int solve(struct solver *S) {
  // Initialize the solver
  int decision = S->head;
  S->res = 0;
  // Main solve loop
  for (;;) {
    // Store nLemmas to see whether propagate adds lemmas
    int old_nLemmas = S->nLemmas;
    // Propagation returns UNSAT for a root level conflict
    if (propagate(S) == UNSAT)
      return UNSAT;

    // If the last decision caused a conflict
    if (S->nLemmas > old_nLemmas) {
      // Reset the decision heuristic to head
      decision = S->head;
      // If fast average is substantially larger than slow average
      if (S->fast > (S->slow / 100) * 125) {
        // printf("c restarting after %i conflicts (%i %i) %i\n", S->res,
        //   S->fast, S->slow, S->nLemmas > S->maxLemmas);
        // Restart and update the averages
        S->res = 0;
        S->fast = (S->slow / 100) * 125;
        restart(S);
        // Reduce the DB when it contains too many lemmas
        if (S->nLemmas > S->maxLemmas)
          reduceDB(S, 6);
      }
    }

    // As long as the temporay decision is assigned
    while (S->false[decision] || S->false[-decision]) {
      // Replace it with the next variable in the decision list
      decision = S->prev[decision];
    }
    // If the end of the list is reached, then a solution is found
    if (decision == 0)
      return SAT;
    // Otherwise, assign the decision variable based on the model
    decision = S->model[decision] ? decision : -decision;
    // Assign the decision literal to true (change to IMPLIED-1?)
    S->false[-decision] = 1;
    // And push it on the assigned stack
    *(S->assigned++) = -decision;
    // Decisions have no reason clauses
    decision = abs(decision);
    S->reason[decision] = 0;
  }
}

void initCDCL(struct solver *S, int n, int m) {
  // The code assumes that there is at least one variable
  if (n < 1)
    n = 1;
  // Set the number of variables
  S->nVars = n;
  // Set the number of clauses
  S->nClauses = m;
  // Set the initial maximum memory
  S->mem_max = 1 << 30;
  // The number of integers allocated in the DB
  S->mem_used = 0;
  // The number of learned clauses -- redundant means learned
  S->nLemmas = 0;
  // Number of conflicts used to update scores
  S->nConflicts = 0;
  // Initial maximum number of learned clauses
  S->maxLemmas = 2000;
  // Initialize the fast and slow moving averages
  S->fast = S->slow = 1 << 24;
  // Allocate the initial database
  S->DB = (int *)malloc(sizeof(int) * S->mem_max);
  // Full assignment of the (Boolean) variables (initially set to false)
  S->model = getMemory(S, n + 1);
  // Next variable in the heuristic order
  S->next = getMemory(S, n + 1);
  // Previous variable in the heuristic order
  S->prev = getMemory(S, n + 1);
  // A buffer to store a temporary clause
  S->buffer = getMemory(S, n);
  // Array of clauses
  S->reason = getMemory(S, n + 1);
  // Stack of falsified literals -- this pointer is never changed
  S->falseStack = getMemory(S, n + 1);
  // Points inside *falseStack at first decision (unforced literal)
  S->forced = S->falseStack;
  // Points inside *falseStack at first unprocessed literal
  S->processed = S->falseStack;
  // Points inside *falseStack at last unprocessed literal
  S->assigned = S->falseStack;
  // Labels for variables, non-zero means false
  S->false = getMemory(S, 2 * n + 1);
  S->false += n;
  // Offset of the first watched clause
  S->first = getMemory(S, 2 * n + 1);
  S->first += n;
  // Make sure there is a 0 before the clauses are loaded.
  S->DB[S->mem_used++] = 0;
  int i;
  // Initialize the main datastructures:
  for (i = 1; i <= n; i++) {
    S->prev[i] = i - 1;
    // the double-linked list for variable-move-to-front,
    S->next[i - 1] = i;
    // the model (phase-saving), the false array,
    S->model[i] = S->false[-i] = S->false[i] = 0;
    // and first (watch pointers).
    S->first[i] = S->first[-i] = END;
  }
  // Initialize the head of the double-linked list
  S->head = n;
}

static void read_until_new_line(FILE *input) {
  int ch;
  while ((ch = getc(input)) != '\n')
    if (ch == EOF) {
      printf("parse error: unexpected EOF");
      exit(1);
    }
}

// Parse the formula and initialize
int parse(struct solver *S, char *filename) {
  int tmp;
  // Read the CNF file
  FILE *input = fopen(filename, "r");
  while ((tmp = getc(input)) == 'c')
    read_until_new_line(input);
  ungetc(tmp, input);
  do {
    // Find the first non-comment line
    tmp = fscanf(input, " p cnf %i %i \n", &S->nVars, &S->nClauses);
    // In case a commment line was found
    if (tmp > 0 && tmp != EOF)
      break;
    tmp = fscanf(input, "%*s\n");
  }
  // Skip it and read next line
  while (tmp != 2 && tmp != EOF);
  // Allocate the main datastructures
  initCDCL(S, S->nVars, S->nClauses);
  // Initialize the number of clauses to read
  int nZeros = S->nClauses, size = 0;
  // While there are clauses in the file
  while (nZeros > 0) {
    int ch = getc(input);
    if (ch == ' ' || ch == '\n')
      continue;
    if (ch == 'c') {
      read_until_new_line(input);
      continue;
    }
    ungetc(ch, input);
    int lit = 0;
    // Read a literal.
    tmp = fscanf(input, " %i ", &lit);
    // If reaching the end of the clause
    if (!lit) {
      // Then add the clause to data_base
      int *clause = addClause(S, S->buffer, size, 1);
      // Check for empty clause or conflicting unit
      if (!size || ((size == 1) && S->false[clause[0]]))
        // If either is found return UNSAT
        return UNSAT;
      // Check for a new unit
      if ((size == 1) && !S->false[-clause[0]]) {
        // Directly assign new units (forced = 1)
        assign(S, clause, 1);
      }
      // Reset buffer
      size = 0;
      --nZeros;
    } else
      // Add literal to buffer
      S->buffer[size++] = lit;
  }
  // Close the formula file
  fclose(input);
  // Return that no conflict was observed
  return SAT;
}
