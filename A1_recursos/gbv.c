#include <stdio.h>
#include <stdlib.h>
#include "gbv.h"


int gbv_create(const char *filename){
    if (!filename) return 0;


}
int gbv_open(Library *lib, const char *filename){
    if (!lib || !filename) return 1;
    FILE *file = fopen ("filename", "rb");
    file.
    lib->docs
}
int gbv_add(Library *lib, const char *archive, const char *docname);
int gbv_remove(Library *lib, const char *docname);

int gbv_list(const Library *lib){
    if (!lib) return 0;
}

int gbv_view(const Library *lib, const char *docname);