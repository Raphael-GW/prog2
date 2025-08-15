#include <stdlib.h>
#include <stdio.h>
#include "jukefila.h"


jukefila* criar_jukefila(){
    jukefila *fila = malloc (sizeof (jukefila));
    
    if (!fila)
        return NULL
        
    fila->inicio = NULL;
    fila->final = NULL;
    
    return fila
}

void inserir_jukefila(pedido* elemento, jukefila* fila){
    if (!elemento || !fila)
        return ;
        
    //fila vazia
    if (!fila->inicio || !fila->final){
        fila->inicio = elemento;
        fila->final = elemento;
        return ;
    }
    
    //inserir no inicio
    if (elemento->valor < fila->inicio->valor){
        elemento->proximo = fila->inicio;
        fila->inicio->anterior = elemento;
        fila->inicio = elemento;
        return ;
    }
    
    //inserir no final
    if (elemento->valor > fila->final->valor){
        elemento->anterior = fila->final;
        fila->final->proximo = elemento;
        fila->final = elemento;
        return ;
    }
    
    //inserir no meio
    pedido *aux = fila->inicio;
    for (int)
}
pedido* consumir_jukefila(jukefila* fila);
unsigned int contar_jukefila(jukefila* fila);
void destruir_jukefila(jukefila *fila);