#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "headers/init_funcs.h"
#include "headers/termination_funcs.h"
#include "headers/struct_objects.h"
#include "headers/buttons.h"
#include "headers/matrix_ops.h"
#include "headers/move_validation.h"
#include "headers/player_input_processing.h"
#include "headers/rendering.h"
#include "headers/game_states.h"


/*
 *      TODO
 *
 * - Transicionar a renderizar los grids individualmente, mayor control y más customizable.
 * - Completar funcionalidad de botones toggle
 * - Agregar UI al tablero, cambiar texturas.
 * - Hay "tearing" en botones de main menu, revisar porq ué.
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

                state_t state = menu_st;
                

                /* Definimos un rectángulo para renderizar el tablero en base a este */
                SDL_Rect window_rectangle = {
                    .x = 0,
                    .y = 0,
                    .w = RENDER_W,
                    .h = RENDER_H
                };
                /* middle pad */
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
                    .toggle = true,             /* Esto en true por conveniencia */
                    .txt_enum = nine_by_nine_btn,
                    .val = 9
                };

                toggle_button_t thirteen_by_thirteen_btn_obj = {
                    .rect = thirteen_by_thirteen_btn_rec,
                    .toggle = false,
                    .txt_enum = thirteen_by_thirteen_btn,
                    .val = 13
                };

                toggle_button_t nineteen_by_nineteen_btn_obj = {
                    .rect = nineteen_by_nineteen_btn_rec,
                    .toggle = false,
                    .txt_enum = nineteen_by_nineteen_btn,
                    .val = 19
                };

                /* Un arreglo de tres punteros a botones toggle */
                toggle_button_t* toggle_btn_ptrs[3] = {
                    &nine_by_nine_btn_obj,
                    &thirteen_by_thirteen_btn_obj,
                    &nineteen_by_nineteen_btn_obj
                };

                /* Creamos un grupo exclusivo de botones toggles */
                toggle_button_group_t board_size_btns = init_toggle_btn_grp(3, toggle_btn_ptrs, 9);
                
                               
                /* 
                 * Declaramos el game_arr como 19x19, esto para setearlo a 0 inmediatamente sin 
                 * tener que usar memset, además que a futuro cambiará el tamaño del tablero,
                 * en ese caso no quería andar usando malloc y realloc para pedir memoria.
                 * Adicionalmente creamos prev_game_arr que será usado para la regla ko
                 */
                int game_arr[19][19] = {{0}}, prev_game_arr[19][19] = {{0}}; 
                //print_matrix(19, game_arr);
                

                /* 
                 * Inicializamos el struct de estadísticas de juego, esto incluye el tamaño del
                 * tablero, que será 9 en default
                 */
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
                            game_set(renderer, textures, &state, &window_rectangle,
                                    &board_size_btns, &game_stats);
                            break;
                        case game_st:
                            play(renderer, textures, game_arr, &state,
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

