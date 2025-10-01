#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "util.h"
#include "gbv.h"

const char *gbv;

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
    
    if (fwrite (b, sizeof (struct bloco), 1, file) != 1){
        printf ("Erro ao escrever superbloco\n");
        return 1;
    }

    free (b);
    fclose (file);
    return 0;
}

int gbv_open(Library *lib, const char *filename){
    if (!filename || !lib){
        printf("filename NULL\n");
        return 1;
    }
    gbv = filename;
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
        fseek (file, b->offset, SEEK_SET);
        if (fread(lib->docs, sizeof(Document), lib->count, file) != (size_t)lib->count) {
            free(lib->docs);
            lib->docs = NULL;
            lib->count = 0;
            fclose(file);
            printf("Erro ao ler diretório de documentos\n");
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

    // lê superbloco
    struct bloco *b = cria_bloco ();
    if (fread(b, sizeof(struct bloco), 1, file) != 1){
        fclose(file);
        printf("Bloco não achado\n");
        return 1;
    }

    // pega as informações do arquivo
    struct stat info;
    if (stat(docname, &info) != 0) {
        printf ("Erro stat\n");
        fclose(file);
        fclose(arq);
        return 1;
    }

    // Verifica se já existe no diretório
    int arq_existe = 0, indice = -1;
    for (int i = 0; i < lib->count; i++) {
        if (strcmp(lib->docs[i].name, docname) == 0) {
            arq_existe = 1;
            indice = i;
            break;
        }
    }

    // Onde será gravado o conteúdo do arquivo
    long off;
    if (arq_existe) {
        off = lib->docs[indice].offset;
    } else {
        off = b->offset;  
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

    fclose(arq);
    Document *new_docs;
    // atualizar metadados em memória (lib)
    if (!arq_existe){
        new_docs = realloc(lib->docs, sizeof(Document) * (lib->count + 1));
        if (!new_docs) {
            printf (" Erro realloc docs\n");
            fclose(file);
            fclose(arq);
            return 1;
        }
        indice = lib->count;
    }
    
    Document *doc = &lib->docs[indice];
    strncpy(doc->name, docname, MAX_NAME - 1);
    doc->name[MAX_NAME - 1] = '\0';
    doc->size   = (long) info.st_size;
    doc->date   = info.st_mtime;
    doc->offset = off;
    
    b->num_arquivos++;
    b->offset += doc->size;
    
    rewind (file);
    fwrite (b, sizeof(struct bloco), 1, file);

    // Vai para o final do arquivo e regrava diretório inteiro
    fseek(file, b->offset, SEEK_SET);
    fwrite(new_docs, sizeof(Document), b->num_arquivos, file);

    free (b);
    fclose(file);
    return 0;
}

int gbv_remove(Library *lib, const char *docname){
    if (!lib || !docname) return 1;

    FILE *file = fopen (gbv, "r+b");
    if (!file){
        perror("Erro ao abrir o arquivo (add)");
        return 1;
    }

    struct bloco *b = cria_bloco ();
    if (fread(b, sizeof(struct bloco), 1, file) != 1){
        fclose(file);
        printf("Bloco não achado\n");
        return 1;
    }
    
    int indice = 0, achou = 0;
    for (int i = 0; i < lib->count; i++){
        if (strcmp (docname, lib->docs[i].name) == 0){
            indice = i;
            achou = 1;
            break;
        }
    }

    if (!achou){
        fclose (file);
        free (b);
        return 1;
    }

    // move o metadado do arquivo para o final do diretório
    for (int i = indice; i < lib->count - 1; i++){
        Document aux = lib->docs[i];
        lib->docs[i] = lib->docs[i + 1];
        lib->docs[i + 1] = aux;
    }

    Document *new_docs = realloc(lib->docs, sizeof(Document) * (lib->count - 1));
    if (!new_docs) {
        printf (" Erro realloc docs\n");
        fclose(file);
        fclose(arq);
        return 1;
    }

    rewind (file);
    b->num_arquivos--;
    fwrite (b, sizeof (struct bloco), 1, file);

    fseek (file, b->offset, SEEK_SET);
    fwrite (new_docs, sizeof(Document) * (lib->count - 1), 1, file);
}

int gbv_list(const Library *lib){
    if (!lib) return 0;

    char data[MAX_NAME];
    printf ("%d\n", lib->count);
    for (int i = 0; i < lib->count; i++){
        printf ("Nome: %s\n", lib->docs[i].name);
        printf ("Tamanho: %ld\n", lib->docs[i].size);

        format_date (lib->docs[i].date, data, MAX_NAME);
        printf ("Data: %s\n", data);
        printf ("Offset: %ld\n", lib->docs[i].offset);
    }

    return 1;
}

int gbv_view(const Library *lib, const char *docname){
    if (!lib || !docname) return 1;

}