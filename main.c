#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "init_funcs.h"
#include "termination_funcs.h"
#include "matrix_ops.h"
#include "move_validation.h"
#include "player_input_processing.h"
#include "rendering.h"


/*
 *      TODO
 *
 * - Pasar un arreglo a punteros de botones para chequear si fueron presionados (?!)
 * - Reconsiderar seriamente la estructura de las funciones de play y menu
 * - Empezar a hablar de settings de tablero, modo de juego, etc.. Es decir, un menu pre-partida
 * - Menu overlay para cuando se presiona escape sobre las partidas. Dar opciones de volver a HOME,
 *   settings, o guardar y salir, etc etc etc etc
 * - Empezar a hablar de resolución adaptable
 * - Empezar a hablar de lo estético que podría ser este juego con lindas texturas y efectos de
 *   sonido
 * - Implementar casos de suicidio.
 * - Implementar completamente el sistema de Ko de Martín (ya se puede)
 * -  
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
void menu(SDL_Renderer*, SDL_Texture*[OBJ_QTY], state_type*, SDL_Rect*);

// /* Renderiza los botones del menú principal */
// void render_menu_buttons(SDL_Renderer*, SDL_Texture*[OBJ_QTY], SDL_Rect*, SDL_Rect*);
// 
/* Función de estado de juego en tablero, retorna el jugador que debe hacer el siguiente turno */
int play(SDL_Renderer*, SDL_Texture*[OBJ_QTY], int len, int[len][len], state_type*, SDL_Rect*, int);

/* Renderiza las piezas en base a la matriz de juego */
//void render_game_state(int len, int[len][len], SDL_Renderer*, SDL_Texture*[OBJ_QTY], SDL_Rect*);

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
                "./assets/menu/menu_background.bmp",
                "./assets/menu/play_button.bmp",
                "./assets/menu/option_button.bmp",
                "./assets/menu/leaderboard_button.bmp"
            };

            /* Intentamos cargar las superficies y texturas */
            if (load_surfaces_and_textures(surfaces, textures, img_paths, renderer)) {

                /* MENU GAME LOOP */
                //bool quit = false; /* Gameloop seguirá mientras la flag sea false */
                int len = 9; /* Tamaño del tablero, a modificar en el menú más adelante. */
                state_type state = menu_st;
                
                /* Definimos un rectángulo para renderizar el tablero en base a este */
                SDL_Rect window_rectangle;
                window_rectangle.x = 0;
                window_rectangle.y = 0;
                window_rectangle.w = RENDER_W;
                window_rectangle.h = RENDER_H;
                                            
                /* 
                 * Como game_arr es un arreglo de orden variable, no podemos setearlo a 0 inmediata
                 * mente, asi que lo hacemos manualmente. 
                 */
                int game_arr[len][len], player = 1; /* Turno jugador. 1 es Negro. */

                /* Seteamos la matriz de juego a ceros. */
                memset(game_arr,0, sizeof(game_arr));
                // print_game_arr(len, game_arr);

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
                        case game_st:
                            player = play(renderer, textures, len, game_arr, &state,
                                    &window_rectangle, player);
                            break;
                        case opt_st:
                            break;
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

int play(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY], int len, int game_arr[len][len],
        state_type* state_ptr, SDL_Rect* window_rectangle, int player)
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
                if (process_move(len, game_arr, mouse_event, player)) player = player % 2 + 1;
                break;
            case SDL_KEYDOWN:
                SDL_KeyboardEvent* keyboard_event = &event.key;
                if (keyboard_event->keysym.sym == SDLK_ESCAPE) { 
                    *state_ptr = menu_st;
                    /* 
                     * Hay un frame en el que se verán las piezas desaparecer del tablero, esto
                     * podría ser arreglado con un estado de transición, ya que no me agrada la 
                     * idea de hacer que el estado menu setee la matriz de juego a 0, tampoco me
                     * agrada la idea de setear el tablero a 0 en el switch case, ya que este está
                     * dentro de un loop, sería poco eficiente y tampoco le concierne a main estar
                     * resetenado matrices de juego. 
                     */
                    memset(game_arr, 0, sizeof(int) * len * len);
                    //print_game_arr(len, game_arr);
                    player = 1;
                }
                break;
            /* Caso default, por buena onda */
            default:
                break;
        }
    }

    /* Renderizamos toodooooo */
    render_game_state(len, game_arr, renderer, textures, window_rectangle); 
    
    /* Retornamos el jugador que debe hacer el siguiente movimiento */
    return player;
}

void menu(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY], state_type* state_ptr,
        SDL_Rect* window_rectangle)
{   

    SDL_Rect play_btn_rec;
    play_btn_rec.x = 50;
    play_btn_rec.y = 50;
    play_btn_rec.w = 390;
    play_btn_rec.h = 143;

    SDL_Rect opt_btn_rec = play_btn_rec;
    opt_btn_rec.y += play_btn_rec.h + 20;

    button play_btn_obj;
    play_btn_obj.rect = play_btn_rec;
    play_btn_obj.st_event = game_st;

    button opt_btn_obj;
    opt_btn_obj.rect = opt_btn_rec;
    opt_btn_obj.st_event = opt_st;


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
                check_menu_btn_press(&play_btn_obj, &opt_btn_obj, mouse_event, state_ptr);
                //printf("mouse click x = %i y = %i\n", mouse_event->x,
                //        mouse_event->y);
                break;
            /* Caso default, por buena onda */
            default:
                break;
        }
    }

    /* Manejamos updates al estado de juego */


    /* Limpiar y dibujar a la pantalla */
    SDL_RenderClear(renderer);

    /* Renderizamos el fondo del menu */ 
    SDL_RenderCopy(renderer, textures[menu_bck], NULL, window_rectangle);

    /* Renderizamos los botones del menu */
    render_menu_buttons(renderer, textures, &play_btn_rec, &opt_btn_rec);
    
    SDL_RenderPresent(renderer);
}
