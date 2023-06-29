#include <string.h>
#include "headers/game_states.h"
#include "headers/player_input_processing.h"
#include "headers/bot.h"
#include "headers/rendering.h"
#include "headers/move_validation.h"
#include "headers/matrix_ops.h"


extern const int SCREEN_WIDTH, SCREEN_HEIGHT, PANEL_WIDTH, OVERLAY_MENU_WIDTH, PT_PANEL_WIDTH,
       PT_PANEL_HEIGHT, B_PAD;

void menu(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY], state_t* state_ptr,
        SDL_Rect* window_rectangle)
{   

    int menu_pad = (SCREEN_HEIGHT - 121 * 3) / 4.0;
    SDL_Rect new_game_btn_rec = {
        .x = (SCREEN_WIDTH - 361) / 2.0,
        .y = menu_pad, 
        .w = 361,
        .h = 121
    };

    SDL_Rect load_game_btn_rec = {
        .x = (SCREEN_WIDTH - 361) / 2.0,
        .y = new_game_btn_rec.y + new_game_btn_rec.h + menu_pad, 
        .w = 361,
        .h = 106
    };

    SDL_Rect exit_btn_rec = {
        .x = (SCREEN_WIDTH - 361) / 2.0,
        .y = load_game_btn_rec.y + load_game_btn_rec.h + menu_pad,
        .w = 361,
        .h = 121
    };

    button_t new_game_btn_obj = init_button(new_game_btn_rec, game_set_st, new_game_btn, true);
    button_t load_game_btn_obj = init_button(load_game_btn_rec, load_game_st, load_game_btn, true);
    button_t exit_btn_obj = init_button(exit_btn_rec, exit_st, exit_btn, true);
    
    button_t* button_ptrs[3] = {
        &new_game_btn_obj,
        &load_game_btn_obj,
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


    /* Esto para no renderizar el último frame en caso de que se cambie el estado */
    if (*state_ptr == menu_st) {
        /* Limpiar y dibujar a la pantalla */
        SDL_RenderClear(renderer);

        /* Renderizamos el fondo del menu */ 
        SDL_RenderCopy(renderer, textures[menu_bck], NULL, window_rectangle);

        /* Renderizamos los botones del menu */
        render_menu_buttons(renderer, textures, button_ptrs);
        
        SDL_RenderPresent(renderer);
    }
}


void game_set(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY], state_t* state_ptr,
        SDL_Rect* window_rectangle, toggle_button_group_t* board_size_btns_ptr,
            game_stats_t* game_stats_ptr, int game_arr[19][19], int prev_game_arr[19][19],
            bool* overlay_menu_ptr, toggle_button_group_t* opponent_btns_ptr, 
            unsigned int* prev_play_ms_ptr) 
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
                bool start_game = check_game_set_btn_press(&start_btn_obj, board_size_btns_ptr,
                        mouse_event, opponent_btns_ptr);
                if (start_game) {
                    /* Reseteamos los game_stats */
                    *game_stats_ptr = init_game_stats();

                    /* Cambiamos el estilo del oponente */
                    game_stats_ptr->opp = opponent_btns_ptr->val;
                    /* Cambiamos el tamaño del tablero al indicado en game_stats */
                    game_stats_ptr->len = board_size_btns_ptr->val;
  
                    /* Dejamos los tablero en cero */
                    memset(game_arr, 0, sizeof(int) * 19 * 19);
                    memset(prev_game_arr, 0, sizeof(int) * 19 * 19);

                    /* Reseteamos el tiempo de jugada previa */
                    *prev_play_ms_ptr = 0;

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
    
    /* Esto para no renderizar el último frame en caso de que se cambie el estado */
    if (*state_ptr == game_set_st) {
        /* Renderizamos los botones del menu */
        render_game_set(renderer, textures, &start_btn_obj, board_size_btns_ptr,
                window_rectangle, opponent_btns_ptr);
    }
}

