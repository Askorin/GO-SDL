#ifndef PLAYER_INPUT_PROCESSING_H
#define PLAYER_INPUT_PROCESSING_H
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "enums.h" 
#include "buttons.h"
#include "struct_objects.h"

/* Maneja click izquierdo en estado de juego, returna true si es que se debe pasar el turno. */
bool check_mdown(game_stats_t*, int[19][19], int[19][19], SDL_MouseButtonEvent*, button_t*[4],
        state_t*, button_t*[3], bool*, SDL_Rect*, bool);

/* 
 * Mapea un clickeo en el tablero a una columna y fila en matriz de juego, retorna verdadero si es
 * mapeable a un puesto vació.
 */
bool map_mdown_to_board(game_stats_t*, int*, int*,
        SDL_MouseButtonEvent*, int[19][19]);

/* Chequea cuál botón se presionó en el juego */
bool check_game_btn_press(button_t*[4], SDL_MouseButtonEvent*, int*);

/* Chequea cuál botón se presionó en el menú */
void check_menu_btn_press(button_t*[3], SDL_MouseButtonEvent*, state_t*);

/* Chequea cuál botón se presionó en el setting de partida, si se presionó start, se retorna true */
bool check_game_set_btn_press(button_t*, toggle_button_group_t*, SDL_MouseButtonEvent*, toggle_button_group_t*);

/* 
 * Chequea cuál botón se presionó en menu overlay, retorna false si se presiono fuera del overlay y
 * se debe cerrar.
 */
bool check_menu_overlay_btn_press(button_t*[3], SDL_MouseButtonEvent*, state_t*, SDL_Rect*);

/* Chequea si se clickeó en state save_game */
void check_save_game_mdown(game_stats_t*, int[19][19], int[19][19], SDL_MouseButtonEvent*,
        button_t*, state_t*, char*);

/* Chequea si se clickeó en state load_game */
bool check_load_game_mdown(game_stats_t*, int[19][19], int[19][19], SDL_MouseButtonEvent*,
        button_t*, state_t*, char*);

/* Chequea si se clickeó en state end_game */
void check_end_game_btn_press(button_t*, SDL_MouseButtonEvent*, state_t*);

#endif

