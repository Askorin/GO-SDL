#ifndef BIT_OPS_H
#define BIT_OPS_H
#include <stdio.h>


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

#endif