void play(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY], int game_arr[19][19],
        state_t* state_ptr, SDL_Rect* window_rectangle, game_stats_t* game_stats_ptr,
            int prev_game_arr[19][19], bool* overlay_menu_ptr, TTF_Font* font,
                unsigned int* prev_play_ms_ptr)
{

    /* Ancho de los paneles */
    SDL_Rect resign_btn_rect_blk = {
        .x = 20,
        .w = 240,
        .h = 70
    };  
    resign_btn_rect_blk.y = SCREEN_HEIGHT - resign_btn_rect_blk.h - 20;

    SDL_Rect resign_btn_rect_wht = {
        .x = SCREEN_WIDTH - PANEL_WIDTH + 20,
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

    /* Los botones de blanco no deberían estar disponibles para negro y viceversa */
    if (game_stats_ptr->player == 1) {
        pass_btn_wht.enabled = false;
        resign_btn_wht.enabled = false;
    } else {
        pass_btn_blk.enabled = false;
        resign_btn_blk.enabled = false;
    }

    SDL_Rect main_menu_btn_rect = {
        .x = (SCREEN_WIDTH - 240) / 2,
        .y = 20*2 + 5,
        .w = 240,
        .h = 70
    };

    SDL_Rect save_game_btn_rect = main_menu_btn_rect;
    save_game_btn_rect.y += main_menu_btn_rect.h + 20 * 2;
    SDL_Rect exit_btn_rect = save_game_btn_rect;
    exit_btn_rect.y += save_game_btn_rect.h + 20 * 2;

    button_t main_menu_btn_obj = init_button(main_menu_btn_rect, menu_st, main_menu_text, true);
    button_t save_game_btn_obj = init_button(save_game_btn_rect, save_game_st, save_game_text, true);
    button_t exit_btn_obj = init_button(exit_btn_rect, exit_st, exit_text, true);
           
    button_t* button_ptrs[4] = {&pass_btn_blk, &pass_btn_wht, &resign_btn_blk, &resign_btn_wht};

    button_t* overlay_menu_btn_ptrs[3] = {&main_menu_btn_obj, &save_game_btn_obj, &exit_btn_obj};

    /* El rectángulo del menu */
    SDL_Rect menu_rect = {
        .x = (SCREEN_WIDTH - OVERLAY_MENU_WIDTH) / 2,
        .y = 5,
        .w = OVERLAY_MENU_WIDTH,
        .h = SCREEN_HEIGHT
    };

    /* Empezamos a procesar eventos con la variable event */
    SDL_Event event;
    SDL_Point mouse_position;
    SDL_GetMouseState(&mouse_position.x, &mouse_position.y);
    
    bool bot_exists = game_stats_ptr->opp;
    int bot_player = game_stats_ptr->bot_player;
    /* Flag nos dice si este es el turno del bot. */
    bool is_bot_turn = bot_exists && bot_player == game_stats_ptr->player;
    if (is_bot_turn) {
            if (process_bot_play(game_stats_ptr, prev_game_arr, game_arr, *prev_play_ms_ptr, state_ptr)) { 
                game_stats_ptr->player = game_stats_ptr->player % 2 + 1;
            }
    }
    /* SDL_PollEvent retorna 0 si no hay eventos disponibles, si no, retorna 1. */
    while (SDL_PollEvent(&event)) {
        /* Si existe IA... */
        
        switch (event.type) {
            /* El usuario pide salir del juego */
            case SDL_QUIT:
                *state_ptr = exit_st; 
                break;
            /* Se registra un click izquierdo down del usuario */
            case SDL_MOUSEBUTTONDOWN: 
                SDL_MouseButtonEvent* mouse_event = &event.button;
                if (check_mdown(game_stats_ptr, game_arr, prev_game_arr, mouse_event,
                            button_ptrs, state_ptr, overlay_menu_btn_ptrs, overlay_menu_ptr,
                                &menu_rect, is_bot_turn)) {
                    *prev_play_ms_ptr = SDL_GetTicks64();
                    game_stats_ptr->player = game_stats_ptr->player % 2 + 1;
                }
                break;
            case SDL_KEYDOWN:
                SDL_KeyboardEvent* keyboard_event = &event.key;
                if (keyboard_event->keysym.sym == SDLK_ESCAPE) { 
                    *overlay_menu_ptr = !*overlay_menu_ptr;
                }
                break;
            /* Caso default, por buena onda */
            default:
                break;
        }
    }

    /* Esto para no renderizar el último frame. en caso de que cambie el estado */
    if (*state_ptr == game_st) {
        /* Renderizamos toodooooo */
        render_game_state(game_stats_ptr, game_arr, renderer, textures, window_rectangle,
                button_ptrs, overlay_menu_ptr, overlay_menu_btn_ptrs, &menu_rect, font); 
    } else if (*state_ptr != save_game_st) {
        /* 
         * Esencialmente, si el estado cambia a algo que no sea guardado de partida, queremos
         * remover el overlay
         */
        *overlay_menu_ptr = false;
    }
}

void save_game(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY], state_t* state_ptr,
        SDL_Rect* window_rectangle, game_stats_t* game_stats_ptr, int prev_game_arr[19][19], 
        int game_arr[19][19], char** input_text, int* input_text_len, TTF_Font* font)
{
    
    SDL_Rect save_btn_rect = {
        .x = (SCREEN_WIDTH - 421) / 2.0,
        .y = (SCREEN_HEIGHT) / 2.0,
        .w = 421,
        .h = 171
    };

    button_t save_btn_obj = init_button(save_btn_rect, game_st, save_btn, true);
      

    SDL_StartTextInput();
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
                check_save_game_mdown(game_stats_ptr, game_arr, prev_game_arr, mouse_event,
                        &save_btn_obj, state_ptr, *input_text);
                break;
            case SDL_KEYDOWN:
                SDL_KeyboardEvent* keyboard_event = &event.key;
                if (keyboard_event->keysym.sym == SDLK_ESCAPE) *state_ptr = game_st;
                else if (keyboard_event->keysym.sym == SDLK_BACKSPACE && *input_text_len > 0) {
                    *input_text = realloc(*input_text, sizeof(char) * (*input_text_len));
                    (*input_text)[*input_text_len - 1] = '\0';
                    --(*input_text_len);
                }
                break;
            case SDL_TEXTINPUT:
                /* Ingresamos las letras y las concatenamos al nombre del save file */
                if (*input_text_len < 20) {
                    int to_append_len = strlen(event.text.text);
                    int concat_len = to_append_len + *input_text_len;
                    *input_text = realloc(*input_text, sizeof(char) * (concat_len + 1));
                    strncat(*input_text, event.text.text, to_append_len);
                    *input_text_len = concat_len;
                }
                break;
            /* Caso default, por buena onda */
            default:
                break;
        }
    }

    /* Si el estado cambió, debemos limpiar input_text */
    if (*state_ptr != save_game_st) {
        /* Limpiamos input_text */
        *input_text = realloc(*input_text, sizeof(char));
        **input_text = '\0';
        *input_text_len = 0;
    } else {
        /* Renderizamos el fondo del menu */ 
        render_save_game(renderer, textures, window_rectangle, font, *input_text, *input_text_len,
                &save_btn_obj);
    }

    SDL_StopTextInput();
    
}

