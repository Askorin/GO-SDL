#include "headers/termination_funcs.h"

void free_texture_ptrs(SDL_Texture* textures[OBJ_QTY])
{
    printf("Dejando en nulo punteros a texturas...\n");
    for (int id = board; id < end_enum; ++id) {
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

