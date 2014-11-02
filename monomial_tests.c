
#include <stdio.h>
#include "Monomial.h"

#define NUMBER_OF_TEST_MONOMIALS 10
#define TEST_X 5

static void dump_monomials_errno(void) {
  switch(monomials_errno) {
    case MONOMIAL_MATH_ERROR:
      puts("MONOMIAL_MATH_ERROR");
      break;

    case MONOMIAL_ILLEGAL_OPERATION:
      puts("MONOMIAL_ILLEGAL_OPERATION");
      break;

    case MONOMIAL_INPUT_ERROR:
      puts("MONOMIAL_INPUT_ERROR");
      break;

    default: break;
  }
}

void monomial_tests_run(void) {
  Monomial *monomials[NUMBER_OF_TEST_MONOMIALS];
  int index = 0;

  for(; index < NUMBER_OF_TEST_MONOMIALS; index++) {
    monomials_errno = MONOMIAL_SUCCESS;

    monomials[index] = monomial_create(index, index % (NUMBER_OF_TEST_MONOMIALS / 2));

    dump_monomials_errno();
  }

  printf("==========INITIALIZATIONS==========\n");
  for(index = 0; index < NUMBER_OF_TEST_MONOMIALS; index++) {
    printf("M%d(x) = ", index);
    monomial_print(monomials[index]);
    printf("\n");
  }

  printf("\n==========COMPUTATIONS WITH X=%d==========\n", TEST_X);
  for(index = 0; index < NUMBER_OF_TEST_MONOMIALS; index++) {
    monomials_errno = MONOMIAL_SUCCESS;

    long double computed = monomial_compute(monomials[index], TEST_X);
    printf("P%d(%d) = %Lf\n", index, TEST_X, computed);

    dump_monomials_errno();
  }

  printf("\n==========DERIVATIVES==========\n");
  for(index = 0; index < NUMBER_OF_TEST_MONOMIALS; index++) {
    monomials_errno = MONOMIAL_SUCCESS;

    monomial_print(monomials[index]);
    printf("' = ");
    Monomial *derivative = monomial_derivative(monomials[index]);
    monomial_print(derivative);
    printf("\n");
    monomial_free(&derivative);

    dump_monomials_errno();
  }

  printf("\n==========SUMS==========\n");
  for(index = 0; index < NUMBER_OF_TEST_MONOMIALS / 2; index++) {
    monomials_errno = MONOMIAL_SUCCESS;

    monomial_print(monomials[index]);
    printf(" + ");
    monomial_print(monomials[index + (NUMBER_OF_TEST_MONOMIALS / 2)]);

    Monomial *sum = monomial_sum(monomials[index], monomials[index + (NUMBER_OF_TEST_MONOMIALS / 2)]);
    printf(" = ");
    monomial_print(sum);
    monomial_free(&sum);
    printf("\n");

    dump_monomials_errno();
  }

  printf("\n==========PRODUCTS==========\n");
  for(index = 0; index < (NUMBER_OF_TEST_MONOMIALS - 1); index++) {
    monomials_errno = MONOMIAL_SUCCESS;

    monomial_print(monomials[index]);
    printf(" * ");
    monomial_print(monomials[index + 1]);

    Monomial *product = monomial_product(monomials[index], monomials[index + 1]);
    printf(" = ");
    monomial_print(product);
    monomial_free(&product);
    printf("\n");

    dump_monomials_errno();
  }

  for(index = 0; index < NUMBER_OF_TEST_MONOMIALS; index++) {
    monomial_free(&(monomials[index]));
  }

  printf("\n==========CREATE FROM STRINGS==========\n");
  char *strings[25] = { // a list of legal cases
    "2",
    "-2",
    " - 2 ",
    "+2",
    " + 2 ",
    "2x",
    "-2x",
    " - 2x ",
    "+2x",
    " + 2x ",
    "2x^2",
    "-2x^2",
    " - 2x^2 ",
    "+2x^2",
    " + 2x^2 ",
    "x^2",
    "-x^2",
    " - x^2 ",
    "+x^2",
    " + x^2 ",
    "x",
    "-x",
    " - x ",
    "+x",
    " + x "
  };

  for(index = 0; index < 25; index++) {
    monomials_errno = MONOMIAL_SUCCESS;

    char *end = strings[index];
    printf("Reading from '%s' -> ", strings[index]);
    Monomial *new = monomial_create_from_string(strings[index], &end);

    monomial_print(new);
    printf("\n");
    monomial_free(&new);

    dump_monomials_errno();
  }
}

