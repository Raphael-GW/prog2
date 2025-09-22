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
        printf("filename NULL\n");
        return 1;
    }

    FILE *file = fopen(filename, "r+b");
    if (!file){
        if (gbv_create(filename) == 0){
            file = fopen(filename, "r+b");
            if (!file) {
                perror("fopen after create");
                return 1;
            }
        } else {
            printf("Erro ao criar lib\n");
            return 1;
        }
    }

    struct bloco b;
    if (fread(&b, sizeof(struct bloco), 1, file) != 1){
        fclose(file);
        printf("Bloco não achado\n");
        return 1;
    }

    lib->count = b.num_arquivos;
    if (lib->count > 0){
        lib->docs = malloc(sizeof(Document) * lib->count);
        if (!lib->docs){
            fclose(file);
            perror("malloc lib->docs");
            return 1;
        }
        if (fread(lib->docs, sizeof(Document), lib->count, file) != (size_t)lib->count) {
            free(lib->docs);
            lib->docs = NULL;
            lib->count = 0;
            fclose(file);
            fprintf(stderr, "Erro ao ler diretório de documentos\n");
            return 1;
        }
    } 
    else {
        lib->docs = NULL;
    }

    fclose(file);
    return 0;
}

int gbv_add(Library *lib, const char *archive, const char *docname){
    if (!lib || !archive || !docname) return 1;

    FILE* file = fopen(archive, "r+b");
    FILE* arq  = fopen(docname, "rb");
    if (!file || !arq){
        perror("Erro ao abrir o arquivo (add)");
        if (file) fclose(file);
        if (arq) fclose(arq);
        return 1;
    }

    struct bloco b;
    if (fread(&b, sizeof(struct bloco), 1, file) != 1){
        fclose(file);
        printf("Bloco não achado\n");
        return 1;
    }

    struct stat info;
    if (stat(docname, &info) != 0) {
        perror("stat");
        fclose(file);
        fclose(arq);
        return 1;
    }

    long off;
    if (lib->count == 0){
        off = sizeof (struct bloco);
    }
    else{
        off = b.offset[lib->count - 1];
    }
    
    fseek (file, off, SEEK_SET);
    // copia em blocos do arquivo fonte para o container
    char buffer[BUFFER_SIZE];
    size_t n;
    while ((n = fread(buffer, 1, BUFFER_SIZE, arq)) > 0) {
        if (fwrite(buffer, 1, n, file) != n) {
            perror("Erro na escrita");
            fclose(file);
            fclose(arq);
            return 1;
        }
    }
    if (ferror(arq)) {
        perror("Erro ao ler arquivo de origem");
        fclose(file);
        fclose(arq);
        return 1;
    }

    

    /* atualizar metadados em memória (lib) */
    Document *new_docs = realloc(lib->docs, sizeof(Document) * (lib->count + 1));
    if (!new_docs) {
        perror("realloc");
        fclose(file);
        fclose(arq);
        return 1;
    }
    lib->docs = new_docs;
    
    Document *doc = &lib->docs[lib->count];
    strncpy(doc->name, docname, MAX_NAME - 1);
    doc->name[MAX_NAME - 1] = '\0';

    doc->size   = (long) info.st_size;
    doc->date   = info.st_mtime;   /* ou st_atime/st_ctime conforme necessidade */
    doc->offset = off;


    lib->count++;

    long *new_off = realloc (b.offset, sizeof (long) * lib->count);
    if (!new_off){
        printf ("Erro realloc new_off\n");
        return 1;
    }
    b.offset = new_off;
    b.offset[lib->count] = doc->offset + doc->size;

    fclose(file);
    fclose(arq);
    return 0;
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