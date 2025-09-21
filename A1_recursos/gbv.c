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
        fprintf(stderr, "filename NULL\n");
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
            fprintf(stderr, "Erro ao criar lib\n");
            return 1;
        }
    }

    struct bloco b;
    if (fread(&b, sizeof(struct bloco), 1, file) != 1){
        fclose(file);
        fprintf(stderr, "Bloco não achado\n");
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
            /* leitura parcial/erro */
            free(lib->docs);
            lib->docs = NULL;
            lib->count = 0;
            fclose(file);
            fprintf(stderr, "Erro ao ler diretório de documentos\n");
            return 1;
        }
    } else {
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

    struct stat info;
    if (stat(docname, &info) != 0) {
        perror("stat");
        fclose(file);
        fclose(arq);
        return 1;
    }

    /* posiciona no fim do container e pega offset inicial onde os bytes serão gravados */
    if (fseek(file, 0, SEEK_END) != 0) {
        perror("fseek container");
        fclose(file);
        fclose(arq);
        return 1;
    }
    long offset = ftell(file);
    if (offset == -1L) {
        perror("ftell container");
        fclose(file);
        fclose(arq);
        return 1;
    }

    /* copia em blocos do arquivo fonte para o container */
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
    /* copiar só o nome (não o caminho completo se preferir) */
    strncpy(doc->name, docname, MAX_NAME - 1);
    doc->name[MAX_NAME - 1] = '\0';

    doc->size   = (long) info.st_size;
    doc->date   = info.st_mtime;   /* ou st_atime/st_ctime conforme necessidade */
    doc->offset = offset;

    lib->count++;

    /* NOTA: aqui atualizamos apenas a estrutura em memória.
       Se você precisa persistir o header/diretório no arquivo .gbv,
       faça fseek(file, 0, SEEK_SET) e reescreva o bloco/header e a tabela de Document. */

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