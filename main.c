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
 * - Implementar casos de suicidio.
 * - Implementar completamente el sistema de Ko de Martín (ya se puede)
 * -  
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

/* Función de estado de juego en tablero, retorna el jugador que debe hacer el siguiente turno */
int play(SDL_Renderer*, SDL_Texture*[OBJ_QTY], int len, int[len][len], state_type*, SDL_Rect*, int);

/* Setea todos los valores de la matriz a un valor entero */
void set_matrix(int len, int[len][len], int);

/* Compara dos matrices, útil en la regla del KO */
bool boards_are_equal(int len, int[len][len], int[len][len]);

/* Revisa si el click izquierdo colisionó con un "hitbox" válido */
bool check_mdown(int len, int[len][len], SDL_MouseButtonEvent*, int, int*, int*);

/* Procesa un movimiento. Retorna true si todo salió correctamente y debe jugar el otro jugador. */
bool process_move(int len, int[len][len], SDL_MouseButtonEvent*, int);

/* Renderiza las piezas en base a la matriz de juego */
void render_game_state(int len, int[len][len], SDL_Renderer*, SDL_Texture*[OBJ_QTY], SDL_Rect*);

/* Libera las texturas del tablero, piezas de juego, etc */
void free_texture_ptrs(SDL_Texture*[OBJ_QTY]);

/* 
 * Para guardar las libertades de una pieza, podemos usar un bitmap, o bitarray. Como en el peor de
 * los casos tenemos un tablero de 19x19 posiciones, tenemos como mucho 361 posibles ubicaciones.
 * Lo subimos a 384 para que sea múltiplo de 32 (cantidad de bits en un int), y tenemos que
 * necesitamos un arreglo de 12 números int para guardar este bitmap.
 */

/* Setea el bit en posición k a 1. en un bitmap array */
void set_bit(unsigned int[12], int k);

/* Retorna el valor del bit en la posición k de un bitmap array */
int get_bit(unsigned int[12], int k);

/* Dado un tablero, una columna y una fila, chequea las libertades de la pieza en las coords */
int get_liberties(int len, int[len][len], int, int);

/* 
 * Chequea las coordenadas adyacentes de una fila y columna en el tablero, se podría decir que es
 * función hija de get_liberties, funciona de manera recursiva
 */
void check_adj(int len, int[len][len], int, int, unsigned int[12], unsigned int[12], int*);

/* Aplicamos una transformación para pasar de fila-columna a un entero. de 0 a lenxlen */
int coords_to_int(int, int, int);

/* Encuentra las piezas muertas (sin libertades) en el tablero */
void find_dead_pieces(int len, int[len][len]);

void print_game_arr(int len, int[len][len]);


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

void set_matrix(int len, int game_arr[len][len], int n)
{
    for (int i = 0; i < len; ++i) {
        for(int j = 0; j < len; ++j) game_arr[i][j] = n;
    }
}

bool boards_are_equal(int len, int prev_board[len][len], int temp_board[len][len])
{
    for (int i = 0; i < len; ++i) {
        for (int j = 0; j < len; ++j) {
            if (prev_board[i][j] != temp_board[i][j])
                return false;
        }
    }
    return true;
}

bool process_move(int len, int game_arr[len][len], SDL_MouseButtonEvent* mouse_event, int player)
{
    int row, col;
    /* Flag de movimiento que cumple todas las condiciones para ser efectuado */
    bool success = false;

    /* Chequeamos si la jugada se puede mapear a una cordenada de la matriz de tablero. */
    if (check_mdown(len, game_arr, mouse_event, player, &row, &col)) {
        /* Ahora, chequear si la jugada es suicidio, aquí el camino se bifurca en 3:
         * 1 - La jugada es suicidio y no hace capturas, entonces no se procesa y se retorna false.
         * 2 - La jugada no es suicidio, entonces se procesa normalmente, se chequea Ko.
         * 3 - La jugada es suicidio y hace capturas, en este caso se debe guardar las coordenadas
         *     de la figura que se "suicidó", y flagearla para ser ignorada. Pero no parar ahí, 
         *     cualquier figura que haya estado enlazada a esta tampoco puede ser capturada.
         *     En ese sentido, quizás sería mejor poner una pieza "falsa" en ese punto, con tal
         *     de que se borren las libertades que queremos, y después poner las piezas de verdad.
         *     Se me ocurre poner un número de la misma paridad de la pieza que fue jugada. Es de
         *     cir, si se jugó una piedra negra (1), se pone un 3. Después modificar el código a 
         *     que si una piedra tiene como libertad a una pieza con número distinta a ella pero de
         *     de su misma paridad (excluimos al 0), entonces esa posición sigue siendo libertad,
         *     y tampoco se enlaza con ella. 
         */
        success = true;
        game_arr[row][col] = player;
        find_dead_pieces(len, game_arr);
       
    } 

    return success;
}


