#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "headers/init_funcs.h"
#include "headers/termination_funcs.h"
#include "headers/struct_objects.h"
#include "headers/matrix_ops.h"
#include "headers/move_validation.h"
#include "headers/player_input_processing.h"
#include "headers/rendering.h"
#include "headers/game_states.h"


/*
 *      TODO
 *
 * - Pasar un arreglo a punteros de botones para chequear si fueron presionados (?!)
 * - Empezar a hablar de settings de tablero, modo de juego, etc.. Es decir, un menu pre-partida
 * - Transicionar a renderizar los grids individualmente, mayor control y más customizable.
 * - Arreglar funciones que toman rectangulos de botones, es poco intuitivo y molesto.
 *
 */


/* Resolución ventana */
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

/* Resolución lógica para renderizado */
const int RENDER_W = 1280;
const int RENDER_H = 720;

/* Cap de framerate en frames por segundo */
const int F_CAP = 30;

/* Función de estado del menu */
//void menu(SDL_Renderer*, SDL_Texture*[OBJ_QTY], state_t*, SDL_Rect*);

/* Función de estado de juego en tablero */
//void play(SDL_Renderer*, SDL_Texture*[OBJ_QTY], int len, int[len][len], state_t*, SDL_Rect*,
//        game_stats_t*, int[len][len]);

/* Función previa al juego, elección de tamaño de tablero, settings, etc... */
//void game_set(SDL_Renderer*, SDL_Texture*[OBJ_QTY], state_t*, SDL_Rect*);


