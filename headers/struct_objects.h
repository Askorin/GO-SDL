#ifndef STRUCT_OBJECTS_H
#define STRUCT_OBJECTS_H
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "enums.h"

typedef struct {
    bool pass;
    int player;
    int black_caps;   /* La cantidad de capturas que ha hecho negro */
    int white_caps;   /* La cantidad de capturas que ha hecho blanco */
    int black_terr;   /* Territorio de las negras */
    int white_terr;   /* Territorio de las blancas */
    int len;          /* Tamaño del tablero, lenxlen */
    float komi;       /* Valor del komi */
} game_stats_t;

game_stats_t init_game_stats();

void print_game_stats(game_stats_t);

#endif
