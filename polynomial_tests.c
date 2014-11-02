
#include <stdio.h>
#include <stdlib.h>
#include "Polynomial.h"

#define NUMBER_OF_TEST_POLYNOMIALS 7
#define TEST_X 3
#define TEST_POWER 2

static void dump_polynomials_errno(void) {
  switch(polynomials_errno) {
    case POLYNOMIAL_MATH_ERROR:
      puts("MONOMIAL_MATH_ERROR");
      break;

    case POLYNOMIAL_INPUT_ERROR:
      puts("MONOMIAL_INPUT_ERROR");
      break;

    default: break;
  }
}

void polynomial_tests_run(void) {

  printf("\n==========CREATE FROM STRINGS==========\n");
  char *strings[NUMBER_OF_TEST_POLYNOMIALS - 1] = { // a list of legal cases
    "2 + 5x - 7x^2",
    "-15x^3",
    " - 6 - 6x - 6x^2 - 6x^3 - 6x^4 ",
    "+2x - 11 + 0x^12",
    " + 1 + 1x + x^4 ",
    "1 + x + x^2 + x^3 + x^4 + x^5 + x^6"
  };

  Polynomial *polynomials[NUMBER_OF_TEST_POLYNOMIALS];

  int index = 0;
  for(index = 0; index < NUMBER_OF_TEST_POLYNOMIALS - 1; index++) {
    polynomials_errno = POLYNOMIAL_SUCCESS;

    printf("P%d Reading from '%s' -> ", index, strings[index]);
    polynomials[index] = polynomial_create_from_string(strings[index]);

    polynomial_print(polynomials[index], 1);

    dump_polynomials_errno();
  }

  printf("\n==========CREATE FROM USER INPUT==========\n");
  printf("Please input a polynomial: ");
  polynomials[NUMBER_OF_TEST_POLYNOMIALS - 1] = polynomial_create_from_stdin();

  if(!polynomials[NUMBER_OF_TEST_POLYNOMIALS - 1]) {
    fprintf(stderr, "Fatal error: you cannot input a null polynomial!\n");
    return;
  }

  printf("P%d = ", NUMBER_OF_TEST_POLYNOMIALS - 1);
  polynomial_print(polynomials[index], 1);

  printf("\n==========COMPUTATIONS WITH X=%d==========\n", TEST_X);
  for(index = 0; index < NUMBER_OF_TEST_POLYNOMIALS; index++) {
    polynomials_errno = POLYNOMIAL_SUCCESS;

    long double computed = polynomial_compute(polynomials[index], TEST_X);
    printf("P%d(%d) = %Lf\n", index, TEST_X, computed);

    dump_polynomials_errno();
  }

  printf("\n==========DERIVATIVES==========\n");
  for(index = 0; index < NUMBER_OF_TEST_POLYNOMIALS; index++) {
    polynomials_errno = POLYNOMIAL_SUCCESS;

    printf("P%d' = ", index);
    Polynomial *derivative = polynomial_derivative(polynomials[index]);
    polynomial_print(derivative, 1);
    polynomial_free(&derivative);

    dump_polynomials_errno();
  }

  printf("\n==========SUMS==========\n");
  for(index = 0; index < NUMBER_OF_TEST_POLYNOMIALS / 2; index++) {
    polynomials_errno = POLYNOMIAL_SUCCESS;

    printf("P%d + P%d = ", index, (index + (NUMBER_OF_TEST_POLYNOMIALS / 2)));

    Polynomial *sum = polynomial_sum(polynomials[index], polynomials[index + (NUMBER_OF_TEST_POLYNOMIALS / 2)]);
    polynomial_print(sum, 1);
    polynomial_free(&sum);

    dump_polynomials_errno();
  }

  printf("\n==========PRODUCTS==========\n");
  for(index = 0; index < NUMBER_OF_TEST_POLYNOMIALS / 2; index++) {
    polynomials_errno = POLYNOMIAL_SUCCESS;

    printf("P%d * P%d = ", index, (index + (NUMBER_OF_TEST_POLYNOMIALS / 2)));

    Polynomial *product = polynomial_product(polynomials[index], polynomials[index + (NUMBER_OF_TEST_POLYNOMIALS / 2)]);
    polynomial_print(product, 1);
    polynomial_free(&product);

    dump_polynomials_errno();
  }

  printf("\n==========POWERS of %d==========\n", TEST_POWER);
  for(index = 0; index < NUMBER_OF_TEST_POLYNOMIALS; index++) {
    Polynomial *powered = polynomial_power(polynomials[index], TEST_POWER);

    printf("P%d^%d = ", index, TEST_POWER);
    polynomial_print(powered, 1);
    polynomial_free(&powered);
  }

  printf("\n==========WRITING TO FILE==========\n");
  printf(
    polynomial_write_to_file((const Polynomial **)polynomials, NUMBER_OF_TEST_POLYNOMIALS, "saved.txt") ?
    "successful\n" : "failure\n"
  );

  for(index = 0; index < NUMBER_OF_TEST_POLYNOMIALS; index++) {
    polynomial_free(&(polynomials[index]));
  }

  printf("\n==========READING FROM FILE==========\n");
  int file_number_of_polynomials = 0;
  Polynomial **file_polynomials = polynomial_create_from_file("saved.txt", &file_number_of_polynomials);
  if(!file_polynomials) {
    fprintf(stderr, "Error in file 'saved.txt'!\nExiting\n");
    exit(EXIT_FAILURE);
  }

  for(index = 0; index < file_number_of_polynomials; index++) {
    printf("P%d = ", index);
    polynomial_print(file_polynomials[index], 1);
    polynomial_free(&(file_polynomials[index]));
  }

  free(file_polynomials);
}
