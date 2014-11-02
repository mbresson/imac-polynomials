
#ifndef H_MONOMIAL
#define H_MONOMIAL

typedef struct Monomial Monomial;

typedef enum {
  MONOMIAL_SUCCESS,
  MONOMIAL_MATH_ERROR, // check errno for further information
  MONOMIAL_ILLEGAL_OPERATION, // example: trying to sum up 2x^2 and 3x^4
  MONOMIAL_INPUT_ERROR
} MONOMIALS_ERRNO;

/*
 * This variable must be set to SUCCESS before calling a monomial_* function.
 * monomial_* functions may change its value to indicate an error.
 */
extern MONOMIALS_ERRNO monomials_errno;


/*
 * @function monomial_compute
 *
 * @return long double
 * The result of the computation of monomial, with x given.
 */
extern long double monomial_compute(const Monomial* monomial, int x);


/*
 * @function monomial_copy
 */
extern Monomial* monomial_copy(const Monomial* monomial);


/*
 * @function monomial_create
 *
 * @param double coefficient
 * The coefficient of the monomial.
 *
 * @param int degree
 * The degree of the monomial.
 *
 * @return Monomial*
 * Must be freed with monomial_free after use.
 *
 * @example
 * monomial_create(2., 2) will create 2x^2
 */
extern Monomial* monomial_create(double coefficient, unsigned int degree);


/*
 * @function monomial_create_from_string
 *
 * @param const char* string
 * The string to read from.
 *
 * @param char** end
 * Before returning, it will be set to point after the last char of information.
 *
 * @return Monomial*
 * Must be freed with monomial_free after use.
 *
 * @example
 * monomial_create("+2x^3 - 5) will create 2x^3 and end will point to the space after 3
 */
extern Monomial* monomial_create_from_string(char* string, char** end);


/*
 * @function monomial_derivative
 *
 * @return Monomial*
 * The derivative of monomial. Must be freed with monomial_free after use.
 */
extern Monomial* monomial_derivative(const Monomial *monomial);


/*
 * @function monomial_free
 *
 * Frees associated resources and sets *monomial to NULL to prevent further use.
 */
extern void monomial_free(Monomial** monomial);


/*
 * @function monomial_get_coefficient
 */
extern double monomial_get_coefficient(Monomial *monomial);


/*
 * @function monomial_get_degree
 */
extern long monomial_get_degree(Monomial *monomial);


/*
 * @function monomial_get_next
 *
 * @return Monomial*
 * The next monomial.
 * This function doesn't create any new Monomial, thus the returned monomial needn't be freed lest it be freed twice.
 * It may return NULL if monomial has no next sibling.
 */
extern Monomial* monomial_get_next(Monomial *monomial);


/*
 * @function monomial_print
 *
 * Prints monomial to stdout without starting a new line.
 */
extern void monomial_print(const Monomial* monomial);


/*
 * @function monomial_product
 *
 * @return Monomial*
 * The result of the product of leftm and rightm. Must be freed with monomial_free after use.
 */
extern Monomial* monomial_product(const Monomial* leftm, const Monomial* rightm);


/*
 * @function monomial_set_next
 */
extern void monomial_set_next(Monomial *monomial, Monomial *next);


/*
 * @function monomial_sum
 *
 * @return Monomial*
 * The result of the sum of leftm and rightm. Must be freed with monomial_free after use.
 */
extern Monomial* monomial_sum(const Monomial* leftm, const Monomial* rightm);


#endif

