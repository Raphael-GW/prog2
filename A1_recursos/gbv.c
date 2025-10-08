#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "util.h"
#include "gbv.h"

const char *gbv;

struct bloco {
    long num_arquivos;
    long offset;
};

struct bloco *cria_bloco (){
    struct bloco *b = malloc (sizeof (struct bloco));
    if (!b) return NULL;

    b->num_arquivos = 0;
    b->offset = 0;

    return b;
}

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

    // coloca o offset logo depois do bloco
    b->offset = sizeof (struct bloco);

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

    gbv = filename; // adiciona a biblioteca na variavel global

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

    // verifica se existe mais de um arquivo e cria o diretorio
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

    // Verifica se já existe no diretório
    int indice = -1;
    for (int i = 0; i < lib->count; i++) {
        if (strcmp(lib->docs[i].name, docname) == 0) {
            indice = i;
            break;
        }
    }

    if (indice != -1){
        gbv_remove (lib, docname);
    }

    FILE* file = fopen(archive, "r+b");
    FILE* arq  = fopen(docname, "rb");
    if (!file || !arq){
        if (file) fclose (file);
        if (arq) fclose (arq);
        printf ("Erro ao abrir o arquivo (add)\n");
        return 1;
    }

    // lê superbloco
    struct bloco *b = cria_bloco ();
    if (fread(b, sizeof(struct bloco), 1, file) != 1){
        fclose (file);
        printf ("Bloco não achado\n");
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

    // Onde será gravado o conteúdo do arquivo
    long off = b->offset;
    fseek (file, off, SEEK_SET);

    long start_write = ftell (file);
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
    fflush (file);

    if (ferror(arq)) {
        printf ("Erro ao ler arquivo de origem");
        fclose(file);
        fclose(arq);
        free (b);
        return 1;
    }
    

    // atualizar metadados em memória (lib)
    Document *new_docs = realloc(lib->docs, sizeof(Document) * (lib->count + 1));
    if (!new_docs) {
        printf (" Erro realloc docs\n");
        fclose(file);
        fclose(arq);
        free (b);
        return 1;
    }
    indice = lib->count;
    lib->docs = new_docs;
    
    Document *doc = &lib->docs [indice];
    strncpy(doc->name, docname, MAX_NAME - 1);
    doc->name[MAX_NAME - 1] = '\0';
    doc->size   = (long) info.st_size;
    doc->date   = info.st_mtime;
    doc->offset = start_write;
    
    lib->count++;
    b->num_arquivos = lib->count;
    b->offset = ftell (file);
    
    // escreve o bloco e o diretorio na memoria
    fseek (file, b->offset, SEEK_SET);
    if (fwrite (lib->docs, sizeof (Document), lib->count, file) != lib->count){
        printf ("Erro ao escrever diretório\n");
        fclose (file);
        fclose (arq);
        free (b);
        return 1;
    }

    rewind (file);
    if (fwrite(b, sizeof(struct bloco), 1, file) != 1) {
        printf("Erro ao atualizar superbloco\n");
        fclose (file);
        fclose (arq);
        free (b);
        return 1;
    }

    // fecha os arquivos
    fclose (arq);
    free (b);
    fclose(file);
    return 0;
}

