# GO-SDL
Para compilar con gcc, correr en terminal:

```bash
gcc main.c init_funcs.c termination_funcs.c matrix_ops.c move_validation.c bit_ops.c player_input_processing.c rendering.c game_stats_funcs.c game_states.c buttons.c game_saving.c -o main -Ipath -lSDL2main -lSDL2 -lm
```

- Donde path es el directorio en donde está SDL.h, en mi caso: ./usr/include
