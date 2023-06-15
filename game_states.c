#include "headers/game_states.h"
#include "headers/player_input_processing.h"
#include "headers/rendering.h"
#include "headers/move_validation.h"
#include "headers/matrix_ops.h"


extern const int SCREEN_WIDTH, SCREEN_HEIGHT;

void menu(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY], state_t* state_ptr,
        SDL_Rect* window_rectangle)
{   

    SDL_Rect new_game_btn_rec = {
        .x = 50,
        .y = 50,
        .w = 390,
        .h = 143
    };

    SDL_Rect settings_btn_rec = {
        .x = 50,
        .y = new_game_btn_rec.y + new_game_btn_rec.h + 20,
        .w = 390,
        .h = 143
    };

    SDL_Rect rankings_btn_rec = {
        .x = 50,
        .y = settings_btn_rec.y + settings_btn_rec.h + 20,
        .w = 390,
        .h = 143
    };

    SDL_Rect exit_btn_rec = {
        .x = 50,
        .y = rankings_btn_rec.y + rankings_btn_rec.h + 20,
        .w = 390,
        .h = 143
    };

    button_t new_game_btn_obj = {
        .rect = new_game_btn_rec,
        .st_event = game_set_st,
        .txt_enum = new_game_btn
    };

    button_t settings_btn_obj = {
        .rect = settings_btn_rec,
        .st_event = settings_st,
        .txt_enum = settings_btn
    };

    button_t rankings_btn_obj = {
        .rect = rankings_btn_rec,
        .st_event = rankings_st,
        .txt_enum = rankings_btn 
    };
    
    button_t exit_btn_obj = {
        .rect = exit_btn_rec,
        .st_event = exit_st,
        .txt_enum = exit_btn 
    };

    button_t* button_ptrs[4] = {
        &new_game_btn_obj,
        &settings_btn_obj,
        &rankings_btn_obj,
        &exit_btn_obj
    };

    /* Empezamos a procesar eventos con la variable event */
    SDL_Event event;
    //SDL_Point mouse_position;
    //SDL_GetMouseState(&mouse_position.x, &mouse_position.y);

    /* SDL_PollEvent retorna 0 si no hay eventos disponibles, si no, retorna 1. */
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            /* El usuario pide salir del juego */
            case SDL_QUIT:
                *state_ptr = exit_st; 
                break;
            /* Se registra un click izquierdo down del usuario */
            case SDL_MOUSEBUTTONDOWN: 
                SDL_MouseButtonEvent* mouse_event = &event.button;
                check_menu_btn_press(button_ptrs, mouse_event, state_ptr);
                break;
            /* Caso default, por buena onda */
            default:
                break;
        }
    }


    /* Limpiar y dibujar a la pantalla */
    SDL_RenderClear(renderer);

    /* Renderizamos el fondo del menu */ 
    SDL_RenderCopy(renderer, textures[menu_bck], NULL, window_rectangle);

    /* Renderizamos los botones del menu */
    render_menu_buttons(renderer, textures, button_ptrs);
    
    SDL_RenderPresent(renderer);
}


void game_set(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY], state_t* state_ptr,
        SDL_Rect* window_rectangle, toggle_button_group_t* board_size_btns_ptr,
            game_stats_t* game_stats_ptr, int game_arr[19][19], int prev_game_arr[19][19]) 
{

    /* Creamos rectángulo para el boton de comienzo en gameset */
    SDL_Rect start_btn_rec;
    start_btn_rec.w = 410;
    start_btn_rec.h = 121;
    start_btn_rec.x = (SCREEN_WIDTH - start_btn_rec.w) / 2; /* Centrado en eje x */
    start_btn_rec.y = SCREEN_HEIGHT - start_btn_rec.h - 50; /* Padding de 50 con inferior */

    button_t start_btn_obj = {
        .rect = start_btn_rec,
        .st_event = game_st,
        .txt_enum = start_btn
    };

    /* Empezamos a procesar eventos con la variable event */
    SDL_Event event;
    // SDL_Point mouse_position;
    // SDL_GetMouseState(&mouse_position.x, &mouse_position.y);

    /* SDL_PollEvent retorna 0 si no hay eventos disponibles, si no, retorna 1. */
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            /* El usuario pide salir del juego */
            case SDL_QUIT:
                *state_ptr = exit_st; 
                break;
            /* Se registra un click izquierdo down del usuario */
            case SDL_MOUSEBUTTONDOWN: 
                SDL_MouseButtonEvent* mouse_event = &event.button;
                if (check_game_set_btn_press(&start_btn_obj, board_size_btns_ptr, mouse_event)) {

                    /* Reseteamos los game_stats */
                    *game_stats_ptr = init_game_stats();

                    /* Cambiamos el tamaño del tablero al indicado en game_stats */
                    game_stats_ptr->len = board_size_btns_ptr->val;
  
                    /* Dejamos los tablero en cero */
                    memset(game_arr, 0, sizeof(int) * 19 * 19);
                    memset(prev_game_arr, 0, sizeof(int) * 19 * 19);

                    /* Empezamos la partida cambiando el estado del juego */
                    *state_ptr = game_st;
                }
                break;
            case SDL_KEYDOWN:
                SDL_KeyboardEvent* keyboard_event = &event.key;
                if (keyboard_event->keysym.sym == SDLK_ESCAPE) { 
                    *state_ptr = menu_st;
                }
            /* Caso default, por buena onda */
            default:
                break;
        }
    }

    /* Renderizamos los botones del menu */
    render_game_set_buttons(renderer, textures, &start_btn_obj, board_size_btns_ptr,
            window_rectangle);
}

void play(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY], int game_arr[19][19],
        state_t* state_ptr, SDL_Rect* window_rectangle, game_stats_t* game_stats_ptr,
            int prev_game_arr[19][19])
{
 
    /* Empezamos a procesar eventos con la variable event */
    SDL_Event event;
    SDL_Point mouse_position;
    SDL_GetMouseState(&mouse_position.x, &mouse_position.y);

    /* SDL_PollEvent retorna 0 si no hay eventos disponibles, si no, retorna 1. */
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            /* El usuario pide salir del juego */
            case SDL_QUIT:
                *state_ptr = exit_st; 
                break;
            /* Se registra un click izquierdo down del usuario */
            case SDL_MOUSEBUTTONDOWN: 
                SDL_MouseButtonEvent* mouse_event = &event.button;
                //printf("mouse click x = %i y = %i\n", mouse_event->x,mouse_event->y);
                if (process_move(game_stats_ptr->len, game_arr, mouse_event, game_stats_ptr->player,
                            prev_game_arr)) {
                    game_stats_ptr->player = game_stats_ptr->player % 2 + 1;
                }
                break;
            case SDL_KEYDOWN:
                SDL_KeyboardEvent* keyboard_event = &event.key;
                if (keyboard_event->keysym.sym == SDLK_ESCAPE) { 
                    *state_ptr = menu_st;
                }
                break;
            /* Caso default, por buena onda */
            default:
                break;
        }
    }

    /* Renderizamos toodooooo */
    render_game_state(game_stats_ptr->len, game_arr, renderer, textures, window_rectangle); 
}

