#ifndef BUTTONS_H 
#define BUTTONS_H 

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "enums.h"


/* Objeto para uso en botones, guarda un rectangulo y el state al que debe llevar */
typedef struct {
    SDL_Rect rect;
    state_t st_event;
    sf_and_txt_enum_t txt_enum;
    bool enabled;
} button_t;

/* Botón tipo toggle, tiene un valor asociado, esto era conveniente para el uso en game_set */
typedef struct {
    SDL_Rect rect;
    bool toggle;
    sf_and_txt_enum_t txt_enum;
    int val;
} toggle_button_t;

/* 
 * Definimos un grupo de toggle_buttons asociados, con selección exclusiva, es decir, solo puede
 * estar uno en toggle = true en cada instante 
 */
typedef struct {
    int len;
    toggle_button_t** toggle_button_ptrs;
    int val;
} toggle_button_group_t;

/* Inicializa un botón. */
button_t init_button(SDL_Rect, state_t, sf_and_txt_enum_t, bool);

/* Incializa un grupo de toggle_buttons con selección exclusiva */
toggle_button_group_t init_toggle_btn_grp(int, toggle_button_t**, int);

/* Presiona un botón de un toggle_button group, retorna true en caso de ser hecho exitosamente */
bool press_toggle_btn(toggle_button_group_t*, int);

#endif


