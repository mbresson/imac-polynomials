
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "Monomial.h"
#include "Polynomial.h"

#define MAX_STDIN_BUFFER_SIZE 1000
#define MAX_POLYNOMIAL_DEGREE 50

POLYNOMIALS_ERRNO polynomials_errno;

struct Polynomial {
  Monomial *first;
  long degree;
};


static inline int is_coefficient_null(double coefficient) {
  // comparing a double to 0 may fail because of its internal representation
  return (coefficient > -0.0001 && coefficient < 0.0001);
}


static Polynomial* polynomial_create_empty(void) {
  Polynomial *new_polynomial = malloc(sizeof(Polynomial));
  if(!new_polynomial) {
    fprintf(stderr, "Fatal error: couldn't allocate %zu bytes!\nExiting\n", sizeof(Polynomial));
    exit(EXIT_FAILURE);
  }

  new_polynomial->first = NULL;
  new_polynomial->degree = 0;

  return new_polynomial;
}


static void polynomial_recalculate_degree(Polynomial *polynomial) {
  assert(polynomial != NULL);

  long degree = 0;
  Monomial *monomial = polynomial->first;
  while(monomial != NULL) {
    long new_degree = monomial_get_degree(monomial);
    if(new_degree > degree) {
      degree = new_degree;
    }
    monomial = monomial_get_next(monomial);
  }

  polynomial->degree = degree;
}


static void read_string_from_stdin(char buffer[MAX_STDIN_BUFFER_SIZE]) {
  if(fgets(buffer, MAX_STDIN_BUFFER_SIZE, stdin) != buffer) {
    fprintf(stderr, "Fatal error: your input is invalid!\nExiting\n");
    exit(EXIT_FAILURE);
  }

  // clean the buffer in case of an overflow
  char *newline = strrchr(buffer, '\n');
  if(!newline) {
    int c;
    while((c = getchar()) != '\n');

    fprintf(stderr, "Error: your input is too long!\nPlease shorten it up.\n");
    memset(buffer, 0, MAX_STDIN_BUFFER_SIZE);
    read_string_from_stdin(buffer);
  } else {
    if(*newline == buffer[0]) {
      // nothing was entered
      fprintf(stderr, "Error: please input something!\n");
      *newline = 0;
      read_string_from_stdin(buffer);
    }
    *newline = '\0';
  }
}


/*
 * @function polynomial_convert_to_array
 *
 * Create an array to store the polynomial's coefficients and degree,
 * sorted in ascending order.
 *
 * @param double *array
 * Its length MUST absolutely be the same as the polynomial's degree.
 */
static void polynomial_convert_to_array(const Polynomial *polynomial, double *array) {
  assert(polynomial != NULL);
  assert(polynomial->first != NULL);
  assert(array != NULL);

  Monomial *current = polynomial->first;

  while(current != NULL) {
    array[monomial_get_degree(current)] = monomial_get_coefficient(current);
    current = monomial_get_next(current);
  }
}


static inline long double polynomial_compute_method_horner(const Polynomial* polynomial, int x) {
  assert(polynomial != NULL);
  assert(polynomial->first != NULL);

  long degree = polynomial->degree;
  size_t size_coeff_array = sizeof(double) * (degree + 1);
  double *coeff_array = malloc(size_coeff_array);

  if(!coeff_array) {
    fprintf(stderr, "Fatal error: couldn't allocate %zu bytes!\nExiting\n", size_coeff_array);
    exit(EXIT_FAILURE);
  }

  int index_coeff_array = 0;
  while(index_coeff_array < (degree + 1)) {
    // fill the array with 0 to avoid any problem
    coeff_array[index_coeff_array] = 0;
    index_coeff_array++;
  }

  polynomial_convert_to_array(polynomial, coeff_array);

  errno = 0;

  long double result = coeff_array[degree];
  for(index_coeff_array = degree - 1; index_coeff_array >= 0; index_coeff_array--) {
    result = (result * x) + coeff_array[index_coeff_array];
  }

  free(coeff_array);

  return result;
}


long double polynomial_compute(const Polynomial* polynomial, int x) {
  return polynomial_compute_method_horner(polynomial, x);
}


Polynomial* polynomial_copy(const Polynomial* polynomial) {
  assert(polynomial != NULL);

  Polynomial *copy = polynomial_create_empty();

  Monomial *current = polynomial->first, *copy_current = NULL;

  while(current != NULL) {
    if(copy_current == NULL) {
      copy->first = monomial_copy(current);
      copy_current = copy->first;
    } else {
      Monomial *previous = copy_current;
      copy_current = monomial_copy(current);
      monomial_set_next(previous, copy_current);
    }

    current = monomial_get_next(current);
  }

  polynomial_recalculate_degree(copy);

  return copy;
}


