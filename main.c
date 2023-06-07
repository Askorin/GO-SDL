#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

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

/* Incializa SDL y crea una ventana */
bool init_sdl(SDL_Window**);

/* Crea el renderer */
bool create_renderer(SDL_Window**, SDL_Renderer**);

/* Carga todas las superficies y texturas necesarias */
bool load_surfaces_and_textures(SDL_Surface*[OBJ_QTY], SDL_Texture*[OBJ_QTY], char[OBJ_QTY][BMP_LEN],
        SDL_Renderer*); 

/* Gameloop del menu */
void open_menu();

/* Gameloop juego en tablero */
void start_game();

/* Renderiza las piezas en base a la matriz de juego */
void render_game_state(int len, int game_arr[len][len], SDL_Renderer*, SDL_Texture*[OBJ_QTY]);

/* Libera las texturas del tablero, piezas de juego, etc */
void free_texture_ptrs(SDL_Texture*[OBJ_QTY]);

/* Libera window, renderer y cierra SDL */
void close_sdl();

/* Enums para texturas y superficies, solo para que no hayan números mágicos dando vuelta. */
typedef enum
{
    board = 0,
    white_pc,
    black_pc
}sf_and_txt_enum;



int main(int argc, char** argv)
{
    
    /* Ventana de juego */
    SDL_Window* g_window = NULL;

    /* Renderer para la ventana */
    SDL_Renderer* renderer = NULL;

    /* Inicializamos SDL y creamos la ventana */
    if (init_sdl(&g_window)) {

        /* Creamos el renderer */ 
        if (create_renderer(&g_window, &renderer)) {

            /* Creamos arreglos para las superficies, texturas, y sus bmps en el archivo assets. */
            SDL_Surface* surfaces[OBJ_QTY] = {NULL};
            SDL_Texture* textures[OBJ_QTY] = {NULL}; 
            char img_paths[OBJ_QTY][BMP_LEN] = {
                "./assets/game_board_9x9.bmp",
                "./assets/white_piece.bmp",
                "./assets/black_piece.bmp"
            };

            /* Intentamos cargar las superficies y texturas */
            if (load_surfaces_and_textures(surfaces, textures, img_paths, renderer)) {

                /* MENU GAME LOOP */
                open_menu();
                bool quit = false; /* Gameloop seguirá mientras la flag sea false */
                int len = 9; /* Tamaño del tablero, a modificar en el menú más adelante. */

                /* Definimos un rectángulo para renderizar el tablero en base a este */
                SDL_Rect board_rectangle;
                board_rectangle.x = 0;
                board_rectangle.y = 0;
                board_rectangle.w = RENDER_W;
                board_rectangle.h = RENDER_H;
                                            
                /* 
                 * Como game_arr es un arreglo de orden variable, no podemos setearlo a 0 inmediata
                 * mente, asi que lo hacemos manualmente. 
                 */
                int game_arr[len][len];
                for (int i = 0; i < len; ++i) {
                    for (int j = 0; j < len; ++j) game_arr[i][j] = 0;
                }

                unsigned int prev_ms = 0;
                /* Gameloop principal empieza aquí */
                int dummy_ctr = 0;
                while (!quit) {
                    /* Queremos que cada 1000 / F_CAP milisegundos ocurra un frame */ 
                    unsigned int current_ms = SDL_GetTicks64();
                    /* Delta es cant. de milisegundos desde último frame */
                    unsigned int delta = current_ms - prev_ms; 
                    if (delta < 1000.0 / F_CAP && !prev_ms) continue;
                    //printf("Current time in ms is %li\n", SDL_GetTicks64());
                    prev_ms = current_ms;

                    /* Empezamos a procesar eventos con la variable event */
                    SDL_Event event;
                    SDL_Point mouse_position;
                    SDL_GetMouseState(&mouse_position.x, &mouse_position.y);

                    /* SDL_PollEvent retorna 0 si no hay eventos disponibles, si no, retorna 1. */
                    while (SDL_PollEvent(&event)) {
                        switch (event.type) {
                            /* El usuario pide salir del juego */
                            case SDL_QUIT:
                                quit = true;
                                break;
                            /* Se registra un click izquierdo down del usuario */
                            case SDL_MOUSEBUTTONDOWN: 
                                game_arr[dummy_ctr][dummy_ctr] = dummy_ctr % 2 + 1;
                                ++dummy_ctr;
                                SDL_MouseButtonEvent* mouse_event = &event.button;
                                printf("mouse click x = %i y = %i\n", mouse_event->x,
                                        mouse_event->y);
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
                    SDL_RenderCopy(renderer, textures[board], NULL, &board_rectangle);
                    /* Renderizamos en base al estado de la matriz de juego. */
                    render_game_state(len, game_arr, renderer, textures); 
                    
                    SDL_RenderPresent(renderer);
                }
            }

            free_texture_ptrs(textures);
        } 
    }

    /* Liberamos recursos y cerramos SDL */
    close_sdl(&g_window, &renderer);
    return 0;
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

bool create_renderer(SDL_Window** window_ptr, SDL_Renderer** renderer_ptr)
{
    /* Flag de éxito para la creación del renderer */
    bool success = true;
    *renderer_ptr = SDL_CreateRenderer(*window_ptr, -1, SDL_RENDERER_ACCELERATED);
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
    for (int id = board; id <= black_pc; ++id) {
        printf("path a bmp es %s\n", img_paths[id]);
        surfaces[id] = SDL_LoadBMP(img_paths[id]);
        if (!surfaces[id]) {
            printf("Superficie de indice %d en enum no pudo ser cargada. SDL_ERROR: %s\n", id,
                    SDL_GetError());
            success = false;
            break;
        }
    }
    
    /* En caso de que se hayan cargado las superficies de manera exitosa, procedemos a texturas. */
    if (success) {
        for (int id = board; id <= black_pc; ++id) {
            textures[id] = SDL_CreateTextureFromSurface(renderer, surfaces[id]);
            SDL_FreeSurface(surfaces[id]);
            surfaces[id] = NULL;
            if (!textures[id]) {
                printf("Texture de indice %d en enum no pudo ser cargada. SDL_ERROR: %s\n", id,
                        SDL_GetError());
                success = false;
                break;
            }
        }
    }

    return success;
}

void open_menu() {}

void start_game() {}

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
    for (int id = board; id < black_pc; ++id) {
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





















