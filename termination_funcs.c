#include "headers/termination_funcs.h"

void free_texture_ptrs(SDL_Texture* textures[OBJ_QTY])
{
    printf("Dejando en nulo punteros a texturas...\n");
    for (int id = 0; id < end_enum; ++id) {
        textures[id] = NULL;
    }
}

void close_sdl(SDL_Window* window_ptr, SDL_Renderer* renderer_ptr)
{
    printf("Cerrando aplicación.\n");
    /* Destruimos el rednererl esto libera las texturas de la memoria igualmente */
    if (renderer_ptr) {
        SDL_DestroyRenderer(renderer_ptr);
        renderer_ptr = NULL;
    } else {
        printf("Error: %s\n", SDL_GetError());
    }
    
    /* Se destruye la ventana de juego */
    if (window_ptr) {
        SDL_DestroyWindow(window_ptr);
        window_ptr = NULL;
    } else {
        printf("Error. %s\n", SDL_GetError());
    }

    /* Finalizamos SDL */
    SDL_Quit();
}

