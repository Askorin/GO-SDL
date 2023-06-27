#ifndef RENDERING_H
#define RENDERING_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include "enums.h"
#include "struct_objects.h"
#include "buttons.h"

void render_board(int, SDL_Renderer*, SDL_Texture*[OBJ_QTY]);

/* Renderiza las piezas en base a la matriz de juego */
void render_game_state(game_stats_t*, int[19][19], SDL_Renderer*, SDL_Texture*[OBJ_QTY], SDL_Rect*, button_t*[4], bool*, button_t*[3], SDL_Rect*);

/* Renderiza los botones del menú principal */
void render_menu_buttons(SDL_Renderer*, SDL_Texture*[OBJ_QTY], button_t*[4]);

/* Renderiza los botones del estado de pre partida */
void render_game_set_buttons(SDL_Renderer*, SDL_Texture*[OBJ_QTY], button_t*,
        toggle_button_group_t*, SDL_Rect*);

/* Renderiza el UI del juego, esto es, los paneles y lo que contienen, no el tablero o piezas. */
void render_game_ui(SDL_Renderer*, SDL_Texture*[OBJ_QTY], button_t*[4], game_stats_t*); 

void render_overlay_menu(SDL_Renderer*, SDL_Texture*[OBJ_QTY], button_t*[3], SDL_Rect*); 

void render_save_game(SDL_Renderer*, SDL_Texture*[OBJ_QTY], SDL_Rect*, TTF_Font*, char*, int,
        button_t* save_btn_ptr);

void render_text(SDL_Renderer*, TTF_Font*, char*, SDL_Color, SDL_Rect*);

#endif

