#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

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

/* Carga archivos media y texturas piezas de juego */
bool load_pcs(SDL_Surface**, SDL_Surface**, SDL_Texture**, SDL_Texture**, SDL_Renderer**);

/* Carga archivos media y texturas tablero */
bool load_board(SDL_Surface**, SDL_Texture**, SDL_Renderer**);

/* Libera las texturas del tablero, piezas de juego, etc */
void free_texture_ptrs(SDL_Texture**, SDL_Texture**, SDL_Texture**);

/* Libera window, renderer y cierra SDL */
void close_sdl();


/* Renderiza las piezas en base a la matriz de juego */
void render_game_state(int len, int game_arr[len][len], SDL_Renderer**, SDL_Texture**,
        SDL_Texture**);

int main(int argc, char** argv)
{
    
    /* Ventana de juego */
    SDL_Window* g_window = NULL;

    SDL_Renderer* renderer = NULL;

    /* Inicializamos SDL y creamos la ventana */
    if (init_sdl(&g_window)) {

        /* Creamos el renderer */ 
        if (create_renderer(&g_window, &renderer)) {

            /* Superficies y texturas de tablero y piezas. */
            SDL_Surface *board_surface = NULL, *white_pc_surface = NULL, *black_pc_surface = NULL;
            SDL_Texture *board_texture = NULL, *white_pc_texture = NULL, *black_pc_texture = NULL;

            /* Cargamos el tablero y guardamos a lo que valua en un bool para luego evaluarlo... */ 
            bool board_is_loaded = load_board(&board_surface, &board_texture, &renderer);
            /* Cargamos las piezas y guardamos lo que valua en un bool para luego evaluarlo... */
            bool pieces_are_loaded = load_pcs(&white_pc_surface, &black_pc_surface,
                    &white_pc_texture, &black_pc_texture, &renderer);

            if (board_is_loaded && pieces_are_loaded) {
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
                    for (int j = 0; j < len; ++j) {
                        game_arr[i][j] = 0;
                    }
                }

                unsigned int prev_ms = 0;
                /* Gameloop principal empieza aquí */
                //int dummy_ctr = 0;
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
                                // game_arr[dummy_ctr][dummy_ctr] = dummy_ctr % 2 + 1;
                                // ++dummy_ctr;
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
                    
                    SDL_RenderCopy(renderer, board_texture, NULL, &board_rectangle);
                    
                    render_game_state(len, game_arr, &renderer, &white_pc_texture,
                            &black_pc_texture);

                    SDL_RenderPresent(renderer);
                }
            }

            free_texture_ptrs(&board_texture, &white_pc_texture, &black_pc_texture);
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

bool load_pcs(SDL_Surface** white_pc_surface_ptr, SDL_Surface** black_pc_surface_ptr,
        SDL_Texture** white_pc_texture_ptr, SDL_Texture** black_pc_texture_ptr,
        SDL_Renderer** renderer_ptr)
{
    /* Flag de éxito para la carga de piezas */ 
    bool success = true;

    /* Cargamos imagenes de las piezas */ 
    *white_pc_surface_ptr = SDL_LoadBMP("./assets/white_piece.bmp");
    *black_pc_surface_ptr = SDL_LoadBMP("./assets/black_piece.bmp");

    if (!(*white_pc_surface_ptr && *black_pc_surface_ptr)) {
        printf("Imagen de piezas no pudo ser cargada. SDL_ERROR: %s\n", SDL_GetError());
        success = false;
    } else {
        *white_pc_texture_ptr = SDL_CreateTextureFromSurface(*renderer_ptr, *white_pc_surface_ptr);
        *black_pc_texture_ptr = SDL_CreateTextureFromSurface(*renderer_ptr, *black_pc_surface_ptr);
        SDL_FreeSurface(*white_pc_surface_ptr);
        SDL_FreeSurface(*black_pc_surface_ptr);
        *white_pc_surface_ptr = NULL;
        *black_pc_surface_ptr = NULL;
    }
    if (!(*white_pc_texture_ptr && *black_pc_texture_ptr)) {
        printf("Texturas de piezas no pudieron ser cargada. SDL_ERROR: %s\n", SDL_GetError());
        success = false;
    }
    return success;
}

bool load_board(SDL_Surface** board_surface_ptr, SDL_Texture** board_texture_ptr, SDL_Renderer** 
        renderer_ptr)
{
    /* Flag de éxito para la carga del tablero */ 
    bool success = true;

    /* Cargamos imagen del tablero */
    *board_surface_ptr = SDL_LoadBMP("./assets/game_board_9x9.bmp");
    if (!(*board_surface_ptr)) {
        printf("Imagen del tablero no pudo ser cargada. SDL_ERROR: %s\n", SDL_GetError());
        success = false;
    }
    else {
        *board_texture_ptr = SDL_CreateTextureFromSurface(*renderer_ptr, *board_surface_ptr);
        SDL_FreeSurface(*board_surface_ptr);
        *board_surface_ptr = NULL;
    }
    return success;
}


void render_game_state(int len, int game_arr[len][len], SDL_Renderer** renderer_ptr,
        SDL_Texture** white_pc_texture_ptr, SDL_Texture** black_pc_texture_ptr)
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
                SDL_RenderCopy(*renderer_ptr, *white_pc_texture_ptr, NULL, &pc_rectangle);
            }
            else if (game_arr[i][j] == 2) {
                /* Dibujamos pieza negra en coordenada correspondiente */
                SDL_RenderCopy(*renderer_ptr, *black_pc_texture_ptr, NULL, &pc_rectangle);
            }
        }
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

void free_texture_ptrs(SDL_Texture** board_texture_ptr, SDL_Texture** white_pc_texture_ptr,
        SDL_Texture** black_pc_texture_ptr)
{
    printf("Dejando en nulo punteros a texturas...\n");
    *white_pc_texture_ptr = NULL;
    *black_pc_texture_ptr = NULL;
    *board_texture_ptr = NULL;
}




















