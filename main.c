#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

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
 *
 */





/* 
 * Definimos un límite arbtirario para la cantidad de texturas/superficies a tener, además de una 
 * longitud máxima para el path a un archivo bmp a ser cargado.
 */
#define OBJ_QTY 10
#define BMP_LEN 50

/* Resolución ventana */
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

/* Resolución lógica para renderizado */
const int RENDER_W = 1280;
const int RENDER_H = 720;

/* Cap de framerate en frames por segundo */
const int F_CAP = 30;

/* Enums para texturas y superficies, solo para que no hayan números mágicos dando vuelta. */
typedef enum
{
    board = 0,
    white_pc,
    black_pc,
    menu_bck,
    play_btn,
    opt_btn,
    ldrbrd_btn
}sf_and_txt_enum;

typedef enum
{
    exit_st = 0,
    menu_st,
    game_st,
    opt_st
}state_type;

/* Objeto para uso en botones, guarda un rectangulo y el state al que debe llevar */
typedef struct{
    SDL_Rect rect;
    state_type st_event;
}button;



/* Incializa SDL y crea una ventana */
bool init_sdl(SDL_Window**);

/* Crea el renderer */
bool create_renderer(SDL_Window*, SDL_Renderer**);

/* Carga todas las superficies y texturas necesarias */
bool load_surfaces_and_textures(SDL_Surface*[OBJ_QTY], SDL_Texture*[OBJ_QTY], char[OBJ_QTY][BMP_LEN],
        SDL_Renderer*); 

/* Función de estado del menu */
void menu(SDL_Renderer*, SDL_Texture*[OBJ_QTY], state_type*, SDL_Rect*);

/* Chequea cuál botón se presionó en el menú */
void check_menu_btn_press(button*, button*, SDL_MouseButtonEvent*, state_type*);

/* Renderiza los botones del menú principal */
void render_menu_buttons(SDL_Renderer*, SDL_Texture*[OBJ_QTY], SDL_Rect*, SDL_Rect*);

/* Función de estado de juego en tablero */
void play(SDL_Renderer*, SDL_Texture*[OBJ_QTY], int len, int[len][len], state_type*, SDL_Rect*);

/* Renderiza las piezas en base a la matriz de juego */
void render_game_state(int len, int[len][len], SDL_Renderer*, SDL_Texture*[OBJ_QTY]);

/* Libera las texturas del tablero, piezas de juego, etc */
void free_texture_ptrs(SDL_Texture*[OBJ_QTY]);

