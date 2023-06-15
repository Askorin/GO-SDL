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
