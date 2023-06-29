#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include "headers/init_funcs.h"
#include "headers/termination_funcs.h"
#include "headers/struct_objects.h"
#include "headers/buttons.h"
#include "headers/matrix_ops.h"
#include "headers/game_states.h"


/*
 *      TODO
 *
 * - Implementar IA básica (random)
 *
 */


/* Resolución ventana */
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
/* Padding para parte superior e inferior del tablero */
const int B_PAD = 50;

/* Ancho de los paneles laterales en UI para partidas */
const int PANEL_WIDTH = (SCREEN_WIDTH - SCREEN_HEIGHT) / 2.0;

/* Ancho y altura de paneles de resumen de putos */
const int PT_PANEL_WIDTH = 940 * (3.0 / 8.0);
const int PT_PANEL_HEIGHT = 1400 * (3.0 / 8.0);

/* Ancho menu overlay en UI para partidas */
const int OVERLAY_MENU_WIDTH = 436;

/* Resolución lógica para renderizado */
const int RENDER_W = 1280;
const int RENDER_H = 720;

/* Cap de framerate en frames por segundo */
const int F_CAP = 30;

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
                "./assets/menu/19x19_btn_pressed.bmp",
                "./assets/game/grid.bmp",
                "./assets/game/black_panel.bmp",
                "./assets/game/white_panel.bmp",
                "./assets/game/pass_btn.bmp",
                "./assets/game/panel_mask.bmp",
                "./assets/game/resign_btn.bmp",
                "./assets/game/captures.bmp",
                "./assets/game/overlay_menu.bmp",
                "./assets/game/main_menu_text.bmp",
                "./assets/game/save_game_text.bmp",
                "./assets/game/exit_text.bmp",
                "./assets/save_game_menu/save_name_field.bmp",
                "./assets/save_game_menu/save_btn.bmp",
                "./assets/load_game_menu/load_btn.bmp",
                "./assets/menu/load_game_btn.bmp",
                "./assets/game/territory.bmp",
                "./assets/end_game/black_point_panel.bmp",
                "./assets/end_game/white_point_panel.bmp",
                "./assets/menu/none.bmp",
                "./assets/menu/none_pressed.bmp",
                "./assets/menu/basic.bmp",
                "./assets/menu/basic_pressed.bmp",
            };
            TTF_Font* ethnocentric_rg = NULL;
            /* Intentamos cargar las superficies y texturas */
            if (load_assets(surfaces, textures, img_paths, &ethnocentric_rg, renderer)) {

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

                /* Creamos el rectángulo para para botones de elección de oponente */
                SDL_Rect none_btn_rec;
                none_btn_rec.w = 361;
                none_btn_rec.h = 106;
                /* Dos elementos centrados en eje x, padding de 50 entre ellos */
                none_btn_rec.x = (SCREEN_WIDTH - 2 * none_btn_rec.w - m_pad) / 2;
                none_btn_rec.y = nine_by_nine_btn_rec.y - nine_by_nine_btn_rec.h - m_pad;
                /* El rectángulo de botón de oponente básico */
                SDL_Rect basic_btn_rec = none_btn_rec;
                basic_btn_rec.x += none_btn_rec.w + m_pad;

                /* 
                 * Creamos los objetos para los botones tipo toggle, la razón por la que creamos
                 * esta cantidad de botones en main es por temas de scope, queremos mantener el
                 * state de ciertas cosas, y lamentablemente eso implica declararlas acá. Igualmente 
                 * debe haber una mejor implementación.
                 */
                toggle_button_t nine_by_nine_btn_obj = {
                    .rect = nine_by_nine_btn_rec,
                    .toggle = true,                     /* Esto en true como default */
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

                toggle_button_t none_btn_obj = {
                    .rect = none_btn_rec,
                    .toggle = true,                    /* Esto en true como default */
                    .txt_enum = none_btn,
                    .val = 0,
                };

                toggle_button_t basic_btn_obj = {
                    .rect = basic_btn_rec,
                    .toggle = false,
                    .txt_enum = basic_btn,
                    .val = 1
                };

                /* Un arreglo de tres punteros a botones toggle */
                toggle_button_t* toggle_btn_ptrs_board[3] = {
                    &nine_by_nine_btn_obj,
                    &thirteen_by_thirteen_btn_obj,
                    &nineteen_by_nineteen_btn_obj
                };

                toggle_button_t* toggle_btn_ptrs_opp[2] = {
                    &none_btn_obj,
                    &basic_btn_obj
                };

                /* 
                 * Creamos dos grupos de botones toggle de estado mutuamente exclusivo, es decir
                 * si uno se encuentra presionado, los otros deben estar desactivados, y así.
                 */
                toggle_button_group_t board_size_btns = init_toggle_btn_grp(3,toggle_btn_ptrs_board, 9);
                toggle_button_group_t opponent_btns = init_toggle_btn_grp(2, toggle_btn_ptrs_opp, 0);

                
                               
                /* 
                 * Declaramos el game_arr como 19x19, esto para setearlo a 0 inmediatamente sin 
                 * tener que usar memset, además que a futuro cambiará el tamaño del tablero,
                 * en ese caso no quería andar usando malloc y realloc para pedir memoria.
                 * Adicionalmente creamos prev_game_arr que será usado para la regla ko
                 */
                int game_arr[19][19] = {{0}}, prev_game_arr[19][19] = {{0}}; 
                

                /* 
                 * Inicializamos el struct de estadísticas de juego, esto incluye, entre muchas
                 * otras cosas, el tamaño del tablero, que será 9x9 como default.
                 */
                game_stats_t game_stats = init_game_stats();
                
                /* Booleano de overlay_menu para state de juego, se activa presionando escape */
                bool overlay_menu = false;

                /* Esto es para el cálculo de cuadros por segundo. Y delay para jugadas de bot */
                unsigned int prev_frame_ms = 0, prev_play_ms = 0;
                /* Input_text para guardar texto de archivos de guardado y carga */
                char* input_text = malloc(sizeof(char));
                input_text[0] = '\0';
                int input_text_len = 0;
                
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
                                    &board_size_btns, &game_stats, game_arr, prev_game_arr,
                                    &overlay_menu, &opponent_btns, &prev_play_ms);
                            break;
                        case game_st:
                            play(renderer, textures, game_arr, &state, &window_rectangle,
                                    &game_stats, prev_game_arr, &overlay_menu, ethnocentric_rg,
                                    &prev_play_ms);
                            break;
                        case end_game_st:
                            end_game(renderer, textures, &state, &window_rectangle, &game_stats,
                                    ethnocentric_rg);
                            break;
                        case save_game_st:
                            save_game(renderer, textures, &state, &window_rectangle, &game_stats,
                                    prev_game_arr, game_arr, &input_text, &input_text_len,
                                        ethnocentric_rg);
                            break;
                        case load_game_st:
                            load_game(renderer, textures, &state, &window_rectangle, &game_stats,
                                    prev_game_arr, game_arr, &input_text, &input_text_len,
                                        ethnocentric_rg);
                            break;
                        /* Manejamos exit state para que el compilador deje de molestar con warnings */
                        case exit_st:
                            break;
                    }
                }

                /* Liberamos la cadena de input_text */
                free(input_text);
                input_text = NULL;
            }
            /* Se nulifican punteros a texturas y fuente cargada */
            free_asset_ptrs(textures, ethnocentric_rg);
        } 
    }

    /* Liberamos recursos, destruimos cositas como el renderer y cerramos SDL */
    close_sdl(g_window, renderer);
    return 0;
}

