#include "headers/rendering.h"
#include "headers/enums.h"

void render_menu_buttons(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY],
        SDL_Rect* play_btn_rec, SDL_Rect* opt_btn_rec)
{
        
    SDL_RenderCopy(renderer, textures[play_btn], NULL, play_btn_rec);
    SDL_RenderCopy(renderer, textures[opt_btn], NULL, opt_btn_rec);

}

void render_game_state(int len, int game_arr[len][len], SDL_Renderer* renderer,
        SDL_Texture* textures[OBJ_QTY], SDL_Rect* window_rectangle)
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
    
    /* Limpiar la pantalla */
    SDL_RenderClear(renderer);

    /* Renderizamos el tablero */ 
    SDL_RenderCopy(renderer, textures[board], NULL, window_rectangle);

    /* Flag de éxito para la actualización del estado del juego */
    for (int i = 0; i < len; ++i) {
        for (int j = 0; j < len; ++j) {
            if (game_arr[i][j] == 0) continue;
            
            pc_rectangle.x = 363 + j*69 - pc_rectangle.w / 2.0;
            pc_rectangle.y = 82 + i*69 - pc_rectangle.h / 2.0;

            if (game_arr[i][j] == 1) {
                /* Dibujamos pieza negra en coordenada correspondiente */
                SDL_RenderCopy(renderer, textures[black_pc], NULL, &pc_rectangle);
            }
            else if (game_arr[i][j] == 2) {
                /* Dibujamos pieza blanca en coordenada correspondiente */
                SDL_RenderCopy(renderer, textures[white_pc], NULL, &pc_rectangle);
            }
        }
    }

    SDL_RenderPresent(renderer);
    
}
