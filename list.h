/*Alumnos:
/ Miguel Alessandro Arijon Alvarez
/ Saul Raposo Raposo
*/

#ifndef P0SO_LIST_H
#define P0SO_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>


#define MAX_INPUT_SIZE 1024
#define MAX_COMMAND_LENGTH 512


typedef char *command;

typedef struct CommandNode *pos;

typedef struct CommandNode {
    command command;
    pos next;
}CommandNode;

typedef struct listHistorico {
    pos head;
    int count;
}listHistorico ;

typedef struct FileNode {
    int df;
    char filename[MAX_COMMAND_LENGTH];
    struct FileNode* next;
} FileNode;

typedef FileNode *listaFicheros;

void initHistorico(listHistorico *history);

void addCommand(listHistorico *history, const char *input);

void printHistory(listHistorico history);

void initFicheros(listaFicheros *ficheros);

int AnadirAFicherosAbiertos(int df, const char* filename, listaFicheros *ficheros);

int abrirArchivo(const char* filename, int mode, listaFicheros *ficheros);

int cerrarArchivo(int df, listaFicheros *ficheros);

void closeAllFiles(listaFicheros *ficheros);

void ListarFicherosAbiertos(listaFicheros ficheros);

void EliminarFicherosAbiertos(listaFicheros* ficheros, int df);

void freeCommandHistory(listHistorico *hist);

void freeFicheros(listaFicheros *ficheros);


#endif //P0SO_LIST_H
