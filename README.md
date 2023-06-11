# GO-SDL
Para compilar con gcc, correr en terminal:
gcc main.c init_funcs.c termination_funcs.c matrix_ops.c move_validation.c bit_ops.c player_input_processing.c rendering.c -o main -Ipath -lSDL2main -lSDL2
- Donde path es el directorio en donde está SDL.h, en mi caso: ./usr/include
- De igual manera recomiendo flags -Wall y -g