Polynomial* polynomial_create(const double *coefficients, unsigned int degree) {
  assert(coefficients != NULL);

  Polynomial *new_polynomial = malloc(sizeof(Polynomial));
  if(!new_polynomial) {
    fprintf(stderr, "Fatal error: couldn't allocate %zu bytes!\nExiting\n", sizeof(Polynomial));
    exit(EXIT_FAILURE);
  }

  // polynomial_create({2., -4., 0, 3.}, 3) will create 2 -4x + 3x^3
  unsigned int index = 1;
  Monomial *current = monomial_create(coefficients[0], 0), *previous = NULL;
  new_polynomial->first = current;
  previous = current;

  for(; index < degree + 1; index++) {
    current = monomial_create(coefficients[index], index);
    if(index == 0) {
      new_polynomial->first = current;
    } else {
      monomial_set_next(previous, current);
    }

    previous = current;
  }

  new_polynomial->degree = (long) degree;

  return new_polynomial;
}


/*
 * @function polynomial_remove_null_monomials
 *
 * Remove all monomials from polynomial where coefficient is 0
 */
static void polynomial_remove_null_monomials(Polynomial *polynomial) {
  assert(polynomial != NULL);

  Monomial *current = polynomial->first, *previous = NULL;
  while(current != NULL) {
    double coefficient = monomial_get_coefficient(current);

    if(is_coefficient_null(coefficient)) {
      // remove this monomial
      if(current == polynomial->first) {
        polynomial->first = monomial_get_next(current);
        monomial_free(&current);
        current = polynomial->first;
      } else {
        Monomial *next = monomial_get_next(current);
        monomial_free(&current);
        current = next;
        monomial_set_next(previous, current);
      }

      continue;
    }

    previous = current;
    current = monomial_get_next(current);
  }

  polynomial_recalculate_degree(polynomial);
}


Polynomial** polynomial_create_from_file(const char* filename, int* length) {
  assert(filename != NULL);
  assert(length != NULL);

  errno = 0;
  FILE *file = fopen(filename, "r");
  if(!file) {
    polynomials_errno = POLYNOMIAL_INPUT_ERROR;
    return 0;
  }

  // read number of lines
  *length = 0;
  int character = ' ';
  while((character = fgetc(file)) != EOF) {
    if(character == '\n') {
      (*length)++;
    }
  }

  Polynomial **polynomials = malloc(sizeof(Polynomial) * *length);
  if(!polynomials) {
    fprintf(stderr, "Fatal error: couldn't allocate %zu bytes!\nExiting\n", sizeof(Polynomial) * (*length));
    exit(EXIT_FAILURE);
  }

  rewind(file);

  char line[MAX_STDIN_BUFFER_SIZE];

  int line_index = 0;
  for(; line_index < *length; line_index++) {
    if(fgets(line, MAX_STDIN_BUFFER_SIZE, file) != line) {
      fprintf(stderr, "Fatal error: file input is longer than maximum buffer size(%u)!\nExiting\n", MAX_STDIN_BUFFER_SIZE);
      exit(EXIT_FAILURE);
    }

    char *end_of_line = strrchr(line, '\n');
    if(end_of_line != NULL) {
      *end_of_line = '\0';
    }

    Polynomial *readp = polynomial_create_from_string(line);
    if(!readp) {
      continue;
    }

    polynomials[line_index] = readp;
  }

  fclose(file);

  return polynomials;
}


Polynomial* polynomial_create_from_stdin(void) {
  char input[MAX_STDIN_BUFFER_SIZE];

  read_string_from_stdin(input);

  return polynomial_create_from_string(input);
}


Polynomial* polynomial_create_from_string(char *string) {
  assert(string != NULL);

  // read a random number of numbers
  // example string: 7x^3 + x^2 -9x + 30
  // allowed characters: digits, +, -, x, ^

  Polynomial* new_polynomial = polynomial_create_empty();
  Monomial* previous_monomial = NULL;

  char *cursor = string;
  long max_monomial_degree = 0;
  int first_monomial = 1;

  while(*cursor != '\0') {
    monomials_errno = MONOMIAL_SUCCESS;

    char *tmp_cursor = cursor;
    Monomial *new_monomial = monomial_create_from_string(cursor, &tmp_cursor);
    switch(monomials_errno) {
      case MONOMIAL_INPUT_ERROR:
        fprintf(stderr, "Fatal error: malformatted input!\nExiting\n");
        exit(EXIT_FAILURE);

      default: break;
    }

    cursor = tmp_cursor;

    long new_degree = monomial_get_degree(new_monomial);
    if(max_monomial_degree < new_degree) {
      max_monomial_degree = new_degree;
    }

    if(first_monomial) {
      new_polynomial->first = new_monomial;
      first_monomial = 0;
    } else {
      monomial_set_next(previous_monomial, new_monomial);
    }

    previous_monomial = new_monomial;

    // skip spaces
    while(*cursor == ' ') {
      cursor++;
    }
  }

  new_polynomial->degree = max_monomial_degree;

  // remove null monomials which may be there (e.g. 0x^12)
  polynomial_remove_null_monomials(new_polynomial);

  if(!new_polynomial->first) {
    // empty polynomial
    polynomial_free(&new_polynomial);
    return NULL;
  }

  // reduct the polynomial (e.g. in case the user has input 2x - 6x)
  Polynomial *reducted = polynomial_reduct(new_polynomial);
  polynomial_free(&new_polynomial);

  if(!reducted->first) {
    // empty polynomial
    polynomial_free(&reducted);
    return NULL;
  }

  return reducted;
}


