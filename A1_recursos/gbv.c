#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "util.h"
#include "gbv.h"

// função chamada se arquivo não existe
int gbv_create(const char *filename){
    if (!filename) return 1;
    
    FILE *file = fopen (filename, "w+b");
    if (!file) return 1;

    struct bloco *b = cria_bloco ();
    if (!b){
        printf ("Erro ao criar bloco\n");
        return 1;
    }

    fwrite (b, sizeof (struct bloco), 1, file);

    fclose (file);
    return 0;
}

int gbv_open(Library *lib, const char *filename){
    if (!filename || !lib){
        printf ("filename NULL\n");
        return 1;
    }

    FILE *file = fopen (filename, "r+b");

    if (!file){
        if (gbv_create (filename) == 0){
            file = fopen (filename, "r+b");
        }
        else{
            printf ("Erro ao criar lib\n");
            return 1;
        }
    }

    struct bloco *b;
    if (fread (b, sizeof (struct bloco), 1, file) != 1){
        fclose (file);
        printf ("Bloco não achado\n");
        return 1;
    }

    lib->count = b->num_arquivos;
    if (lib->count > 0){
        lib->docs = malloc (sizeof (Document) * lib->count);
        if (!lib->docs){
            fclose (file);
            return 1;
        }
        fread (lib->docs, sizeof (Document), lib->count, file);
    }
    else lib->docs = NULL;

    fclose (file);
    return 0;
}

int gbv_add(Library *lib, const char *archive, const char *docname){
    if (!lib || !archive || !docname) return 1;

    FILE* file = fopen (archive, "r+b");
    FILE* arq = fopen (docname, "r+b");
    if (!file || !arq){
        perror ("Erro ao abrir o arquivo (add)\n");
        return 1;
    }
    
    struct stat info;
    stat (docname, &info);
    char buffer[BUFFER_SIZE];
    int n;
    if (lib->count < 1){
        while ((n = fread(buffer, 1, BUFFER_SIZE, arq)) > 0) {
        if (fwrite(buffer, 1, n, file) != n) {
            perror("Erro na escrita");
            fclose(file);
            fclose(arq);
            return 1;
        }
        }
        strncpy(lib->docs[0].name, docname, MAX_NAME - 1);
        lib->docs[0].name[MAX_NAME - 1] = '\0';
        lib->docs[0].date = info.st_atime;
        lib->docs[0].offset = ftell (arq);
        lib->docs[0].size = info.st_size;
    }
    
}

int gbv_remove(Library *lib, const char *docname){
    if (!lib || !docname) return 1;
}

int gbv_list(const Library *lib){
    if (!lib) return 0;

}

int gbv_view(const Library *lib, const char *docname){
    if (!lib || !docname) return 1;

}