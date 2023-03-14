#include <stdio.h>
#include <stdlib.h>
#include "papi.h"

#define ROWS 4096 // do not change
#define COLS 512  // do not change

typedef struct {
  unsigned long long toAccess; // use this. do not change
  unsigned long long ignore[7]; // ignore this. do not change
} elementOf2DArray;

void part1A_1B(elementOf2DArray (&myArray)[ROWS][COLS]) {
  /* Start your code for Part 1A here */
  
  /* Start your code for Part 1B here */

  // Print your conceptual answer to Part 1
}

void part2(elementOf2DArray (&myArray)[ROWS][COLS]) {
  /* Start your code for Part 2 here */
  // Print your conceptual answer to Part 2
}

void part3(elementOf2DArray (&myArray)[ROWS][COLS]) {
  /* Start your code for Part 3 here */
  // Print your conceptual answer to Part 3
}

int main() { //Do not change anything in main
  // Allocating and initializing a 2D array.
  elementOf2DArray array[ROWS][COLS];
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      array[i][j].toAccess = i*j;
    }
  }
  
  // Calling functions for Part 1A, 1B, 2, and 3.
  part1A_1B(array);
  part2(array);
  part3(array);

  return 0;
}