Polynomial* polynomial_derivative(const Polynomial *polynomial) {
  assert(polynomial != NULL);
  assert(polynomial->first != NULL);

  Polynomial* new_polynomial = polynomial_create_empty();
  new_polynomial->degree = polynomial->degree - 1;

  if(new_polynomial->degree < 0) {
    // a polynomial of degree < 0 is 0
    new_polynomial->degree = 0;
    new_polynomial->first = monomial_create(0, 0);

    return new_polynomial;
  }

  Monomial *cursor_oldp = polynomial->first, *cursor_newp = NULL;
  int first = 1;
  while(cursor_oldp != NULL) {
    // compute its derivative
    if(first) {
      Monomial *new_monomial = monomial_derivative(cursor_oldp);
      if(new_monomial != NULL) {
        // degree >= 0, because 2 derivated gives 0, no need to add it to the polynomial
        new_polynomial->first = new_monomial;
        cursor_newp = new_monomial;
        first = 0;
      }
    } else {
      Monomial *new_monomial = monomial_derivative(cursor_oldp);
      if(new_monomial != NULL) {
        // degree >= 0, because 2 derivated gives 0, no need to add it to the polynomial
        monomial_set_next(cursor_newp, new_monomial);
        cursor_newp = new_monomial;
      }
    }

    cursor_oldp = monomial_get_next(cursor_oldp);
  }

  polynomial_remove_null_monomials(new_polynomial);

  return new_polynomial;
}


void polynomial_free(Polynomial** polynomial) {
  assert(polynomial != NULL);
  assert(*polynomial != NULL);

  Monomial* current = (*polynomial)->first, *next = NULL;
  while(current != NULL) {
    next = monomial_get_next(current);
    monomial_free(&current);
    current = next;
  }

  free(*polynomial);
  *polynomial = NULL;
}


Polynomial* polynomial_power(const Polynomial *polynomial, int power) {
  assert(polynomial != NULL);
  assert(power > 0);

  Polynomial *result = polynomial_copy(polynomial);

  while(power > 1) {
    Polynomial *tmp = result;
    result = polynomial_product(result, polynomial);

    if(tmp != polynomial) {
      polynomial_free(&tmp);
    }
    power--;
  }

  return result;
}


void polynomial_print(const Polynomial* polynomial, int newline) {
  assert(polynomial != NULL);

  printf("(");
  Monomial *current = polynomial->first;
  while(current != NULL) {
    monomial_print(current);
    current = monomial_get_next(current);
    if(current != NULL) {
      printf(", ");
    }
  }

  printf(")");

  if(newline) {
    printf("\n");
  }
}


Polynomial* polynomial_product(const Polynomial* leftp, const Polynomial* rightp) {
  assert(leftp != NULL);
  assert(rightp != NULL);

  /*
   * Steps for the product of two polynomials:
   * - compute the degree of the product
   * - create an array to store the product
   * - compute the product by adding the product of each monomial in leftp with each monomial in rightp to the array
   * - convert the array back to a polynomial
   */

  // compute the degree of the product
  long result_degree = leftp->degree + rightp->degree;

  // create an array to store the product
  double *result_coefficients = malloc(sizeof(double) * (result_degree + 1));
  if(!result_coefficients) {
    fprintf(stderr, "Fatal error: couldn't allocate %zu bytes!\nExiting\n", sizeof(double) * (result_degree + 1));
    exit(EXIT_FAILURE);
  }

  int index_coefficient = 0;
  for(; index_coefficient < result_degree + 1; index_coefficient++) {
    result_coefficients[index_coefficient] = 0;
  }

  // compute the product by adding the product of each monomial in leftp with each monomial in rightp to the array
  Monomial *current = leftp->first;
  while(current != NULL) {
    Monomial *rightm = rightp->first;

    while(rightm != NULL) {
      Monomial *productm = monomial_product(current, rightm);

      long productm_degree = monomial_get_degree(productm);
      double productm_coefficient = monomial_get_coefficient(productm);

      result_coefficients[productm_degree] += productm_coefficient;

      monomial_free(&productm);

      rightm = monomial_get_next(rightm);
    }

    current = monomial_get_next(current);
  }

  Polynomial *product = polynomial_create(result_coefficients, result_degree);
  polynomial_remove_null_monomials(product);

  free(result_coefficients);

  return product;
}


