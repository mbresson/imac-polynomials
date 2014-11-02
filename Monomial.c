
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Monomial.h"

MONOMIALS_ERRNO monomials_errno;

struct Monomial {
  double coefficient;
  long degree;
  Monomial *next;
};


/*
 * Try to read a double in string.
 * If successful, sets *end to the next character after the number's digits.
 * If failed, sets *end to string and returns 0.
 */
static double read_double_from_string(char *string, char **end) {
  assert(string != NULL);
  assert(end != NULL);

  char *cursor = string;

  while(*cursor == ' ') {
    cursor++;
  }

  if(!isdigit(*cursor)) {
    *end = string;
    return 0;
  }

  /*
   * we must remove x from the string if present, before calling strtod
   * or else, strtod will infer from it that it must read an hexadecimal number
   */
  char *xcursor = cursor;
  while(*xcursor == '.' || isdigit(*xcursor)) {
    xcursor++;
  }

  if(*xcursor == 'x') {
    // create a copy of the string
    size_t copy_length = strlen(cursor) + 1;
    char *copy = malloc(sizeof(char) * copy_length);
    if(!copy) {
      fprintf(stderr, "Fatal error: couldn't allocate %zu bytes!\nExiting\n", sizeof(char) * copy_length);
      exit(EXIT_FAILURE);
    }

    strcpy(copy, cursor);
    char *x_copycursor = strchr(copy, 'x');

    *x_copycursor = ' ';
    
    // now we can read safely our double
    char *copy_end = copy;
    double val = strtod(copy, &copy_end);
    if(*end == copy) {
      *end = string;
      free(copy);
      return 0;
    }

    // move end to the x
    *end = cursor + (copy_end - copy);

    free(copy);
    return val;
  } else {
    double val = strtod(cursor, end);
    if(*end == cursor) {
      *end = string;
      return 0;
    }

    return val;
  }
}


long double monomial_compute(const Monomial* monomial, int x) {
  assert(monomial != NULL);

  errno = 0;
  long double result = monomial->coefficient;

  result *= pow((double) x, monomial->degree);

  if(errno != 0) {
    monomials_errno = MONOMIAL_MATH_ERROR;
    return 0;
  }

  return result;
}


Monomial* monomial_copy(const Monomial* monomial) {
  assert(monomial != NULL);

  return monomial_create(monomial->coefficient, monomial->degree);
}


Monomial* monomial_create(double coefficient, unsigned int degree) {
  Monomial *new_monomial = malloc(sizeof(Monomial));
  if(!new_monomial) {
    fprintf(stderr, "Fatal error: couldn't allocate %zu bytes!\nExiting\n", sizeof(Monomial));
    exit(EXIT_FAILURE);
  }

  new_monomial->coefficient = coefficient;
  new_monomial->degree = degree;
  new_monomial->next = NULL;

  return new_monomial;
}


