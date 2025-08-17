#include <stdlib.h>
#include <stdio.h>
#include "jukefila.h"


jukefila* criar_jukefila(){
    jukefila *fila = malloc (sizeof (jukefila));
    
    if (!fila)
        return NULL;
        
    fila->inicio = NULL;
    fila->final = NULL;
    
    return fila;
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
    if (elemento->valor > fila->inicio->valor){
        elemento->proximo = fila->inicio;
        fila->inicio->anterior = elemento;
        fila->inicio = elemento;
        return ;
    }
    
    //inserir no final
    if (elemento->valor <= fila->final->valor){
        elemento->anterior = fila->final;
        fila->final->proximo = elemento;
        fila->final = elemento;
        return ;
    }
    
    //inserir no meio
    pedido *aux = fila->inicio;
    while (aux->valor >= elemento->valor)
    {
        aux = aux->proximo;
    }

    elemento->anterior = aux->anterior;
    elemento->proximo = aux;
    aux->anterior = elemento;
    return ;
    
}

pedido* consumir_jukefila(jukefila* fila){
    if (!fila || !fila->inicio)
        return NULL;

    pedido *aux = fila->inicio;

    // fila com mais de um componente
    if (aux->proximo)
        fila->inicio = aux->proximo;

    return aux;

}

unsigned int contar_jukefila(jukefila* fila){
    if (!fila)
        return 0;
    
    
    unsigned int tam = 0;
    pedido *aux = fila->inicio;

    while (aux != NULL){
        tam += 1;
        aux = aux->proximo;
    }

    return tam;
}

void destruir_jukefila(jukefila *fila){
    if (!fila)
        return ;
    
    pedido *aux = fila->inicio;

    // fila com 1 pedido
    if (!aux->proximo){
        destruir_pedido (aux);
        free (fila);
        return ;
    }

    while (aux != fila->final){
        pedido *aux = fila->inicio;
        if (aux->proximo)
            fila->inicio = aux->proximo;
        destruir_pedido (aux);
        aux = NULL;
    }
    free (fila);
}