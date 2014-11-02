#include <stdio.h>
#include <stdlib.h>

#include "monomial_tests.h"
#include "polynomial_tests.h"

int main(void) {
  printf("\n\n\n");
  puts("##############################");
  puts("Running tests on MONOMIALS");
  puts("##############################");
  puts("");
  monomial_tests_run();

  printf("\n\n\n");
  puts("##############################");
  puts("Running tests on POLYNOMIALS");
  puts("##############################");
  puts("");
  polynomial_tests_run();

  return EXIT_SUCCESS;
}
