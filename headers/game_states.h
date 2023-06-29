#ifndef GAME_STATES_H
#define GAME_STATES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include "enums.h"
#include "struct_objects.h"
#include "buttons.h"

/* Función de estado del menu */
void menu(SDL_Renderer*, SDL_Texture*[OBJ_QTY], state_t*, SDL_Rect*);

/* Función de estado de juego en tablero */
void play(SDL_Renderer*, SDL_Texture*[OBJ_QTY], int[19][19], state_t*, SDL_Rect*,
        game_stats_t*, int[19][19], bool*, TTF_Font*, unsigned int*);

/* 
 * Función previa al juego, elección de tamaño de tablero, settings, etc... Solo alcanzable desde
 * menu state
 */
void game_set(SDL_Renderer*, SDL_Texture*[OBJ_QTY], state_t*, SDL_Rect*, toggle_button_group_t*,
        game_stats_t*, int[19][19], int[19][19], bool*, toggle_button_group_t*, unsigned int*);

/* Función de guardado de pártidas, solo alcanzable desde game state */
void save_game(SDL_Renderer*, SDL_Texture*[OBJ_QTY], state_t*, SDL_Rect*, game_stats_t*,
        int[19][19], int[19][19], char**, int*, TTF_Font*);

/* Función de carga de pártidas, solo alcanzable desde menu state */
void load_game(SDL_Renderer*, SDL_Texture*[OBJ_QTY], state_t*, SDL_Rect*, game_stats_t*,
        int[19][19], int[19][19], char**, int*, TTF_Font*);

/* Función de terminado de partida, solo alcanzable al terminar una partida. */
void end_game(SDL_Renderer*, SDL_Texture*[OBJ_QTY], state_t*, SDL_Rect*, game_stats_t*, TTF_Font*);

#endif
