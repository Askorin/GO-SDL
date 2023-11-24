# GO-SDL
Para compilar con gcc, correr en terminal:

```bash
gcc main.c init_funcs.c termination_funcs.c matrix_ops.c move_validation.c bit_ops.c player_input_processing.c rendering.c game_stats_funcs.c game_states.c buttons.c game_saving.c bot.c -o main -Ipath -lSDL2 -lSDL2_ttf -lm
```

- Donde path es el directorio en donde está SDL.h, probablemente: ./usr/include