void load_game(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY], state_t* state_ptr,
        SDL_Rect* window_rectangle, game_stats_t* game_stats_ptr, int prev_game_arr[19][19], 
        int game_arr[19][19], char** input_text, int* input_text_len, TTF_Font* font)
{   

    /* Rectángulo para botón de cargado de partida */
    SDL_Rect load_btn_rect = {
        .x = (SCREEN_WIDTH - 421) / 2.0,
        .y = (SCREEN_HEIGHT) / 2.0,
        .w = 421,
        .h = 171
    };

    button_t load_btn_obj = init_button(load_btn_rect, game_st, load_btn, true);

    /* Empezamos a procesar eventos con la variable event */
    SDL_StartTextInput();
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
                check_load_game_mdown(game_stats_ptr, game_arr, prev_game_arr, mouse_event,
                        &load_btn_obj, state_ptr, *input_text);
                break;
            case SDL_KEYDOWN:
                SDL_KeyboardEvent* keyboard_event = &event.key;
                if (keyboard_event->keysym.sym == SDLK_ESCAPE) *state_ptr = menu_st;
                else if (keyboard_event->keysym.sym == SDLK_BACKSPACE && *input_text_len > 0) {
                    *input_text = realloc(*input_text, sizeof(char) * (*input_text_len));
                    (*input_text)[*input_text_len - 1] = '\0';
                    --(*input_text_len);
                }
                break;
            case SDL_TEXTINPUT:
                /* Ingresamos las letras y las concatenamos al nombre del save file */
                if (*input_text_len < 20) {
                    int to_append_len = strlen(event.text.text);
                    int concat_len = to_append_len + *input_text_len;
                    *input_text = realloc(*input_text, sizeof(char) * (concat_len + 1));
                    strncat(*input_text, event.text.text, to_append_len);
                    *input_text_len = concat_len;
                }
                break;
            /* Caso default, por buena onda */
            default:
                break;
        }
    }


    /* Si el estado cambió, debemos limpiar input_text, y no renderizar la última frame. */
    if (*state_ptr != load_game_st) {
        /* Limpiamos input_text */
        *input_text = realloc(*input_text, sizeof(char));
        **input_text = '\0';
        *input_text_len = 0;
    } else {
        render_load_game(renderer, textures, window_rectangle, font, *input_text, *input_text_len,
                &load_btn_obj);
    }
    SDL_StopTextInput();

}