int main(int argc, char** argv)
{
    /* Ventana de juego */
    SDL_Window* g_window = NULL;

    /* Renderer para la ventana */
    SDL_Renderer* renderer = NULL;

    /* Inicializamos SDL y creamos la ventana */
    if (init_sdl(&g_window)) {

        /* Creamos el renderer */ 
        if (create_renderer(g_window, &renderer)) {

            /* Creamos arreglos para las superficies, texturas, y sus bmps en el archivo assets. */
            SDL_Surface* surfaces[OBJ_QTY] = {NULL};
            SDL_Texture* textures[OBJ_QTY] = {NULL}; 
            char img_paths[OBJ_QTY][BMP_LEN] = {
                "./assets/game/game_board_9x9.bmp",
                "./assets/game/white_piece.bmp",
                "./assets/game/black_piece.bmp",
                "./assets/menu/menu_bg.bmp",
                "./assets/menu/new_game_btn.bmp",
                "./assets/menu/settings_btn.bmp",
                "./assets/menu/start_btn.bmp",
                "./assets/menu/rankings_btn.bmp",
                "./assets/menu/exit_btn.bmp",
                "./assets/menu/9x9_btn.bmp",
                "./assets/menu/9x9_btn_pressed.bmp",
                "./assets/menu/13x13_btn.bmp",
                "./assets/menu/13x13_btn_pressed.bmp",
                "./assets/menu/19x19_btn.bmp",
                "./assets/menu/19x19_btn_pressed.bmp"
            };

            /* Intentamos cargar las superficies y texturas */
            if (load_surfaces_and_textures(surfaces, textures, img_paths, renderer)) {

                int len = 9; /* Tamaño del tablero, a modificar en el game_set más adelante. */
                state_t state = menu_st;
                

                /* Definimos un rectángulo para renderizar el tablero en base a este */
                SDL_Rect window_rectangle = {
                    .x = 0,
                    .y = 0,
                    .w = RENDER_W,
                    .h = RENDER_H
                };
                /* Down pad, middle pad */
                int m_pad = 50;

                /* Creamos el rectángulo para el boton de 9x9 en gameset */
                SDL_Rect nine_by_nine_btn_rec;
                nine_by_nine_btn_rec.w = 361;
                nine_by_nine_btn_rec.h = 121;
                /* Tres elementos centrados en eje x, padding de 50 entre ellos */
                nine_by_nine_btn_rec.x = (SCREEN_WIDTH - 3 * nine_by_nine_btn_rec.w - 2 * m_pad) / 2;
                nine_by_nine_btn_rec.y = (SCREEN_HEIGHT - nine_by_nine_btn_rec.h) / 2;

                /* Definimos el resto de los rectangulos de botones para tamaño de tablero */
                SDL_Rect thirteen_by_thirteen_btn_rec = nine_by_nine_btn_rec;
                thirteen_by_thirteen_btn_rec.x += nine_by_nine_btn_rec.w + m_pad;
                SDL_Rect nineteen_by_nineteen_btn_rec = thirteen_by_thirteen_btn_rec;
                nineteen_by_nineteen_btn_rec.x += thirteen_by_thirteen_btn_rec.w + m_pad;

                /* Creamos los objetos para los botones tipo toggle */
                toggle_button_t nine_by_nine_btn_obj = {
                    .rect = nine_by_nine_btn_rec,
                    .toggle = false,
                    .txt_enum = nine_by_nine_btn,
                };

                toggle_button_t thirteen_by_thirteen_btn_obj = {
                    .rect = thirteen_by_thirteen_btn_rec,
                    .toggle = false,
                    .txt_enum = thirteen_by_thirteen_btn
                };

                toggle_button_t nineteen_by_nineteen_btn_obj = {
                    .rect = nineteen_by_nineteen_btn_rec,
                    .toggle = false,
                    .txt_enum = nineteen_by_nineteen_btn
                };

                /* Un arreglo de tres punteros a botones toggle */
                toggle_button_t* toggle_btn_ptrs[3] = {
                    &nine_by_nine_btn_obj,
                    &thirteen_by_thirteen_btn_obj,
                    &nineteen_by_nineteen_btn_obj
                };
                
                /* 
                 * Como game_arr es un arreglo de orden variable, no podemos setearlo a 0 inmediata
                 * mente, asi que lo hacemos manualmente. 
                 * También declaramos una matriz de tablero para guardar un estado anterior, para
                 * así revisar la regla ko 
                 */

                int game_arr[len][len], prev_game_arr[len][len]; 

                /* Inicializamos el struct de estadísticas de juego */
                game_stats_t game_stats = init_game_stats();

                /* Seteamos las matrices de juego a ceros. */
                memset(game_arr, 0, sizeof(game_arr));
                memset(prev_game_arr, 0, sizeof(prev_game_arr));

                unsigned int prev_frame_ms = 0;

                while (state) {
                    /* Queremos que cada 1000 / F_CAP milisegundos ocurra un frame */ 
                    unsigned int current_ms = SDL_GetTicks64();
                    /* Delta es cant. de milisegundos desde último frame */
                    unsigned int delta = current_ms - prev_frame_ms; 
                    if ((delta < 1000.0 / F_CAP) && prev_frame_ms) {
                        continue;
                    }
                    prev_frame_ms = current_ms;

                    switch (state) {
                        case menu_st:
                            menu(renderer, textures, &state, &window_rectangle);
                            break;
                        case game_set_st:
                            game_set(renderer, textures, &state, &window_rectangle, toggle_btn_ptrs);
                            break;
                        case game_st:
                            play(renderer, textures, len, game_arr, &state,
                                    &window_rectangle, &game_stats, prev_game_arr);
                            break;
                        case settings_st:
                            break;
                        case rankings_st:
                            break;
                        /* Manejamos exit state para que el compilador deje de wear */
                        case exit_st:
                            break;
                    }
                }
            }

            free_texture_ptrs(textures);
        } 
    }

    /* Liberamos recursos y cerramos SDL */
    close_sdl(&g_window, &renderer);
    return 0;
}

