#include <stdio.h>
#include <stdlib.h>

#define LINE 3
#define COL 3

int *cria_matriz (){
    int *matriz = malloc (LINE * COL * sizeof (int));

    if (!matriz)
        return NULL;

    for (int i = 0; i < LINE; i++){
        for (int j = 0; j < COL; j++){
            matriz[(i * COL) + j] = 0;
        }
    }

    return matriz;
}

int *mult_matriz (int *mat_1, int *mat_2){
    int *resultado = cria_matriz ();

    if (!resultado)
        return NULL;

    if (!resultado)
        return NULL;

    for (int i = 0; i < LINE; i++){
        for (int j = 0; j < COL; j++){
            resultado[(i * COL) + j] = mat_1[(i * COL) + j] * mat_2[(i * COL) + j];
        }
    }

    return resultado;
}

int main (){
    int *matriz_a = cria_matriz ();
    int *matriz_b = cria_matriz ();
    int *multi;

    // ler matriz A
    for (int i = 0; i < LINE; i++){
        for (int j = 0; j < COL; j++){
            scanf ("%d", &matriz_a[(i * COL) + j]);
        }
    }

    // ler matriz B
    for (int i = 0; i < LINE; i++){
        for (int j = 0; j < COL; j++){
            scanf ("%d", &matriz_b[(i * COL) + j]);
        }
    }

    multi = mult_matriz (matriz_a, matriz_b);

    for (int i = 0; i < LINE; i++){
        for (int j = 0; j < COL; j++){
            printf ("%d ", multi[(i * COL) + j]);
        }
    }

    free (matriz_a);
    free (matriz_b);
    free (multi);

    return 0;
}