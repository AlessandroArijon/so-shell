/*Alumnos:
/ Miguel Alessandro Arijon Alvarez
/ Saul Raposo Raposo
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "list.h"

void initHistorico(listHistorico *history){
    history->head = NULL;
    history->count = 0;
}

void addCommand(listHistorico *hist, const char *input){
    CommandNode *newNode = malloc(sizeof(CommandNode));
    if (newNode == NULL) {
        perror("Error al asignar memoria para el comando");
        return;
    }
    newNode->command = strdup(input);
    if (newNode->command == NULL){  
        perror("Error al duplicar comando");
        free(newNode);
        return;
    }
    newNode->next = NULL;
    if (hist->head == NULL) {
        hist->head = newNode;
    } else {
        CommandNode *last = hist->head;
        while (last->next != NULL) last = last->next;
        last->next = newNode;
    }
    hist->count++;
}


void printHistory(listHistorico history){
    pos current = history.head;
    int command_number = 1;
    while (current != NULL){
        printf("%d: %s\n", command_number++, current->command);
        current = current->next;
    }
}

void initFicheros(listaFicheros *ficheros){
    *ficheros = NULL;
    AnadirAFicherosAbiertos(0, "stdin", ficheros);
    AnadirAFicherosAbiertos(1, "stdout", ficheros);
    AnadirAFicherosAbiertos(2, "stderr", ficheros);
}

int AnadirAFicherosAbiertos(int df, const char* filename, listaFicheros *ficheros){
    if (ficheros == NULL) {
        fprintf(stderr, "Error: lista de ficheros es NULL.\n");
        return -1;
    }

    FileNode* current = *ficheros;

    while (current != NULL){
        if (current->df == df) {
            strncpy(current->filename, filename, MAX_COMMAND_LENGTH);
            current->filename[MAX_COMMAND_LENGTH - 1] = '\0';
            printf("Descriptor %d ya existe, actualizado a %s.\n", df, filename);
            return 0;
        }
        current = current->next;
    }

    FileNode* newNode = (FileNode*) malloc(sizeof(FileNode));
    if (newNode == NULL){
        perror("Error al asignar memoria para el nodo");
        return -1;
    }

    newNode->df = df;
    strncpy(newNode->filename, filename, MAX_COMMAND_LENGTH);
    newNode->filename[MAX_COMMAND_LENGTH - 1] = '\0';
    newNode->next = *ficheros;
    *ficheros = newNode;

    if (df != 0 && df != 1 && df != 2){
        printf("Archivo %s (fd: %d) añadido a la lista de ficheros abiertos.\n", filename, df);
    }
    return 0;
}

int abrirArchivo(const char* filename, int mode, listaFicheros *ficheros){
    int df = open(filename, mode, 0666);
    if (df == -1){
        fprintf(stderr, "Error abriendo el archivo '%s': %s\n", filename, strerror(errno));
        return -1;
    }

    printf("Archivo '%s' abierto con descriptor %d.\n", filename, df);
    if (AnadirAFicherosAbiertos(df, filename, ficheros) == -1) {
        close(df);
        return -1;
    }
    return 0;
}


int cerrarArchivo(int df, listaFicheros *ficheros){
    if (close(df) == -1){
        fprintf(stderr, "Error cerrando el descriptor de archivo %d: %s\n", df, strerror(errno));
        return -1;
    }

    EliminarFicherosAbiertos(ficheros, df);
    printf("Archivo con descriptor %d cerrado y eliminado de la lista.\n", df);
    return 0;
}


void closeAllFiles(listaFicheros *ficheros){
    if (ficheros == NULL){
        fprintf(stderr, "Error: lista de archivos es NULL.\n");
        return;
    }

    FileNode *actual = *ficheros;
    while (actual != NULL){
        if (actual->df > STDERR_FILENO) {
            if (close(actual->df) == -1) perror("Error al cerrar el archivo");
            else printf("Cerrando archivo: %s (fd: %d)\n", actual->filename, actual->df);
        }
        
        FileNode *temp = actual; 
        actual = actual->next;
        free(temp);
    }
    *ficheros = NULL; 
}



void ListarFicherosAbiertos(listaFicheros ficheros) {
    enum { MAX_DESCRIPTORS = 20 };
    FileNode* current = ficheros;
    char* descriptor_names[MAX_DESCRIPTORS];
    off_t descriptor_offsets[MAX_DESCRIPTORS];
    memset(descriptor_names, 0, sizeof(descriptor_names));
    memset(descriptor_offsets, 0, sizeof(descriptor_offsets));

    while (current != NULL) {
        if (current->df >= 0 && current->df < MAX_DESCRIPTORS) {
            descriptor_names[current->df] = (current->df == 0) ? "entrada estándar" :
                                            (current->df == 1) ? "salida estándar" :
                                            (current->df == 2) ? "error estándar" : current->filename;
            descriptor_offsets[current->df] = lseek(current->df, 0, SEEK_CUR);
        }
        current = current->next;
    }

    for (int i = 0; i < MAX_DESCRIPTORS; i++) {
        if (descriptor_names[i] != NULL) {
            int flags = fcntl(i, F_GETFL);
            const char *mode = "desconocido";
            if (flags != -1) {
                switch (flags & O_ACCMODE) {
                    case O_RDONLY: mode = "O_RDONLY"; break;
                    case O_WRONLY: mode = "O_WRONLY"; break;
                    case O_RDWR: mode = "O_RDWR"; break;
                }
            }
            printf("descriptor: %d, offset: (%lld) -> %s %s\n",
                   i, (long long)descriptor_offsets[i], descriptor_names[i], mode);
        } else {
            printf("descriptor: %d, offset: (  ) -> no usado\n", i);
        }
    }
}



void EliminarFicherosAbiertos(listaFicheros* ficheros, int df){
    FileNode* current = *ficheros;
    FileNode* previous = NULL;

    while (current != NULL){
        if (current->df == df){
            if (previous == NULL) *ficheros = current->next;  
            else previous->next = current->next; 
            printf("Eliminando archivo %s (fd: %d) de la lista de ficheros abiertos.\n", current->filename, df);
            free(current);
            current = NULL;
            return;
        }
        previous = current;
        current = current->next;
    }
    printf("No se encontró el descriptor %d en la lista de ficheros abiertos.\n", df);
}



void freeCommandHistory(listHistorico *hist){
    CommandNode *current = hist->head;
    while (current != NULL){
        CommandNode *to_free = current;
        current = current->next;
        free(to_free->command);
        free(to_free);
    }
    hist->head = NULL;
    hist->count = 0;
}

void freeFicheros(listaFicheros *ficheros){
    FileNode *current = *ficheros;
    while (current) {
        FileNode *to_free = current;
        current = current->next;
        free(to_free);
    }
    *ficheros = NULL;
}
