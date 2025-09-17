#include <stdio.h>
#include <stdlib.h>
#include "gbv.h"
#include "util.h"

// função chamada se arquivo não existe
int gbv_create(const char *filename){
    if (!filename) return 1;
    
    FILE *file = fopen (filename, "w+b");
    if (!file){
        perror ("Erro ao abrir arquivo\n");
        exit (1);
    }

    struct bloco *b = cria_bloco ();
    if (!b) return 1;

    fwrite (b, sizeof (struct bloco), 1, file);

}

int gbv_open(Library *lib, const char *filename){
    if (!filename) return 1;

    FILE *file = fopen (filename, "r+b");

    if (!file){
        gbv_create (filename);
        file = fopen (filename, "r+b");
    }
    
    gbv_create (lib);
}
int gbv_add(Library *lib, const char *archive, const char *docname);
int gbv_remove(Library *lib, const char *docname);

int gbv_list(const Library *lib){
    if (!lib) return 0;
}

int gbv_view(const Library *lib, const char *docname);