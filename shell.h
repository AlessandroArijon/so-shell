/*Alumnos:
/ Miguel Alessandro Arijon Alvarez
/ Saul Raposo Raposo
*/

#ifndef P0SO_SHELL_H
#define P0SO_SHELL_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>
#include <sys/utsname.h>
#include <stdint.h>

#include "list.h"
#include "listNodes.h"
#include "dir_proc_list.h"
#include "command.h"
#include "command2.h"
#include "open.h"
#include "command3.h"


#define MAX_TROZOS 512

void imprimirPrompt(void);

char* leerEntrada(void);

int TrocearCadena(char *input, char *trozos[]);

void printLastNCommands(listHistorico list, int N);

char* getCommandByNumber(listHistorico list, int N);

void printDateAndTime(int showDate, int showTime);

void printSystemInfo(void);

void printAuthors(int printNames, int printLogins);

void procesarDateCommand(int numPalabras, char *trozos[]);

void printHelp(const char *cmd);

void procesarHelpCommand(int numPalabras, char *trozos[]);

void auxList(int numPalabras, char **trozos, int opcion);

void limpiarRecursos(listHistorico *hist, listaFicheros *ficheros, listMemblocks *mem);
void freeEnvironmentCopy(char *envp[]);

void procesarEntrada(listHistorico *hist, char *input, listaFicheros *ficheros, listMemblocks *mem, SearchList *searchList, BackgroundProcessList *bgList, char *envp[]);

#endif
