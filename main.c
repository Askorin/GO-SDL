#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

/* Resolución ventana */
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

/* Resolución lógica para renderizado */
const int RENDER_W = 1280;
const int RENDER_H = 720;

/* Incializa SDL y crea una ventana */
bool init_sdl();

/* Crea el renderer */
bool create_renderer();

/* Carga archivos media y texturas piezas de juego */
bool load_pcs();

/* Carga archivos media y texturas tablero */
bool load_board();

/* Libera media y cierra SDL */
void close_sdl();


/* Renderiza las piezas en base a la matriz de juego */
void render_game_state(int len, int game_arr[len][len]);

/* Ventana a la que renderizamos */
SDL_Window* g_window = NULL;

/* Renderer */
SDL_Renderer* renderer = NULL;

/* Superficies de piezas y tablero a mostar en pantalla*/ 
SDL_Surface* white_pc_surface = NULL; 
SDL_Surface* black_pc_surface = NULL;
SDL_Surface* board_surface = NULL;

/* Texturas de piezas y tablero a las que se cargaran las superficies anteriores */
SDL_Texture* white_pc_texture = NULL; 
SDL_Texture* black_pc_texture = NULL;
SDL_Texture* board_texture = NULL;

int main(int argc, char** argv)
{
    /* Inicializamos SDL y creamos la ventana */
    if (init_sdl()) {

        /* Creamos el renderer */ 
        if (create_renderer()) {
            if (load_board() && load_pcs()) {
                bool quit = false; /* Gameloop seguirá mientras la flag sea false */
                int len = 9; /* Tamaño del tablero, a modificar por el menú más adelante. */
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
                    for (int j = 0; j < len; ++j) {
                        game_arr[i][j] = 0;
                    }
                }

                /* Gameloop principal empieza aquí */
                while (!quit) {

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
                                SDL_MouseButtonEvent* mouse_event = &event.button;
                                // printf("mouse click x = %i y = %i\n", mouse_event->x,
                                //         mouse_event->y);
                                break;
                            /* Caso default, por buena onda */
                            default:
                                break;
                        }
                    }
                    
                    /* Manejamos updates al estado de juego */


                    /* Limpiar y dibujar a la pantalla */
                    SDL_RenderClear(renderer);
                    
                    SDL_RenderCopy(renderer, board_texture, NULL, &board_rectangle);
                    
                    render_game_state(len, game_arr);

                    SDL_RenderPresent(renderer);
                }
            }
        } 
    }

    /* Liberamos recursos y cerramos SDL */
    printf("Cerrando aplicación.\n");
    close_sdl();
    return 0;
}


bool init_sdl()
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
        g_window = SDL_CreateWindow("GO", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                SCREEN_WIDTH, SCREEN_HEIGHT, 0);
        if (!g_window) {
            printf("Window could not be created. SDL_ERROR: %s\n", SDL_GetError());
            success = false;
        }
    }

    return success;
}

bool create_renderer()
{
    /* Flag de éxito para la creación del renderer */
    bool success = true;
    if (!(renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED))) {
            printf("Renderer no pudo ser inicializado. SDL_ERROR: %s\n", SDL_GetError());
            success = false;
    }

    /* Intentamos setear una resolución lógica fija para renderizar, retorna negativo en error. */ 
    if (SDL_RenderSetLogicalSize(renderer, RENDER_W, RENDER_H) < 0) {
            printf("Resolución del renderer no pudo ser colocada. SDL_ERROR: %s\n", SDL_GetError());
    }
    return success;
}

bool load_pcs()
{
    /* Flag de éxito para la carga de piezas */ 
    bool success = true;

    /* Cargamos imagenes de las piezas */ 
    white_pc_surface = SDL_LoadBMP("./assets/white_piece.bmp");
    black_pc_surface = SDL_LoadBMP("./assets/black_piece.bmp");

    if (!(white_pc_surface && black_pc_surface)) {
        printf("Imagen de piezas no pudo ser cargada. SDL_ERROR: %s\n", SDL_GetError());
        success = false;
    } else {
        white_pc_texture = SDL_CreateTextureFromSurface(renderer, white_pc_surface);
        black_pc_texture = SDL_CreateTextureFromSurface(renderer, black_pc_surface);
        SDL_FreeSurface(white_pc_surface);
        SDL_FreeSurface(black_pc_surface);
        white_pc_surface = NULL;
        black_pc_surface = NULL;
    }
    if (!(white_pc_texture && black_pc_texture)) {
        printf("Texturas de piezas no pudieron ser cargada. SDL_ERROR: %s\n", SDL_GetError());
        success = false;
    }
    return success;
}

bool load_board()
{
    /* Flag de éxito para la carga del tablero */ 
    bool success = true;

    /* Cargamos imagen del tablero */
    board_surface = SDL_LoadBMP("./assets/game_board_9x9.bmp");
    if (!board_surface) {
        printf("Imagen del tablero no pudo ser cargada. SDL_ERROR: %s\n", SDL_GetError());
        success = false;
    }
    else {
        board_texture = SDL_CreateTextureFromSurface(renderer, board_surface);
        SDL_FreeSurface(board_surface);
        board_surface = NULL;
    }
    return success;
}


void render_game_state(int len, int game_arr[len][len])
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
                SDL_RenderCopy(renderer, white_pc_texture, NULL, &pc_rectangle);
            }
            else if (game_arr[i][j] == 2) {
                /* Dibujamos pieza negra en coordenada correspondiente */
                SDL_RenderCopy(renderer, black_pc_texture, NULL, &pc_rectangle);
            }
        }
    }
    
}

void close_sdl()
{
    /* Liberamos las texturas de la memoria al destruir el renderer */
    if (renderer) SDL_DestroyRenderer(renderer);
    renderer = NULL;
    white_pc_texture = NULL;
    black_pc_texture = NULL;
    board_texture = NULL;

    /* Se destruye la ventana de juego */
    if (g_window) SDL_DestroyWindow(g_window);
    g_window = NULL;

    /* Finalizamos SDL */
    SDL_Quit();
}




















