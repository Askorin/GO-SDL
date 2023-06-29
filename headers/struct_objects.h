#ifndef STRUCT_OBJECTS_H
#define STRUCT_OBJECTS_H
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "enums.h"

typedef struct {
    bool opp;         /* Guarda false si no juega bot, true si juega bot basico. */
    bool pass;
    int player;
    int bot_player;    /* El jugador correspondiente al bot. */
    int black_caps;   /* La cantidad de capturas que ha hecho negro */
    int white_caps;   /* La cantidad de capturas que ha hecho blanco */
    int black_terr;   /* Territorio de las negras */
    int white_terr;   /* Territorio de las blancas */
    int len;          /* Tamaño del tablero, lenxlen */
    int resign;       /* Guarda el jugador que resignó, o un 0. Para uso en pantalla final */
    float komi;       /* Valor del komi */
} game_stats_t;

/* Inicializa el struct de game_stats con valores default. */
game_stats_t init_game_stats();

/* Imprime los game_stats */
void print_game_stats(game_stats_t);

/* Calcula total puntos negras */
float get_black_total(game_stats_t*);

/* Calcula total puntos blancas */
float get_white_total(game_stats_t*);



#endif
