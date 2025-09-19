#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "util.h"
#include "gbv.h"

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

    fclose (file);
    return 0;
}

int gbv_open(Library *lib, const char *filename){
    if (!filename || !lib) return 1;

    FILE *file = fopen (filename, "r+b");

    if (!file){
        if (gbv_create (filename) == 0){
            file = fopen (filename, "r+b");
        }
        else{
            perror ("Erro ao abrir o arquivo");
            exit (1);
        }
    }

    struct bloco *b = NULL;
    if (fread (b, sizeof (struct bloco), 1, file) != 1){
        fclose (file);
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

    FILE* f = fopen (archive, "r+b");
    FILE* arq = fopen (docname, "w+b");
    if (!f || !arq){
        perror ("Erro ao abrir o arquivo (add)\n");
        exit (1);
    }
    
    struct stat *info = NULL;
    fset
    if (lib->count < 1){
        fwrite (arq, sizeof (FILE), 1, f);
        stat (docname, info);
        Document *doc = lib->docs;
        doc->date = docname;

    }
}

int gbv_remove(Library *lib, const char *docname){
    if (!lib || !docname) return 1;
}

int gbv_list(const Library *lib){
    if (!lib) return 0;

    int offset = 0;

    for (int i = 0; i < lib->count; i++){
        struct stat *arq_stat;
        stat (lib, arq_stat);
        arq_stat->st_size;
        
    }
}

int gbv_view(const Library *lib, const char *docname){
    if (!lib || !docname) return 1;

}