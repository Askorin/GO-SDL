#ifndef MATRIX_OPS_H
#define MATRIX_OPS_H
#include <stdio.h>
#include <stdbool.h>

/* Compara dos matrices, útil en la regla del KO */
bool matrices_are_equal(int, int[19][19], int[19][19]);

/* Setea todos los valores de la matriz a un valor entero */
void set_matrix(int, int[19][19], int);

/* Copia una matriz desde source a target */
void copy_matrix(int, int[19][19], int[19][19]);

/* Imprime una matriz 19x19 */
void print_matrix(int, int[19][19]);

/* Cuenta la cantidad de piezas en la matriz, 1 es negro, 2 es blanco. */
void count_pieces(int, int[19][19], int[2]);

/* 
 * Toma dos matrices, un elemento n, recorre una matric source y si encuentra que esta tiene al
 * elemento n en una coordenada, y la matriz target no, entonces repara la diferencia en target. 
 */
void correct_diff(int, int, int[19][19], int[19][19]);

/* Verifica el caso base de la funcion flood_fill */
int is_valid_floodfill(int[19][19], int, int, int, int, int);

/*
 * Dada una matriz 19x19, cambia el valor antiguo de un area dentro de la matriz por uno nuevo, 
 * siendo (x,y) un punto dentro de dicha area
 */
void flood_fill(int[19][19], int, int, int, int, int);

/* Dada una matriz juego, modifica una matriz territorio */
void change_territory(int[19][19], int[19][19], int);

/* Guarda el territorio de una matriz de juego en un arreglo de 2 elementos */
void count_territory(int[19][19], int[2], int);
#endif
