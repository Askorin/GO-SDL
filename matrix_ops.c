#include "headers/matrix_ops.h"

bool matrices_are_equal(int len, int matrix1[19][19], int matrix2[19][19])
{
    for (int i = 0; i < len; ++i) {
        for (int j = 0; j < len; ++j) {
            if (matrix1[i][j] != matrix2[i][j])
                return false;
        }
    }
    return true;
}

void set_matrix(int len, int matrix[19][19], int n)
{
    for (int i = 0; i < len; ++i) {
        for(int j = 0; j < len; ++j) matrix[i][j] = n;
    }
}

void copy_matrix(int len, int source[19][19], int target[19][19])
{
    for (int i = 0; i < len; ++i) {
        for (int j = 0; j < len; ++j) target[i][j] = source[i][j];
    }
}

void print_matrix(int len, int matrix[19][19])
{
    for (int i = 0; i < len; ++i) {
        for (int j = 0; j < len; ++j) printf("%d ", matrix[i][j]);
        printf("\n");
    }
}

void count_pieces(int len, int matrix[19][19], int target[2])
{
    /* Asegurarse no es malo */
    target[0] = 0;
    target[1] = 0;
    for (int i = 0; i < len; ++i) {
        for (int j = 0; j < len; ++j) {
            if (matrix[i][j] == 1) target[0] += 1;
            else if (matrix[i][j] == 2) target[1] += 1;
        }
        
    }
}

void correct_diff(int len, int n, int source[19][19], int target[19][19])
{
    for (int i = 0; i < len; ++i) {
        for (int j = 0; j < len; ++j) {
            if (source[i][j] == n && target[i][j] != n) {
                target[i][j] = n;
            }
        }
    }
}

void count_territory(int game_arr[19][19], int output[2], int len)
{
    int territory_matrix[19][19] = {{0}};
    change_territory(game_arr, territory_matrix, len);
    count_pieces(len, territory_matrix, output);
}

int is_valid_floodfill(int matrix[19][19], int len, int x, int y, int old_value, int new_value)
{
	if (x < 0 || x >= len || y < 0 || y >= len || matrix[x][y] != old_value || matrix[x][y] == new_value)
		return 0;
	return 1;
}

void flood_fill(int matrix[19][19], int len, int x, int y, int old_value, int new_value)
{
	struct coords
	{
		int x_axis;
		int y_axis;
	};

	struct coords cola[len*len];
	int front = 0;
	int rear = 0;

	// agregar las coordenadas de inicio
	struct coords p = { x, y };
	cola[rear++] = p;

	//Cambia el old_value de la coord con new_value
	matrix[x][y] = new_value;

	//Mientras la cola no esté vacía, es decir, toda la coord que tiene el 
	//old_value no haya cambiado a new_value
	while (front < rear)
	{
		// quitar de la cola la coord front
		struct coords currPos = cola[front++];

		int posX = currPos.x_axis;
		int posY = currPos.y_axis;

		// verificar si las posiciones adyacentes son validas
		if (is_valid_floodfill(matrix, len, posX + 1, posY, old_value, new_value))
		{
			// cambiar el valor por new_value si es valido y meterlo en la cola
			matrix[posX + 1][posY] = new_value;
			struct coords p = { posX + 1, posY };
			cola[rear++] = p;
		}

		if (is_valid_floodfill(matrix, len, posX - 1, posY, old_value, new_value))
		{
			matrix[posX - 1][posY] = new_value;
			struct coords p = { posX - 1, posY };
			cola[rear++] = p;
		}

		if (is_valid_floodfill(matrix, len, posX, posY + 1, old_value, new_value))
		{
			matrix[posX][posY + 1] = new_value;
			struct coords p = { posX, posY + 1 };
			cola[rear++] = p;
		}

		if (is_valid_floodfill(matrix, len, posX, posY - 1, old_value, new_value))
		{
			matrix[posX][posY - 1] = new_value;
			struct coords p = { posX, posY - 1 };
			cola[rear++] = p;
		}
	}
}

void change_territory(int matrix[19][19], int territory[19][19], int len)
{
	int matrix_copy[19][19];
	int value = 10;

    for (int i=0;i<len;i++) {
        for(int j=0;j<len;j++) {
            matrix_copy[i][j] = matrix[i][j];
        }
    }

	//Pintamos cada espacio que tiene 0 con un value, ademas guardamos el ultimo area, en la variable value
    for (int i=0;i<len;i++) {
        for(int j=0;j<len;j++) {
            if(matrix_copy[i][j] == 0) {
                flood_fill(matrix_copy, len, i, j, 0, value);
                value++;
            }
        }        
    }

	//LLenamos una matrix de len+2 por len+2 con 7(numero arbritrario) y luego ponemos la matriz pintada dentro, dejando 
	//los bordes con el numero repretante del borde 
	int matrix_borders[21][21];
	for (int i=0;i<21;i++) {
        for(int j=0;j<21;j++) {
            matrix_borders[i][j] = 7;
        }
    }
	for (int i=0;i<len;i++) {
        for(int j=0;j<len;j++) {
            matrix_borders[i+1][j+1] = matrix_copy[i][j];
        }
    }

	for (int area_value=10; area_value < value; area_value++) {
		int cant_area=0;

		//vemos cuantar veces se repite el valor del area en la matriz y guardamos la cantidad en la variable cant_area
		for (int i=0;i<len;i++) {
			for (int j=0;j<len;j++) {
				if (matrix_borders[i+1][j+1]==area_value)
					 cant_area++;
			}
    	}
		
		// verificamos si los bloques contiguos de cada bloque son player, borde(7) o area_value, si de los 4 lados 
		// pertenece a alguno de estos grupos, lo suma a el territorio del player(cant_player_terr).
		for (int player=1; player<=2; player++) {
			int cant_player_terr = 0;

			for (int i=1;i<len+1;i++) {
				for (int j=1;j<len+1;j++) {
					if (matrix_borders[i][j]==area_value) {
						if ( (matrix_borders[i+1][j] == area_value || matrix_borders[i+1][j] == player || matrix_borders[i+1][j] == 7) &&
			 				 (matrix_borders[i-1][j] == area_value || matrix_borders[i-1][j] == player || matrix_borders[i-1][j] == 7) &&
							 (matrix_borders[i][j+1] == area_value || matrix_borders[i][j+1] == player || matrix_borders[i][j+1] == 7) &&
							 (matrix_borders[i][j-1] == area_value || matrix_borders[i][j-1] == player || matrix_borders[i][j-1] == 7)   )
							 	cant_player_terr++;
					}
				}
			}

			//si las veces que se repite area_value es igual al territorio de uno de los dos players(cant_player_terr),
			//remplazamos cambiamos el valor del terrorio en las posiciones de la matriz territory
			if (cant_player_terr==cant_area) {
				for (int i=0;i<len;i++) {
					for (int j=0;j<len;j++) {
						if (matrix_borders[i+1][j+1]==area_value)
							territory[i][j] = player;
					}
    			}
			}
		}
	}
	
}



