#ifndef STRUCT_OBJECTS_H
#define STRUCT_OBJECTS_H
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "enums.h"


/* Objeto para uso en botones, guarda un rectangulo y el state al que debe llevar */
typedef struct {
    SDL_Rect rect;
    state_t st_event;
} button_t;

typedef struct {
    int player;
    bool pass;
    int black_caps; /* La cantidad de capturas que ha hecho negro */
    int white_caps; /* La cantidad de capturas que ha hecho blanco */
} game_stats_t;

game_stats_t init_game_stats();

void print_game_stats(game_stats_t);

#endif