/* Libera window, renderer y cierra SDL */
void close_sdl();




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
                int game_arr[len][len];
                for (int i = 0; i < len; ++i) {
                    for (int j = 0; j < len; ++j) game_arr[i][j] = 0;
                }

                unsigned int prev_frame_ms = 0;
                while (state) {
                    /* Queremos que cada 1000 / F_CAP milisegundos ocurra un frame */ 
                    unsigned int current_ms = SDL_GetTicks64();
                    /* Delta es cant. de milisegundos desde último frame */
                    unsigned int delta = current_ms - prev_frame_ms; 
                    if ((delta < 1000.0 / F_CAP) && prev_frame_ms) {
                        continue;
                    }
                    // printf("Current time in ms is %li\n", SDL_GetTicks64());
                    prev_frame_ms = current_ms;

                    switch (state) {
                        case menu_st:
                            menu(renderer, textures, &state, &window_rectangle);
                            break;
                        case game_st:
                            play(renderer, textures, len, game_arr, &state, &window_rectangle);
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

void play(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY], int len, int game_arr[len][len],
        state_type* state_ptr, SDL_Rect* window_rectangle)
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
                /* ANÁLISIS CLICK CORTESÍA GUILLERMO */
                SDL_MouseButtonEvent* mouse_event = &event.button;
                printf("mouse click x = %i y = %i\n", mouse_event->x,mouse_event->y);
                /*Se inicializa variable correspondiente a la coordenada de la esquina superior izquierda tablero*/
                int currentx= 363, currenty = 82;
                int sqrside= 557 / (len - 1);

                /*Verifica casilla por casilla si el mouse hizo click en la hitbox*/
                for (int i = 0; i < len; i++){
                    currentx = 363;
                    for (int j = 0; j < len; j++){
                        if (currentx - sqrside / 2 < (mouse_event->x) &&
                                (mouse_event->x) < currentx + sqrside / 2 &&
                                currenty-sqrside / 2 < (mouse_event->y) &&
                                (mouse_event->y) < currenty + sqrside / 2 &&
                                game_arr[j][i] == 0){

                        game_arr[j][i] = 1;
                        goto exit;
                        }
                        currentx += sqrside;
                    }
                    currenty+=sqrside;
                }
                printf("espacio no disponible\n");
                exit:
                /* Continue no sé si es necesario */
                continue;
                //NOTA: Creo que seria de utilidad meterlo en una función para que el codigo sea un poco más legible
                break;
            case SDL_KEYDOWN:
                SDL_KeyboardEvent* keyboard_event = &event.key;
                if (keyboard_event->keysym.sym == SDLK_ESCAPE) *state_ptr = menu_st;
                break;
            /* Caso default, por buena onda */
            default:
                break;
        }
    }

    /* Manejamos updates al estado de juego */


    /* Limpiar y dibujar a la pantalla */
    SDL_RenderClear(renderer);

    /* Renderizamos el tablero */ 
    SDL_RenderCopy(renderer, textures[board], NULL, window_rectangle);
    /* Renderizamos en base al estado de la matriz de juego. */
    render_game_state(len, game_arr, renderer, textures); 

    SDL_RenderPresent(renderer);

}

void menu(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY], state_type* state_ptr,
        SDL_Rect* window_rectangle)
{   

    SDL_Rect play_btn_rec;
    play_btn_rec.x = 50;
    play_btn_rec.y = 50;
    play_btn_rec.w = 780;
    play_btn_rec.h = 285;

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

void check_menu_btn_press(button* play_btn_obj, button* opt_btn_obj,
        SDL_MouseButtonEvent* mouse_event, state_type* state_ptr)
{

    int x = mouse_event->x;
    int y = mouse_event->y;
    bool in_x_range = x >= play_btn_obj->rect.x &&  x <= play_btn_obj->rect.x + play_btn_obj->rect.w;
    bool in_y_range = y >= play_btn_obj->rect.y && y <= play_btn_obj->rect.y + play_btn_obj->rect.h;
    if (in_x_range && in_y_range) *state_ptr = game_st;

}

void render_menu_buttons(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY],
        SDL_Rect* play_btn_rec, SDL_Rect* opt_btn_rec)
{
        
    SDL_RenderCopy(renderer, textures[play_btn], NULL, play_btn_rec);
    SDL_RenderCopy(renderer, textures[opt_btn], NULL, opt_btn_rec);

}

bool init_sdl(SDL_Window** window_ptr)
{
    /* Flag de inicialización */
    bool success = true;

    /* Inicializamos SDL, retorna negativo en caso de fallar. */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL no pudo ser inicializado. SDL_Error: %s\n", SDL_GetError());
        success = false;
    }
    else {
        /* Creamos la ventana */
        *window_ptr = SDL_CreateWindow("GO", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                SCREEN_WIDTH, SCREEN_HEIGHT, 0);
        if (!(*window_ptr)) {
            printf("Window could not be created. SDL_ERROR: %s\n", SDL_GetError());
            success = false;
        }
    }

    return success;
}

bool create_renderer(SDL_Window* window, SDL_Renderer** renderer_ptr)
{
    /* Flag de éxito para la creación del renderer */
    bool success = true;
    *renderer_ptr = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!(*renderer_ptr)) {
            printf("Renderer no pudo ser inicializado. SDL_ERROR: %s\n", SDL_GetError());
            success = false;
    }

    /* Intentamos setear una resolución lógica fija para renderizar, retorna negativo en error. */ 
    if (SDL_RenderSetLogicalSize(*renderer_ptr, RENDER_W, RENDER_H) < 0) {
            printf("Resolución del renderer no pudo ser colocada. SDL_ERROR: %s\n", SDL_GetError());
    }
    return success;
}

