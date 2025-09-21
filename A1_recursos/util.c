#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void format_date(time_t t, char *buffer, int max) {
    struct tm *info = localtime(&t);
    strftime(buffer, max, "%d/%m/%Y %H:%M:%S", info);
}

struct bloco *cria_bloco (){
    struct bloco *b = malloc (sizeof (struct bloco));
    if (!b) return NULL;

    b->num_arquivos = 0;
    b->offset = malloc (sizeof (long));
    if (!b->offset) return NULL;

    return b;
}