Polynomial* polynomial_sum(const Polynomial* leftp, const Polynomial* rightp) {
  assert(leftp != NULL);
  assert(rightp != NULL);

  Polynomial *sum = polynomial_create_empty();

  sum->degree = (leftp->degree > rightp->degree) ? leftp->degree : rightp->degree;

  // store the coefficients of rightp and leftp in a bidimensional array for better convenience
  size_t size_coeff_array = sizeof(double) * (sum->degree + 1);

  double **coeff_array = malloc(sizeof(double*) * 2);
  if(!coeff_array) {
    fprintf(stderr, "Fatal error: couldn't allocate %zu bytes!\nExiting\n", sizeof(double*));
    exit(EXIT_FAILURE);
  }

  coeff_array[0] = malloc(size_coeff_array);
  if(!coeff_array[0]) {
    fprintf(stderr, "Fatal error: couldn't allocate %zu bytes!\nExiting\n", size_coeff_array);
    exit(EXIT_FAILURE);
  }

  coeff_array[1] = malloc(size_coeff_array);
  if(!coeff_array[1]) {
    fprintf(stderr, "Fatal error: couldn't allocate %zu bytes!\nExiting\n", size_coeff_array);
    exit(EXIT_FAILURE);
  }

  // initialize coefficients
  int index_coeff = 0;
  for(index_coeff = 0; index_coeff < sum->degree + 1; index_coeff++) {
    coeff_array[0][index_coeff] = 0;
  }

  for(index_coeff = 0; index_coeff < sum->degree + 1; index_coeff++) {
    coeff_array[1][index_coeff] = 0;
  }

  polynomial_convert_to_array(leftp, coeff_array[0]);
  polynomial_convert_to_array(rightp, coeff_array[1]);

  Monomial *current = NULL;
  for(index_coeff = 0; index_coeff < sum->degree + 1; index_coeff++) {
    Monomial *new_monomial = monomial_create(
      coeff_array[0][index_coeff] + coeff_array[1][index_coeff],
      index_coeff
    );

    if(index_coeff == 0) {
      sum->first = new_monomial;
    } else {
      monomial_set_next(current, new_monomial);
    }

    current = new_monomial;
  }

  polynomial_remove_null_monomials(sum);

  free(coeff_array[0]);
  free(coeff_array[1]);
  free(coeff_array);

  return sum;
}


Polynomial* polynomial_reduct(Polynomial* polynomial) {
  assert(polynomial != NULL);

  // create an array to store the coefficients
  double *coefficients = malloc(sizeof(double) * (polynomial->degree + 1));
  if(!coefficients) {
    fprintf(stderr, "Fatal error: couldn't allocate %zu bytes!\nExiting\n", sizeof(double) * (polynomial->degree + 1));
    exit(EXIT_FAILURE);
  }

  int index_coefficient = 0;
  for(; index_coefficient < polynomial->degree + 1; index_coefficient++) {
    coefficients[index_coefficient] = 0;
  }

  // compute the product by adding the product of each monomial in leftp with each monomial in rightp to the array
  Monomial *current = polynomial->first;
  while(current != NULL) {
    long degree = monomial_get_degree(current);
    double coefficient = monomial_get_coefficient(current);
    coefficients[degree] += coefficient;
    
    current = monomial_get_next(current);
  }

  Polynomial *reducted = polynomial_create(coefficients, polynomial->degree);
  polynomial_remove_null_monomials(reducted);

  free(coefficients);
  
  return reducted;
}


int polynomial_write_to_file(const Polynomial** polynomials, unsigned int length, const char* filename) {
  assert(polynomials != NULL);
  assert(*polynomials != NULL);
  assert(filename != NULL);

  errno = 0;
  FILE *file = fopen(filename, "w");
  if(!file) {
    polynomials_errno = POLYNOMIAL_OUTPUT_ERROR;
    return 0;
  }

  unsigned int index;
  for(index = 0; index < length; index++) {
    const Polynomial *currentp = polynomials[index];
    if(!currentp) {
      continue;
    }

    Monomial *monomial = currentp->first;
    while(monomial != NULL) {
      if(monomial != currentp->first) {
        if(monomial_get_coefficient(monomial) >= 0) {
          fprintf(file, "+ ");
        }
      }

      fprintf(file,
        "%.2lfx^%ld ",
        monomial_get_coefficient(monomial),
        monomial_get_degree(monomial)
      );

      monomial = monomial_get_next(monomial);
    }

    fprintf(file, "\n");
  }

  fclose(file);

  return 1;
}


