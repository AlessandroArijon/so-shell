/*Alumnos:
/ Miguel Alessandro Arijon Alvarez
/ Saul Raposo Raposo
*/

#include "listNodes.h"
#include "command2.h"

void iniListMemblocks(listMemblocks *l){
    *l = NULL;
}

int AnadirMemblock(MemType type, key_t key, size_t size, void *p, time_t time, listMemblocks *mem){
    if (mem == NULL) {
        fprintf(stderr, "Error: lista de bloques de memoria es NULL.\n");
        return -1;
    }
    MemNode *current = *mem;
    while (current != NULL) {
        if (current->address == p) {
            current->size = size;
            current->type = type;
            current->time = time;
            if(type == SHARED) {
                current->key = key;
            }
            return 0;
        }
        current = current->next;
    }
    MemNode * newNode = calloc(1, sizeof(MemNode));
    if (newNode == NULL){
        fprintf(stderr, "Error al asignar memoria para el nodo '%p': %s\n", p, strerror(errno));
        return -1;
    }
    newNode->address = p;
    newNode->size = size;
    newNode->type = type;
    newNode->time = time;
    if(type == SHARED) {
        newNode->key = key;
    }
    newNode->next = *mem;
    *mem = newNode;
    return 0;
}

int AnadirMemblockMmap(MemType type, char *filename, size_t size, void *p, time_t time, int df, listMemblocks *mem) {
    if (mem == NULL) {
        fprintf(stderr, "Error: lista de bloques de memoria es NULL.\n");
        return -1;
    }
    MemNode *current = *mem;
    while (current != NULL) {
        if (current->address == p) {
            current->size = size;
            current->type = type;
            current->time = time;
            if (current->filename != NULL) free(current->filename);
            current->filename = strdup(filename);
            current->df = df;
            if (current->filename == NULL) return -1;
            return 0;
        }
        current = current->next;
    }
    MemNode *newNode = calloc(1, sizeof(MemNode));
    if (newNode == NULL) {
        fprintf(stderr, "Error al asignar memoria para el nodo '%p': %s\n", p, strerror(errno));
        return -1;
    }
    newNode->size = size;
    newNode->type = type;
    newNode->time = time;
    newNode->filename = strdup(filename);
    if (newNode->filename == NULL) {
        fprintf(stderr, "Error al duplicar cadena para filename: %s\n", strerror(errno));
        free(newNode);
        return -1;
    }
    newNode->address = p;
    newNode->df = df;
    newNode->next = *mem;
    *mem = newNode;
    return 0;
}


void printType(MemNode current) {
    time_t t = current.time;
    struct tm *tm_info = localtime(&t);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%b %d %H:%M", tm_info);
    if(current.type == SHARED) {
        printf("\t\t%p\t%zu\t%s\tSHARED\t(Key: %d)\n", current.address, current.size, buffer, (int) current.key);
        return;
    }
    else if(current.type == MALLOC) {
        printf("\t\t%p\t%zu\t%s\tMALLOC\n", current.address, current.size, buffer);
        return;
    }
    else {
        printf("\t\t%p\t%zu\t%s\t%s\t(descriptor: %d)\n", current.address, current.size, buffer, current.filename, current.df);
        return;
    }
}


void imprimirListaTipo(listMemblocks l, MemType type) {
    pid_t pid = getpid();
    MemNode *current = l;
    printf("******Lista de bloques asignados para el proceso %d\n", pid);
    while (current != NULL) {
        if (current->type == type) {
            printType(*current);
        }
        current = current->next;
    }
}


void vaciarListaMemblocks(listMemblocks *l) {
    if (l == NULL || *l == NULL) return;

    MemNode *current = *l;
    MemNode *temp = NULL;

    while (current != NULL) {
        temp = current->next;

        if (current->type == MALLOC && current->address != NULL)
            free(current->address);
        else if (current->type == SHARED && current->address != NULL)
            shmdt(current->address);
        else if (current->type == MMAP && current->address != NULL)
            munmap(current->address, current->size);

        free(current->filename);
        free(current);
        current = temp;
    }
    *l = NULL;
}