int gbv_remove(Library *lib, const char *docname){
    if (!lib || !docname) return 1;

    int indice = -1;
    for (int i = 0; i < lib->count; i++){
        if (strcmp (docname, lib->docs[i].name) == 0){
            indice = i;
            break;
        }
    }

    if (indice == -1){
        printf ("Arquivo não achado (remove)\n");
        return 1;
    }

    FILE *file = fopen (gbv, "r+b");
    if (!file){
        printf ("Erro ao abrir arquivo\n");
        return 1;
    }

    struct bloco *b = cria_bloco ();
    if (fread (b, sizeof (struct bloco), 1, file) != 1){
        printf ("Erro ao ler superbloco\n");
        fclose (file);
        free (b);
        return 1;
    }

    // calcula o tamanho do shift
    long tam_total_shift = 0;
    for (int i = indice + 1; i < lib->count; i++){
        tam_total_shift += lib->docs[i].size;
    }

    size_t n;
    char buffer[BUFFER_SIZE];
    long off_escrita = lib->docs[indice].offset;

    // Se houver dados para mover
    if (tam_total_shift > 0) {
        long off_leitura = lib->docs[indice + 1].offset;
        
        fseek(file, off_leitura, SEEK_SET);

        // O loop lê e escreve, movendo os dados
        while (off_leitura < b->offset) {
            fseek(file, off_leitura, SEEK_SET);
            n = fread(buffer, 1, BUFFER_SIZE, file);

            if (n > 0) {
                fseek(file, off_escrita, SEEK_SET);
                if (fwrite(buffer, 1, n, file) != n) {
                    printf("Erro na escrita durante o shift.\n");
                    fclose(file);
                    free (b);
                    return 1;
                }
                off_leitura += n;
                off_escrita += n;
            } else {
                break; // Fim dos dados
            }
        }
    }

    // atualiza o diretório
    long tam_shift = lib->docs[indice].size;
    for (int i = indice; i < lib->count - 1; i++){
        lib->docs[i] = lib->docs[i + 1];
        lib->docs[i].offset -= tam_shift;
    }

    // atualiza os metadados do diretorio
    if (lib->count > 1){
        Document *newdocs = realloc (lib->docs, sizeof (Document) * (lib->count - 1));
        if (!newdocs){
            printf ("Erro realloc\n");
            fclose (file);
            free (b);
            return 1;
        }
        lib->docs = newdocs;
        lib->count--;

        b->num_arquivos = lib->count;
        b->offset = lib->docs[lib->count - 1].offset + lib->docs[lib->count - 1].size;

        // escreve o bloco e o diretorio atualizado na memoria
        rewind (file);
        n = fwrite (b, sizeof (struct bloco), 1, file);
        if (n != 1){
            fclose (file);
            printf ("Erro write bloco\n");
            free (b);
            return 1;
        }

        fseek (file, b->offset, SEEK_SET);
        n = fwrite (lib->docs, sizeof (Document), lib->count, file);
        if (n != lib->count){
            printf ("Erro write diretorio\n");
            fclose (file);
            free (b);
            return 1;
        }
    }
    else{
        lib->count--;

        b->num_arquivos = lib->count;
        b->offset = sizeof (struct bloco);

        rewind (file);
        n = fwrite (b, sizeof (struct bloco), 1, file);
        if (n != 1){
            printf ("Erro escrita bloco (1)");
            fclose (file);
            free (b);
            return 1;
        }
    }


    // trunca o arquivo
    long new_file_size = b->offset + (lib->count * sizeof(Document));
    if (ftruncate(fileno(file), new_file_size) != 0) {
        printf ("Erro ao truncar o arquivo (ftruncate)");
    }

    fclose (file);
    free (b);
    return 1;
}

int gbv_list(const Library *lib){
    if (!lib) return 0;

    char data[MAX_NAME];
    for (int i = 0; i < lib->count; i++){
        printf ("Nome: %s\n", lib->docs[i].name);
        printf ("Tamanho: %ld\n", lib->docs[i].size);

        format_date (lib->docs[i].date, data, MAX_NAME);
        printf ("Data: %s\n", data);
        printf ("Offset: %ld\n", lib->docs[i].offset);
        printf ("---//---\n");
    }

    return 1;
}

int gbv_view(const Library *lib, const char *docname){
    if (!lib || !docname) return 1;

    FILE *file = fopen (gbv, "rb");
    if (!file){
        printf ("Erro ao abrir lib\n");
        return 1;
    }
    
    int indice = -1;
    for (int i = 0; i < lib->count; i++){
        if (strcmp (lib->docs[i].name, docname) == 0){
            indice = i;
            break;
        }
    }

    if (indice == -1){
        printf ("Arquivo não achado\n");
        fclose (file);
        return 1;
    }

    long off = lib->docs[indice].offset; // inicio do arquivo
    long limite = off + lib->docs[indice].size; // final do arquivo
    long pos = off;
    long max_ler;

    char buffer[BUFFER_SIZE + 1];
    char opcao = 'n';

    while (1) {
        // determina quanto do arquivo pode ser lido
        if ((limite - pos) < BUFFER_SIZE) max_ler = limite - pos;
        else max_ler = BUFFER_SIZE;

        fseek(file, pos, SEEK_SET);
        size_t lidos = fread(buffer, 1, max_ler, file);
        
        buffer[lidos] = '\0';
        
        for (int i = 0; i < lidos; i++){
            printf ("%02x", buffer[i]);
        }

        printf("\n[n] próximo | [p] anterior | [q] sair > ");
        fflush(stdout);

        // pega a proxima instrucao
        do {
            opcao = getchar();
        } while (opcao == '\n');

        if (opcao == 'q') break;

        if (opcao == 'n') {
            if (pos + BUFFER_SIZE < limite)
                pos += BUFFER_SIZE;
            else
                printf("\nJá está no último bloco.\n");
        } else if (opcao == 'p') {
            if (pos - BUFFER_SIZE >= off)
                pos -= BUFFER_SIZE;
            else
                printf("\nJá está no primeiro bloco.\n");
        } else {
            printf("\nOpção inválida.\n");
        }
    }

    fclose(file);
    return 0;
}