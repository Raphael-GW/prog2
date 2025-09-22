#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

    Document *doc = malloc (sizeof (Document));
    if (!doc){
        printf ("Erro ao criar docs\n");
        return 1;
    }

    fwrite (b)

    free (b);
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

    struct bloco *b = cria_bloco ();
    if (fread(b, sizeof(struct bloco), 1, file) != 1){
        fclose(file);
        printf("Bloco não achado\n");
        return 1;
    }

    lib->count = b->num_arquivos;
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

    free (b);
    fclose(file);
    return 0;
}

int gbv_add(Library *lib, const char *archive, const char *docname){
    if (!lib || !archive || !docname) return 1;

    FILE* file = fopen(archive, "r+b");
    FILE* arq  = fopen(docname, "rb");
    if (!file || !arq){
        perror("Erro ao abrir o arquivo (add)");
        return 1;
    }

    // le o bloco
    struct bloco *b = cria_bloco ();
    if (fread(b, sizeof(struct bloco), 1, file) != 1){
        fclose(file);
        printf("Bloco não achado\n");
        return 1;
    }

    // pega as informações do arquivo
    struct stat info;
    if (stat(docname, &info) != 0) {
        perror("stat");
        fclose(file);
        fclose(arq);
        return 1;
    }

    // determina o offset de escrita
    long off;
    if (lib->count == 0){
        off = sizeof (struct bloco);
    }
    else{
        off = b->offset;
        for (int i = 0; i < lib->count; i++){
            if (strcmp (lib->docs[i].name, docname) == 0){
                off = lib->docs[i].offset;
                b->num_arquivos--;
            }
        }
    }
    fseek (file, off, SEEK_SET);

    // copia em blocos do arquivo fonte para o container
    char buffer[BUFFER_SIZE];
    size_t n;
    while ((n = fread(buffer, 1, BUFFER_SIZE, arq)) > 0) {
        if (fwrite(buffer, 1, n, file) != n) {
            printf ("Erro na escrita");
            fclose(file);
            fclose(arq);
            return 1;
        }
    }
    if (ferror(arq)) {
        printf ("Erro ao ler arquivo de origem");
        fclose(file);
        fclose(arq);
        return 1;
    }

    

    /* atualizar metadados em memória (lib) */
    Document *new_docs = realloc(lib->docs, sizeof(Document) * (lib->count + 1));
    if (!new_docs) {
        printf ("realloc");
        fclose(file);
        fclose(arq);
        return 1;
    }
    lib->docs = new_docs;
    
    Document *doc = &lib->docs[lib->count];
    strncpy(doc->name, docname, MAX_NAME - 1);
    doc->name[MAX_NAME - 1] = '\0';

    doc->size   = (long) info.st_size;
    doc->date   = info.st_mtime;
    doc->offset = off;
    fwrite (doc, sizeof (Document), 1, file);

    rewind (file);
    b->num_arquivos++;
    b->offset += doc->size;
    fwrite (&b, sizeof (struct bloco), 1, file);
    

    
    free (b);
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