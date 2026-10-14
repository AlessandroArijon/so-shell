/*Alumnos:
/ Miguel Alessandro Arijon Alvarez
/ Saul Raposo Raposo
*/

#ifndef P0SO_OPEN_H
#define P0SO_OPEN_H

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>

#include "list.h"

#define MAXNAME 100

void Cmd_open (char * tr[], listaFicheros *ficheros);

void Cmd_close(char *tr[], listaFicheros *ficheros);

char* NombreFicheroDescriptor(listaFicheros ficheros, int df);

void Cmd_dup(char *tr[], listaFicheros *ficheros);

#endif