bool check_mdown(int len, int game_arr[len][len], SDL_MouseButtonEvent* mouse_event, int player,
        int* row, int* col)
{

    /* Flag de validez del movimiento */
    bool is_valid = false;

    /* Se inicializa variable correspondiente a la coordenada de la esquina superior izquierda */
    int currentx= 363, currenty = 82;
    int sqrside = 557 / (len - 1);

    /* Verifica casilla por casilla si el mouse hizo click en la hitbox */
    for (int i = 0; i < len; i++) {
        currentx = 363;
        for (int j = 0; j < len; j++) {
            if (currentx - sqrside / 2 < (mouse_event->x) &&
                    (mouse_event->x) < currentx + sqrside / 2 &&
                    currenty-sqrside / 2 < (mouse_event->y) &&
                    (mouse_event->y) < currenty + sqrside / 2 &&
                    game_arr[i][j] == 0) {

                /* 
                 * TODO: A futuro, en el momento en que se identifica una "colisión", se podría re-
                 * visar que el movimiento es válido. En ese caso, esta función solo se dedicaría a
                 * chequear colisión.
                 */
                *row = i;
                *col = j;
                is_valid = true;
                /* Chequeamos las piezas muertas del tablero */
                //printf("Checkeando con (%d, %d)\n", i, j);
                //print_game_arr(len, game_arr);
                //find_dead_pieces(len, game_arr);
                goto exit;
            }
            currentx += sqrside;
        }
        currenty+=sqrside;
    }

    if (!is_valid) printf("Error. Movimiento no se encuentra dentro del tablero.\n");

    exit:
    return is_valid;
}

void print_game_arr(int len, int game_arr[len][len])
{
    for (int i = 0; i < len; ++i) {
        for (int j = 0; j < len; ++j) printf("%d ", game_arr[i][j]);
        printf("\n");
    }
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

int coords_to_int(int row, int col, int len) {
    return row * len + col; 
}

void find_dead_pieces(int len, int game_arr[len][len])
{
    int captured[len*len][2];
    int count = 0;

    for (int i = 0; i < len; ++i) {
        for (int j = 0; j < len; ++j) {
            if (game_arr[i][j] != 0) {
                /* 
                 * Hay que tener cuidado de no eliminar la pieza inmediatamente, ya que si lo
                 * hacemos, cuando se chequeen las libertades de las otras piezas, podrían
                 * terminar vivas cuando en verdad estaban muertas.
                 * Por eso, guardaermos las coordenadas de las piezas en un arreglo a recorrer
                 * después.
                 */
                if (!get_liberties(len, game_arr, i, j)) {
                    captured[count][0] = i;
                    captured[count][1] = j;
                    ++count;
                }
            }
        }
    }

    for (int i = 0; i < count; ++i) {
        int row = captured[i][0];
        int col = captured[i][1]; 
        game_arr[row][col] = 0;;
    }
}

int get_liberties(int len, int game_arr[len][len], int row, int col)
{
    /* 
     * Creamos un bitmap array de libertades y coordenadas visitadas, además de un entero que
     * mantendrá cuenta de la cant de libertades de una pieza
     */
    unsigned int liberties[12] = {0}; 
    unsigned int visited[12] = {0};
    int cant_libertades = 0;
    /* Llamamos a chequear las libertades de la pieza */
    check_adj(len, game_arr, row, col, liberties, visited, &cant_libertades);
    return cant_libertades; 
}

void check_adj(int len, int game_arr[len][len], int row, int col, unsigned int liberties[12],
        unsigned int visited[12], int* cant_libertades)
{
    int directions[4][2] = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};
    int coords_en_num = coords_to_int(row, col, len);

    /* Está coordenara está oficialmente visitada. */
    set_bit(visited, coords_en_num);
    for (int i = 0; i < 4; ++i) {
        /* Chequeamos las coordenadas adyacantes */
        int adj_row = row + directions[i][0], adj_col = col + directions[i][1];

        /* Las transformamos en un entero, para poder encontrarlas en los arreglos de bitmaps */
        int adj_coords_en_num = coords_to_int(adj_row, adj_col, len);

        /* Revisamos que las coordenadas adyacentes estén dentro del rango del tablero. */
        if (adj_row >= 0 && adj_row < len && adj_col >= 0 && adj_col < len) {

            /*
             * Si la coordenada está vacía, es decir, no hay pieza puesta en ella, revisamos si es
             * candidata a ser añadida a libertades
             */
            if (game_arr[adj_row][adj_col] == 0) {
                /*
                 * Si coordenada no está en libertades, la añadimos a libertades y sumamos al
                 * contador
                 */
                if (!get_bit(liberties, adj_coords_en_num)) {
                    *cant_libertades += 1;
                }
                set_bit(liberties, adj_coords_en_num);
                
            }
            /*
             * En caso de que la coordenada adyacente no está vacía, es del mismo color, y además
             * no ha sido visitada anteriormente, chequeamos sus libertades
             */
            else if (game_arr[adj_row][adj_col] == game_arr[row][col] &&
                    !get_bit(visited, adj_coords_en_num)) {
                check_adj(len, game_arr, adj_row, adj_col, liberties, visited, cant_libertades);
            }
        }
    }
}

void set_bit(unsigned int bitmap[12], int k)
{
    /* Para saber en qué indice del arreglo está la posición que nos pide, hacemos: */
    int idx = k / 32;
    /* Para saber saber cuál es la posición relativa del bit en ese entero del arreglo, hacemos: */
    int rel_pos = k % 32;
    /* Ahora seteamos el bit en la posición rel_pos del int bitmap[idx] a 1. */ 
    unsigned int num = bitmap[idx];
    num = num | (1 << rel_pos);
    bitmap[idx] = num;
}

int get_bit(unsigned int bitmap[12], int k)
{
    /* Para saber en qué indice del arreglo está la posición que nos pide, hacemos: */
    int idx = k / 32;
    /* Para saber saber cuál es la posición relativa del bit en ese entero del arreglo, hacemos: */
    int rel_pos = k % 32;
    /* Ahora conseguimos el bit en la posición rel_pos del int bitmap[idx] */
    unsigned int num = bitmap[idx];
    num = num >> rel_pos;
    num = num & 1;
    return num;
}



