bool load_surfaces_and_textures(SDL_Surface* surfaces[OBJ_QTY], SDL_Texture* textures[OBJ_QTY],
        char img_paths[OBJ_QTY][BMP_LEN], SDL_Renderer* renderer)
{
    bool success = true;
    /* Cargamos imágenes de las superficies */ 
    printf("Cargando superficies desde bmp...\n");
    for (int id = board; id <= ldrbrd_btn; ++id) {
        surfaces[id] = SDL_LoadBMP(img_paths[id]);
        if (!surfaces[id]) {
            printf("Superficie de indice %d en enum no pudo ser cargada. SDL_ERROR: %s\n", id,
                    SDL_GetError());
            success = false;
            break;
        }

        printf("bmp cargado: %s\n", img_paths[id]);
    }
    
    /* En caso de que se hayan cargado las superficies de manera exitosa, procedemos a texturas. */
    printf("Cargando texturas a renderer...\n");
    if (success) {
        for (int id = board; id <= ldrbrd_btn; ++id) {
            textures[id] = SDL_CreateTextureFromSurface(renderer, surfaces[id]);
            SDL_FreeSurface(surfaces[id]);
            surfaces[id] = NULL;
            if (!textures[id]) {
                printf("Texture de indice %d en enum no pudo ser cargada. SDL_ERROR: %s\n", id,
                        SDL_GetError());
                success = false;
                break;
            }
        printf("bmp de textura cargada: %s\n", img_paths[id]);
        }
    }

    return success;
}

void render_game_state(int len, int game_arr[len][len], SDL_Renderer* renderer,
        SDL_Texture* textures[OBJ_QTY])
{
    SDL_Rect pc_rectangle;
    
    /* 
     * El ancho óptimo para las piezas es 50x50 en ventanas de 1280x720, mantendrémos
     * ese aspect ratio.
     */

    
    pc_rectangle.w = 65;
    pc_rectangle.h = 65;
     

    /* 
     * Posición centrada de x e y en donde colocar la pieza, tener en consideración que
     * el eje y positivo va hacia abajo, por eso se resta a ambos la mitad de height y
     * width.
     * */
    
    /* Flag de éxito para la actualización del estado del juego */
    for (int i = 0; i < len; ++i) {
        for (int j = 0; j < len; ++j) {
            if (game_arr[i][j] == 0) continue;
            
            pc_rectangle.x = 363 + i*69 - pc_rectangle.w / 2.0;
            pc_rectangle.y = 82 + j*69 - pc_rectangle.h / 2.0;

            if (game_arr[i][j] == 1) {
                /* Dibujamos pieza blanca en coordenada correspondiente */
                SDL_RenderCopy(renderer, textures[white_pc], NULL, &pc_rectangle);
            }
            else if (game_arr[i][j] == 2) {
                /* Dibujamos pieza negra en coordenada correspondiente */
                SDL_RenderCopy(renderer, textures[black_pc], NULL, &pc_rectangle);
            }
        }
    }
    
}

void free_texture_ptrs(SDL_Texture* textures[OBJ_QTY])
{
    printf("Dejando en nulo punteros a texturas...\n");
    for (int id = board; id < ldrbrd_btn; ++id) {
        textures[id] = NULL;
    }
}

void close_sdl(SDL_Window** window_ptr, SDL_Renderer** renderer_ptr)
{
    printf("Cerrando aplicación.\n");
    /* Liberamos las texturas de la memoria al destruir el renderer */
    if (*renderer_ptr) SDL_DestroyRenderer(*renderer_ptr);
    *renderer_ptr = NULL;
    
    /* Se destruye la ventana de juego */
    if (*window_ptr) SDL_DestroyWindow(*window_ptr);
    *window_ptr = NULL;

    /* Finalizamos SDL */
    SDL_Quit();
}





















