#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

/* Incializa SDL y crea una ventana */
bool init_sdl();

/* Carga archivos media */
bool load_board();

/* Libera media y cierra SDL */
void close_sdl();

/* Carga la imagen del tablero a la pantalla */
bool render_board();

void update_game_state();

/* Ventana a la que renderizamos */
SDL_Window* g_window = NULL;

/* Superficie en la ventana */
SDL_Surface* g_screen_surface = NULL;

/* La imagen a mostrar en la pantalla */
SDL_Surface* g_image = NULL;

/* Texturas piezas */
SDL_Texture* white_pc = NULL;
SDL_Texture* black_pc = NULL;




int main(int argc, char** argv)
{
    /* Inicializamos SDL y creamos la ventana */
    if (init_sdl()) {

        /* Cargamos imagen del tablero 9x9 */
        if (load_board()) {
            /* Aplicamos la imagen a la superficie de la ventana */
            if (render_board()) {
                SDL_UpdateWindowSurface(g_window);
            }

            int board_state[9][9] = {{0}};
            bool quit = false;
            /* 
             * SDL_PollEvent retorna 0 si no hay eventos pendientes disponibles, en caso contrario,
             * retorna 1.
             */
            while (!quit) {
                SDL_Event event;
                SDL_Point mouse_position;
                SDL_GetMouseState(&mouse_position.x, &mouse_position.y);
                while (SDL_PollEvent(&event) != 0) {
                    switch (event.type) {
                        /* El usuario pide salir */
                        case SDL_QUIT:
                            quit = true;
                            break;
                        /* Se registra un click izquierdo down del usuario */
                        case SDL_MOUSEBUTTONDOWN:
                            SDL_MouseButtonEvent* mouse_event = &event.button;
                            printf("mouse click x = %i y = %i\n", mouse_event->x,
                                    mouse_event->y);
                            break;
                    }
                }
                // update_game_state();
                SDL_UpdateWindowSurface(g_window);
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
        else {
            /* Conseguimos la superficie de la ventana*/
            g_screen_surface = SDL_GetWindowSurface(g_window);
        }
    }

    return success;
}

bool load_board()
{
    /* Flag de éxito para la carga de contenido media */
    bool success = true;

    /* Cargamos una imagen splash */
    g_image = SDL_LoadBMP("./assets/game_board_9x9.bmp");
    if (!g_image) {
        printf("Imagen de tablero no pudo ser cargada. SDL_ERROR: %s\n", SDL_GetError());
        success = false;
    }
    
    return success;
}

bool render_board()
{
    /* Flag de éxito para la renderización inicial del tablero */
    bool success = true;
    if (SDL_BlitSurface(g_image, NULL, g_screen_surface, NULL) < 0) {
        printf("No se pudo copiar la imagen del tablero a la superficie de la ventana. SDL_ERROR: %s\n",
                SDL_GetError());
        success = false;
    }
    return success;
}

bool create_piece_textures()
{


}

void update_game_state(int** game_arr, int len)
{
    /* Flag de éxito para la actualización del estado del juego */
    for (int i = 0; i < len; ++i) {
        for (int j = 0; j < len; ++j) {
            if (game_arr[i][j] == 0) {
                // remover pieza del punto en caso de existir.
            }
            else if (game_arr[i][j] == 1) {
                // draw_white_piece();
            }
            else if (game_arr[i][j] == 2) {
                // draw_black_piece();
            }
        }
    }
    
}

void close_sdl()
{
    /* La superficie es liberada de la memoria */
    SDL_FreeSurface(g_image);
    g_image = NULL;
    /* La superficie de la ventana es liberada cuando destruimos esta */
    SDL_DestroyWindow(g_window);
    g_window = NULL;
    g_image = NULL;

    /* Finalizamos SDL */
    SDL_Quit();
}




















