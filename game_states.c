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

    button_t new_game_btn_obj = init_button(new_game_btn_rec, game_set_st, new_game_btn, true);
    button_t settings_btn_obj = init_button(settings_btn_rec, settings_st, settings_btn, true);
    button_t rankings_btn_obj = init_button(rankings_btn_rec, rankings_st, rankings_btn, true);
    button_t exit_btn_obj = init_button(exit_btn_rec, exit_st, exit_btn, true);
    
    button_t* button_ptrs[4] = {
        &new_game_btn_obj,
        &settings_btn_obj,
        &rankings_btn_obj,
        &exit_btn_obj
    };

    /* Empezamos a procesar eventos con la variable event */
    SDL_Event event;

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
            game_stats_t* game_stats_ptr, int game_arr[19][19], int prev_game_arr[19][19],
            bool* overlay_menu_ptr) 
{

    /* Creamos rectángulo para el boton de comienzo en gameset */
    SDL_Rect start_btn_rec;
    start_btn_rec.w = 410;
    start_btn_rec.h = 121;
    start_btn_rec.x = (SCREEN_WIDTH - start_btn_rec.w) / 2; /* Centrado en eje x */
    start_btn_rec.y = SCREEN_HEIGHT - start_btn_rec.h - 50; /* Padding de 50 con inferior */

    button_t start_btn_obj = init_button(start_btn_rec, game_st, start_btn, true);

    /* Empezamos a procesar eventos con la variable event */
    SDL_Event event;

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

                    /* Desactivamos overlay menu */
                    *overlay_menu_ptr = false;

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
            int prev_game_arr[19][19], bool* overlay_menu_ptr)
{

    /* Ancho de los paneles */
    //int panel_width = (SCREEN_WIDTH - SCREEN_HEIGHT) / 2;

    SDL_Rect resign_btn_rect_blk = {
        .x = 20,
        .w = 240,
        .h = 70
    };  
    resign_btn_rect_blk.y = SCREEN_HEIGHT - resign_btn_rect_blk.h - 20;


    SDL_Rect resign_btn_rect_wht = {
        .x = SCREEN_WIDTH - (SCREEN_WIDTH - SCREEN_HEIGHT ) / 2 + 20,
        .y = resign_btn_rect_blk.y,
        .w = 240,
        .h = 70
    };

    SDL_Rect pass_btn_rect_wht = resign_btn_rect_wht;
    pass_btn_rect_wht.y -= resign_btn_rect_wht.h + 20;
    SDL_Rect pass_btn_rect_blk = resign_btn_rect_blk;
    pass_btn_rect_blk.y -= resign_btn_rect_blk.h + 20;

    button_t resign_btn_blk = init_button(resign_btn_rect_blk, end_game_st, resign_btn, true);
    button_t resign_btn_wht = init_button(resign_btn_rect_wht, end_game_st, resign_btn, true);
    button_t pass_btn_blk = init_button(pass_btn_rect_blk, -1, pass_btn, true); 
    button_t pass_btn_wht = init_button(pass_btn_rect_wht, -1, pass_btn, true); 

    if (game_stats_ptr->player == 1) {
        pass_btn_wht.enabled = false;
        resign_btn_wht.enabled = false;
    } else {
        pass_btn_blk.enabled = false;
        resign_btn_blk.enabled = false;
    }

    SDL_Rect main_menu_btn_rect = {
        .x = 20,
        .y = 20*2 + 5,
        .w = 240,
        .h = 70
    };
        

        
    
    button_t* button_ptrs[4] = {&pass_btn_blk, &pass_btn_wht, &resign_btn_blk, &resign_btn_wht};


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
                if (check_mdown(game_stats_ptr, game_arr, prev_game_arr, mouse_event,
                            button_ptrs, state_ptr)) {
                    game_stats_ptr->player = game_stats_ptr->player % 2 + 1;
                }
                break;
            case SDL_KEYDOWN:
                SDL_KeyboardEvent* keyboard_event = &event.key;
                if (keyboard_event->keysym.sym == SDLK_ESCAPE) { 
                    *overlay_menu_ptr = !*overlay_menu_ptr;
                    //*state_ptr = menu_st;
                }
                break;
            /* Caso default, por buena onda */
            default:
                break;
        }
    }

    /* Renderizamos toodooooo */
    render_game_state(game_stats_ptr, game_arr, renderer, textures, window_rectangle,
            button_ptrs, overlay_menu_ptr); 
    //if (*overlay_menu_ptr)
}

