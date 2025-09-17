#ifndef UTIL_H
#define UTIL_H

#include <time.h>

// Converte time_t para string formatada
void format_date(time_t t, char *buffer, int max);

struct bloco {
    long num_arquivos;
    long *offset;
};

struct bloco *cria_bloco ();

#endif