// void play(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY], int len, int game_arr[len][len],
//         state_t* state_ptr, SDL_Rect* window_rectangle, game_stats_t* game_stats,
//             int prev_game_arr[len][len])
// {
//     
//     /* Empezamos a procesar eventos con la variable event */
//     SDL_Event event;
//     SDL_Point mouse_position;
//     SDL_GetMouseState(&mouse_position.x, &mouse_position.y);
//     
//     /* SDL_PollEvent retorna 0 si no hay eventos disponibles, si no, retorna 1. */
//     while (SDL_PollEvent(&event)) {
//         switch (event.type) {
//             /* El usuario pide salir del juego */
//             case SDL_QUIT:
//                 *state_ptr = exit_st; 
//                 break;
//             /* Se registra un click izquierdo down del usuario */
//             case SDL_MOUSEBUTTONDOWN: 
//                 SDL_MouseButtonEvent* mouse_event = &event.button;
//                 //printf("mouse click x = %i y = %i\n", mouse_event->x,mouse_event->y);
//                 if (process_move(len, game_arr, mouse_event, game_stats->player, prev_game_arr)) {
//                     game_stats->player = game_stats->player % 2 + 1;
//                 }
//                 break;
//             case SDL_KEYDOWN:
//                 SDL_KeyboardEvent* keyboard_event = &event.key;
//                 if (keyboard_event->keysym.sym == SDLK_ESCAPE) { 
//                     *state_ptr = menu_st;
//                     /* 
//                      * Hay un frame en el que se verán las piezas desaparecer del tablero, esto
//                      * podría ser arreglado con un estado de transición, ya que no me agrada la 
//                      * idea de hacer que el estado menu setee la matriz de juego a 0, tampoco me
//                      * agrada la idea de setear el tablero a 0 en el switch case, ya que este está
//                      * dentro de un loop, sería poco eficiente y tampoco le concierne a main estar
//                      * resetenado matrices de juego. 
//                      */
//                     memset(game_arr, 0, sizeof(int) * len * len);
//                     *game_stats = init_game_stats();
// 
//                 }
//                 break;
//             /* Caso default, por buena onda */
//             default:
//                 break;
//         }
//     }
// 
//     /* Renderizamos toodooooo */
//     render_game_state(len, game_arr, renderer, textures, window_rectangle); 
// }
// 
// void menu(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY], state_t* state_ptr,
//         SDL_Rect* window_rectangle)
// {   
// 
//     SDL_Rect play_btn_rec = {
//         .x = 50,
//         .y = 50,
//         .w = 390,
//         .h = 143
//     };
// 
//     SDL_Rect opt_btn_rec = {
//         .x = 50,
//         .y = play_btn_rec.y + play_btn_rec.h + 20,
//         .w = 390,
//         .h = 143
//     };
//    
// 
//     button_t play_btn_obj = {
//         .rect = play_btn_rec,
//         .st_event = game_set_st
//     };
// 
//     button_t opt_btn_obj = {
//         .rect = opt_btn_rec,
//         .st_event = opt_st
//     };
// 
//     
// 
//     /* Empezamos a procesar eventos con la variable event */
//     SDL_Event event;
//     SDL_Point mouse_position;
//     SDL_GetMouseState(&mouse_position.x, &mouse_position.y);
// 
//     /* SDL_PollEvent retorna 0 si no hay eventos disponibles, si no, retorna 1. */
//     while (SDL_PollEvent(&event)) {
//         switch (event.type) {
//             /* El usuario pide salir del juego */
//             case SDL_QUIT:
//                 *state_ptr = exit_st; 
//                 break;
//             /* Se registra un click izquierdo down del usuario */
//             case SDL_MOUSEBUTTONDOWN: 
//                 SDL_MouseButtonEvent* mouse_event = &event.button;
//                 check_menu_btn_press(&play_btn_obj, &opt_btn_obj, mouse_event, state_ptr);
//                 //printf("mouse click x = %i y = %i\n", mouse_event->x,
//                 //        mouse_event->y);
//                 break;
//             /* Caso default, por buena onda */
//             default:
//                 break;
//         }
//     }
// 
// 
//     /* Limpiar y dibujar a la pantalla */
//     SDL_RenderClear(renderer);
// 
//     /* Renderizamos el fondo del menu */ 
//     SDL_RenderCopy(renderer, textures[menu_bck], NULL, window_rectangle);
// 
//     /* Renderizamos los botones del menu */
//     render_menu_buttons(renderer, textures, &play_btn_obj, &opt_btn_obj);
//     
//     SDL_RenderPresent(renderer);
// }
// 
// 
// void game_set(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY], state_t* state_ptr,
//         SDL_Rect* window_rectangle) 
// {
// 
//     /* Down pad, middle pad */
//     int d_pad = 50, m_pad = 50;
//     SDL_Rect start_btn_rec;
//     start_btn_rec.w = 410;
//     start_btn_rec.h = 121;
//     start_btn_rec.x = (SCREEN_WIDTH - start_btn_rec.w) / 2; /* Centrado en eje x */
//     start_btn_rec.y = SCREEN_HEIGHT - start_btn_rec.h - d_pad; /* Padding de 50 con inferior */
// 
//     SDL_Rect nine_by_nine_btn_rec;
//     nine_by_nine_btn_rec.w = 361;
//     nine_by_nine_btn_rec.h = 121;
//     /* Tres elementos centrados en eje x, padding de 50 entre ellos */
//     nine_by_nine_btn_rec.x = (SCREEN_WIDTH - 3 * nine_by_nine_btn_rec.w - 2 * m_pad) / 2;
//     nine_by_nine_btn_rec.y = (SCREEN_HEIGHT - nine_by_nine_btn_rec.h) / 2;
// 
//     /* Definimos el resto de los botones para tamaño de tablero */
//     SDL_Rect thirteen_by_thirteen_btn_rec = nine_by_nine_btn_rec;
//     thirteen_by_thirteen_btn_rec.x += nine_by_nine_btn_rec.w + m_pad;
// 
//     SDL_Rect nineteen_by_nineteen_btn_rec = thirteen_by_thirteen_btn_rec;
//     nineteen_by_nineteen_btn_rec.x += thirteen_by_thirteen_btn_rec.w + m_pad;
// 
//     /* Creamos los structs de botones */
//     button_t start_btn_obj = {
//         .rect = start_btn_rec,
//         .st_event = game_st
//     };
//     
//     /* Creamos los objetos para los botones tipo toggle */
//     toggle_button_t nine_by_nine_btn_obj = {
//         .rect = nine_by_nine_btn_rec,
//         .toggle = false,
//         .txt_enum = nine_by_nine_btn,
//     };
// 
//     toggle_button_t thirteen_by_thirteen_btn_obj = {
//         .rect = thirteen_by_thirteen_btn_rec,
//         .toggle = false,
//         .txt_enum = thirteen_by_thirteen_btn
//     };
// 
//     toggle_button_t nineteen_by_nineteen_btn_obj = {
//         .rect = nineteen_by_nineteen_btn_rec,
//         .toggle = false,
//         .txt_enum = nineteen_by_nineteen_btn
//     };
// 
//     /* Un arreglo de tres punteros a botones toggle */
//     toggle_button_t* toggle_btn_ptrs[3] = {
//         &nine_by_nine_btn_obj,
//         &thirteen_by_thirteen_btn_obj,
//         &nineteen_by_nineteen_btn_obj
//     };
// 
//     /* Empezamos a procesar eventos con la variable event */
//     SDL_Event event;
//     SDL_Point mouse_position;
//     SDL_GetMouseState(&mouse_position.x, &mouse_position.y);
// 
//     /* SDL_PollEvent retorna 0 si no hay eventos disponibles, si no, retorna 1. */
//     while (SDL_PollEvent(&event)) {
//         switch (event.type) {
//             /* El usuario pide salir del juego */
//             case SDL_QUIT:
//                 *state_ptr = exit_st; 
//                 break;
//             /* Se registra un click izquierdo down del usuario */
//             case SDL_MOUSEBUTTONDOWN: 
//                 SDL_MouseButtonEvent* mouse_event = &event.button;
//                 check_game_set_btn_press(&start_btn_obj, toggle_btn_ptrs, mouse_event, state_ptr);
//                 //printf("mouse click x = %i y = %i\n", mouse_event->x,
//                 //        mouse_event->y);
//                 break;
//             case SDL_KEYDOWN:
//                 SDL_KeyboardEvent* keyboard_event = &event.key;
//                 if (keyboard_event->keysym.sym == SDLK_ESCAPE) { 
//                     *state_ptr = menu_st;
//                 }
//             /* Caso default, por buena onda */
//             default:
//                 break;
//         }
//     }
// 
//      /* Limpiar y dibujar a la pantalla */
//     SDL_RenderClear(renderer);
// 
//     /* Renderizamos el fondo del menu */ 
//     SDL_RenderCopy(renderer, textures[menu_bck], NULL, window_rectangle);
// 
//     /* Renderizamos los botones del menu */
//     render_game_set_buttons(renderer, textures, &start_btn_obj, toggle_btn_ptrs);
//     
//     SDL_RenderPresent(renderer);
// 
// }
