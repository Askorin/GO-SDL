#ifndef MOVE_VAL_H
#define MOVE_VAL_H
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "enums.h"

/* Procesa un movimiento. Retorna true si todo salió correctamente y debe jugar el otro jugador. */
bool process_move(int len, int[19][19], SDL_MouseButtonEvent*, int, int[19][19]);

/* Dado un tablero, una columna y una fila, chequea las libertades de la pieza en las coords */

int get_liberties(int len, int[19][19], int, int);

/* 
 * Chequea las coordenadas adyacentes de una fila y columna en el tablero, se podría decir que es
 * función hija de get_liberties, funciona de manera recursiva
 */
void check_adj(int len, int[19][19], int, int, unsigned int[12], unsigned int[12], int*);

/* Aplicamos una transformación para pasar de fila-columna a un entero. de 0 a lenxlen */
int coords_to_int(int, int, int);

/* Encuentra las piezas muertas (sin libertades) en el tablero */
void find_dead_pieces(int len, int[19][19]);

bool check_suicide(int len, int game_arr[19][19], int dummy_game_arr[19][19], int, int, int);

/* Recibe una matriz dos iteraciones anterior, y chequea que la jugada cumpla con la regla ko */
bool check_ko(int len, int[19][19], int[19][19]);

#endif
