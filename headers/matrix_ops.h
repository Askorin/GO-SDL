#ifndef MATRIX_OPS_H
#define MATRIX_OPS_H
#include <stdio.h>
#include <stdbool.h>

/* Compara dos matrices, útil en la regla del KO */
bool matrices_are_equal(int len, int[19][19], int[19][19]);

/* Setea todos los valores de la matriz a un valor entero */
void set_matrix(int len, int[19][19], int);

/* Copia una matriz desde source a target */
void copy_matrix(int len, int[19][19], int[19][19]);

void print_matrix(int len, int[19][19]);

#endif
