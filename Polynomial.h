
#ifndef H_POLYNOMIAL
#define H_POLYNOMIAL

typedef struct Polynomial Polynomial;


typedef enum {
  POLYNOMIAL_SUCCESS,
  POLYNOMIAL_MATH_ERROR, // check errno for further information
  POLYNOMIAL_INPUT_ERROR,
  POLYNOMIAL_OUTPUT_ERROR
} POLYNOMIALS_ERRNO;

/*
 * This variable must be set to SUCCESS before calling a polynomial_* function.
 * polynomial_* functions may change its value to indicate an error.
 */
extern POLYNOMIALS_ERRNO polynomials_errno;


/*
 * @function polynomial_compute
 *
 * @return long double
 * The result of the computation of polynomial, with x given.
 */
extern long double polynomial_compute(const Polynomial* polynomial, int x);


/*
 * @function polynomial_copy
 */
extern Polynomial* polynomial_copy(const Polynomial* polynomial);


/*
 * @function polynomial_create
 *
 * @param double coefficients[]
 * An array containing the coefficients of the polynomial, ending at degree 0.
 *
 * @param unsigned int degree
 * The degree of the polynomial. Must correspond to the length of the array coefficients - 1.
 *
 * @return Polynomial*
 * Must be freed with polynomial_free after use.
 *
 * @example
 * polynomial_create({2., -4., 0, 3.}, 3) will create 2 -4x + 3x^2
 */
extern Polynomial* polynomial_create(const double *coefficients, unsigned int degree);


/*
 * @function polynomial_create_from_file
 *
 * @param int* length
 * This value will be set to the length of the array of polynomials read from the file and returned.
 *
 * @return Polynomial**
 * An array allocated on the heap, containing the polynomials read from the file.
 * The array and its elements must be freed after use.
 */
extern Polynomial** polynomial_create_from_file(const char* filename, int* length);


/*
 * @function polynomial_create_from_stdin
 *
 * @return Polynomial*
 * A polynomial read from stdin.
 * Must be freed with polynomial_free after use.
 *
 * @example
 * If stdin contains "7x^3 + x^2 -9x + 30", it will create polynomial 7x^3 + x^2 -9x + 30
 */
extern Polynomial* polynomial_create_from_stdin(void);


/*
 * @function polynomial_create_from_string
 *
 * @return Polynomial*
 * A polynomial read from the string.
 * Must be freed with polynomial_free after use.
 *
 * @example
 * If string contains "7x^3 + x^2 -9x + 30", it will create polynomial 7x^3 + x^2 -9x + 30
 */
extern Polynomial* polynomial_create_from_string(char *string);


/*
 * @function polynomial_derivative
 *
 * @return Polynomial*
 * The derivative of polynomial. Must be freed with polynomial_free after use.
 */
extern Polynomial* polynomial_derivative(const Polynomial *polynomial);


/*
 * @function polynomial_free
 *
 * Frees associated resources and sets *polynomial to NULL to prevent further use.
 */
extern void polynomial_free(Polynomial** polynomial);


/*
 * @function polynomial_power
 *
 * @return Polynomial*
 * The result of polynomial ^ power. Must be freed with polynomial_free after use.
 */
extern Polynomial* polynomial_power(const Polynomial *polynomial, int power);


/*
 * @function polynomial_print
 *
 * Prints polynomial to stdout.
 */
extern void polynomial_print(const Polynomial* polynomial, int newline);


/*
 * @function polynomial_product
 *
 * @return Polynomial*
 * The result of the product of leftp and rightp. Must be freed with polynomial_free after use.
 */
extern Polynomial* polynomial_product(const Polynomial* leftp, const Polynomial* rightp);


/*
 * @function polynomial_reduct
 *
 * @return Polynomial*
 * The reducted version of the polynomial. Must be freed with polynomial_free after use.
 * E.g. 2x + 4x -> 6x
 */
extern Polynomial* polynomial_reduct(Polynomial* polynomial);


/*
 * @function polynomial_sum
 *
 * @return Polynomial*
 * The result of the sum of leftp and rightp. Must be freed with polynomial_free after use.
 */
extern Polynomial* polynomial_sum(const Polynomial* leftp, const Polynomial* rightp);


/*
 * @function polynomial_write_to_file
 *
 * Write length polynomials to filename.
 *
 * @return int
 * 1 on success, 0 on failure.
 */
extern int polynomial_write_to_file(const Polynomial** polynomials, unsigned int length, const char* filename);


#endif