void end_game(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY], state_t* state_ptr,
        SDL_Rect* window_rectangle, game_stats_t* game_stats_ptr, TTF_Font* font)
{

    SDL_Rect main_menu_btn_rect = {
        .x = (SCREEN_WIDTH - 361) / 2.0,
        .y = (SCREEN_HEIGHT + PT_PANEL_HEIGHT) / 2.0 - 10,
        .w = 361,
        .h = 121
    };
    
    button_t main_menu_btn_obj = init_button(main_menu_btn_rect, menu_st, main_menu_text, true);
    
    /* Cadenas que serán renderizadas */
    char black_caps[5], white_caps[5], black_terr[5], white_terr[5], black_adv[5], komi[5];
    char black_total[5], white_total[5];

    /* Puntajes totales */
    float blk_total_n = get_black_total(game_stats_ptr);
    float wht_total_n = get_white_total(game_stats_ptr);
        
    snprintf(black_caps, 5, "%d", game_stats_ptr->black_caps);
    snprintf(white_caps, 5, "%d", game_stats_ptr->white_caps);
    snprintf(black_terr, 5, "%d", game_stats_ptr->black_terr);
    snprintf(white_terr, 5, "%d", game_stats_ptr->white_terr);
    snprintf(black_adv, 5, "%d", 0);
    snprintf(komi, 5, "%.1f", game_stats_ptr->komi);
    snprintf(black_total, 5, "%.f", blk_total_n); 
    snprintf(white_total, 5, "%.1f", wht_total_n);

    char* blk_results[4] = {
        black_caps,
        black_terr,
        black_adv,
        black_total
    };
    char* wht_results[4] = {
        white_caps,
        white_terr,
        komi,
        white_total
    };
    


    char* winner_text;
    /* Decidimos al ganador */
    if (game_stats_ptr->resign == 1) {
        winner_text = "Black Resigned, White Wins!";
    } else if (game_stats_ptr-> resign == 2) {
        winner_text = "White Resigned, Black Wins!";
    } else if (blk_total_n > wht_total_n) {
        winner_text = "Black Wins!";
    } else {
        winner_text = "White Wins!";
    }

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
                check_end_game_btn_press(&main_menu_btn_obj, mouse_event, state_ptr);
                break;
            case SDL_KEYDOWN:
                SDL_KeyboardEvent* keyboard_event = &event.key;
                if (keyboard_event->keysym.sym == SDLK_ESCAPE) *state_ptr = menu_st;
                break;
            default:
                break;
        }
    }


    /* Esto para no renderizar el último frame en caso de que se cambie el estado */
    if (*state_ptr == end_game_st) {
        render_end_game(renderer, textures, window_rectangle, &main_menu_btn_obj, game_stats_ptr, 
                font, winner_text, blk_results, wht_results);
    }
}


