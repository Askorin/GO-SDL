#include "headers/init_funcs.h"

/* Resolución ventana y renderizado */
extern const int SCREEN_WIDTH, SCREEN_HEIGHT, RENDER_W, RENDER_H;

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
        else if (TTF_Init() < 0) {
            printf("No se pudo inicializar TTF. TTF_Error: %s\n", TTF_GetError());
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

bool load_assets(SDL_Surface* surfaces[OBJ_QTY], SDL_Texture* textures[OBJ_QTY],
        char img_paths[OBJ_QTY][BMP_LEN], TTF_Font** font_ptr, SDL_Renderer* renderer)
{
    bool success = true;
    /* Cargamos imágenes de las superficies */ 
    printf("Cargando superficies desde bmp...\n");
    for (int id = 0; id < end_enum; ++id) {
        surfaces[id] = SDL_LoadBMP(img_paths[id]);
        if (!surfaces[id]) {
            printf("Superficie de indice %d en enum no pudo ser cargada. SDL_ERROR: %s\n", id,
                    SDL_GetError());
            success = false;
            break;
        }
    }
    
    /* En caso de que se hayan cargado las superficies de manera exitosa, procedemos a texturas. */
    printf("Cargando texturas a renderer...\n");
    if (success) {
        for (int id = 0; id < end_enum; ++id) {
            textures[id] = SDL_CreateTextureFromSurface(renderer, surfaces[id]);
            SDL_FreeSurface(surfaces[id]);
            surfaces[id] = NULL;
            if (!textures[id]) {
                printf("Texture de indice %d en enum no pudo ser cargada. SDL_ERROR: %s\n", id,
                        SDL_GetError());
                success = false;
                break;
            }
        //printf("bmp de textura cargada: %s\n", img_paths[id]);
        }
    }
    
    /* Intentamos cargar la fuente */
    *font_ptr = TTF_OpenFont("assets/fonts/ethnocentric_rg.ttf", 24);
    if (!(*font_ptr)) {
        printf("No se puedo cargar la fuente. TTF_Error: %s\n", TTF_GetError());
        success = false;
    }

    printf("Assets cargados.\n");

    return success;
}
