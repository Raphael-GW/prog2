#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "util.h"
#include "gbv.h"


int gbv_create(const char *filename){
    if (!filename) return 0;


}
int gbv_open(Library *lib, const char *filename){
    if (!lib || !filename) return 1;

    FILE *file = fopen (filename, "rb");
    if (!file){
        perror ("Erro ao abrir o arquivo");
        exit (1);
    }
    char buffer[BUFFER_SIZE];

    fread (buffer, sizeof (char), BUFFER_SIZE, filename);
    
    
}
int gbv_add(Library *lib, const char *archive, const char *docname);
int gbv_remove(Library *lib, const char *docname);

int gbv_list(const Library *lib){
    if (!lib) return 0;

    int offset = 0;

    for (int i = 0; i < lib->count; i++){
        struct stat *arq_stat;
        stat (lib, arq_stat);
        arq_stat->st_size
        
    }
}

int gbv_view(const Library *lib, const char *docname);