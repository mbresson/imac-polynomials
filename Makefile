CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = -lm
TARGET = main
OBJECTS = main.o polynomial_tests.o monomial_tests.o Polynomial.o Monomial.o

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(LDFLAGS) $+

%.o: %.c
	$(CC) -c $< $(CFLAGS)

clean:
	rm -Rf $(OBJECTS)

mrproper: clean
	rm -Rf $(TARGET)