Monomial* monomial_create_from_string(char* string, char** end) {
  assert(string != NULL);
  assert(end != NULL);

  /*
   * Possible formats for a monomial:
   * 2, 2x, 2x^2, -2, - 2, - 2x, etc
   *
   * Steps:
   * 1 optionally read the sign, (+2, -2, but 2 is possible)
   * 2 optionally read the coefficient, (2x, but x alone is possible)
   * then, read nothing (return) or
   * 3 read x (2x, but 2 alone is possible)
   * then, read nothing (return) or
   * 4 read ^ followed by an exponent number (2x^3, but 2x alone is possible)
   */

  char sign = '+';
  double coefficient = 0;
  int degree = 0;

  int coeff_read = 0;

  char *cursor = string;

  // skip spaces
  while(*cursor == ' ') {
    cursor++;
  }

  // 1 optionally read the sign,
  if(*cursor == '+' || *cursor == '-') {
    sign = *cursor;
    cursor++;
  }

  while(*cursor == ' ') {
    cursor++;
  }

  // 2 read the coefficient,
  if(isdigit(*cursor) != 0) {
    // read the coefficient

    char *converter_end = cursor;
    errno = 0;
    double val = read_double_from_string(cursor, &converter_end);
    if(converter_end == cursor || errno != 0) {
      // illegal situation: the following characters are somewhat not a number's
      // or errno contains ERANGE
      monomials_errno = MONOMIAL_INPUT_ERROR;
      *end = string;
      return NULL;
    }

    coefficient = val;
    cursor = converter_end;
    coeff_read = 1;
  }

  // 3 read x
  if(*cursor == 'x') {
    if(!coeff_read) {
      coefficient = 1; // x = 1x
    }

    cursor++;

    if(*cursor == '^') {
      cursor++;

      // 4 read ^ followed by an exponent number
      if(!isdigit(*cursor)) {

        // illegal situation: ^ must be followed by a number
        monomials_errno = MONOMIAL_INPUT_ERROR;
        *end = string;
        return NULL;

      } else {

        errno = 0;
        char *converter_end = cursor;
        long val = strtol(cursor, &converter_end, 10);
        if(converter_end == cursor || errno != 0) {
          // illegal situation: the following characters are somewhat not a number's
          // or errno is set to ERANGE
          monomials_errno = MONOMIAL_INPUT_ERROR;
          *end = string;
          return NULL;
        }

        if(val < 0) {
          // illegal situation: the exponent cannot be < 0
          monomials_errno = MONOMIAL_INPUT_ERROR;
          *end = string;
          return NULL;
        }

        cursor = converter_end;

        degree = val;
      }
    } else { // no ^ was read
      if(*cursor != ' ' && *cursor != '\0') {
        // illegal situation : we should have a space or a ^ after x
        monomials_errno = MONOMIAL_INPUT_ERROR;
        *end = string;
        return NULL;
      }

      degree = 1;
    }
  } else { // no x was read
    if(!coeff_read) {
      // illegal situation: we have no coefficient at all
      monomials_errno = MONOMIAL_INPUT_ERROR;
      *end = string;
      return NULL;
    } else {
      *end = string;
      if(*cursor == '\0' || *cursor == ' ') {
        degree = 0;
      } else {
        // illegal situation: we have no coefficient at all
        monomials_errno = MONOMIAL_INPUT_ERROR;
        *end = string;
        return NULL;
      }
    }
  }

  if(sign == '-') {
    coefficient = -coefficient;
  }

  *end = cursor;

  return monomial_create(coefficient, degree);
}


Monomial* monomial_derivative(const Monomial *monomial) {
  assert(monomial != NULL);

  double coefficient = monomial->coefficient * monomial->degree;
  int degree = monomial->degree - 1;

  if(degree < 0) {
    return monomial_create(0, 0);
  } else {
    return monomial_create(coefficient, degree);
  }
}


void monomial_free(Monomial** monomial) {
  assert(monomial != NULL);
  assert(*monomial != NULL);

  free(*monomial);
  *monomial = NULL;
}


double monomial_get_coefficient(Monomial *monomial) {
  assert(monomial != NULL);

  return monomial->coefficient;
}


long monomial_get_degree(Monomial *monomial) {
  assert(monomial != NULL);

  return monomial->degree;
}


Monomial* monomial_get_next(Monomial *monomial) {
  assert(monomial != NULL);

  return monomial->next;
}


void monomial_print(const Monomial* monomial) {
  assert(monomial != NULL);

  printf("(%.2lf, %ld)", monomial->coefficient, monomial->degree);
}


Monomial* monomial_product(const Monomial* leftm, const Monomial* rightm) {
  assert(leftm != NULL);
  assert(rightm != NULL);

  errno = 0;
  double coefficient = leftm->coefficient * rightm->coefficient;
  int degree = leftm->degree + rightm->degree;

  if(errno != 0) {
    monomials_errno = MONOMIAL_MATH_ERROR;
  }

  return monomial_create(coefficient, degree);
}


void monomial_set_next(Monomial *monomial, Monomial *next) {
  assert(monomial != NULL);

  monomial->next = next;
}


Monomial* monomial_sum(const Monomial* leftm, const Monomial* rightm) {
  assert(leftm != NULL);
  assert(rightm != NULL);

  errno = 0;
  if(leftm->degree != rightm->degree) {
    monomials_errno = MONOMIAL_ILLEGAL_OPERATION;
  }

  double coefficient = leftm->coefficient + rightm->coefficient;
  int degree = leftm->degree;

  if(errno != 0) {
    monomials_errno = MONOMIAL_MATH_ERROR;
  }

  return monomial_create(coefficient, degree);
}


