/*Alumnos:
/ Miguel Alessandro Arijon Alvarez
/ Saul Raposo Raposo
*/

#include "open.h"

void Cmd_open(char *tr[], listaFicheros *ficheros) {
    int i, mode = O_RDONLY;
    int access_mode = O_RDONLY;

    if (tr[1] == NULL) {
        ListarFicherosAbiertos(*ficheros);
        return;
    }

    for (i = 2; tr[i] != NULL; i++) {
        if (!strcmp(tr[i], "cr")) mode |= O_CREAT;
        else if (!strcmp(tr[i], "ex")) mode |= O_EXCL;
        else if (!strcmp(tr[i], "ro")) access_mode = O_RDONLY;
        else if (!strcmp(tr[i], "wo")) access_mode = O_WRONLY;
        else if (!strcmp(tr[i], "rw")) access_mode = O_RDWR;
        else if (!strcmp(tr[i], "ap")) mode |= O_APPEND;
        else if (!strcmp(tr[i], "tr")) mode |= O_TRUNC;
        else {
            fprintf(stderr, "Modo de apertura no válido: %s\n", tr[i]);
            return;
        }
    }
    mode = (mode & ~O_ACCMODE) | access_mode;

    if (abrirArchivo(tr[1], mode, ficheros) == -1){
        fprintf(stderr, "Error al abrir el archivo '%s': %s\n", tr[1], strerror(errno));
        return;
    }    
    ListarFicherosAbiertos(*ficheros);
}


void Cmd_close(char *tr[], listaFicheros *ficheros) {
    int df;
    long val;
    char *endptr;
    if (tr[1] == NULL) { /*no hay parametro*/
        ListarFicherosAbiertos(*ficheros);
        return;
    }
    errno = 0;
    val = strtol(tr[1], &endptr, 10);
    if (errno == ERANGE || *endptr != '\0' || val < INT_MIN || val > INT_MAX) {
        printf("El descriptor no es un número válido.\n");
        return;
    }
    df = (int)val;

    if (cerrarArchivo(df, ficheros) == -1) {
        fprintf(stderr, "Error al cerrar el descriptor %d: %s\n", df, strerror(errno));
        return;
    }    
    ListarFicherosAbiertos(*ficheros);
}

char* NombreFicheroDescriptor(listaFicheros ficheros, int df) {
    FileNode* current = ficheros;
    while (current != NULL) {
        if (current->df == df) {
            return current->filename;
        }
        current = current->next;
    }
    return "desconcido";
}


void Cmd_dup(char *tr[], listaFicheros *ficheros) {
    int df, duplicado;
    char aux[MAXNAME];
    long val;
    char *p, *endptr;

    if (tr[1] == NULL) {
        ListarFicherosAbiertos(*ficheros);
        return;
    }
    errno = 0;
    val = strtol(tr[1], &endptr, 10);
    if (errno == ERANGE || *endptr != '\0' || val < INT_MIN || val > INT_MAX) {
        printf("El descriptor no es un número válido.\n");
        return;
    }
    df = (int)val;

    if((duplicado = dup(df)) == -1) {
        fprintf(stderr, "Error no se pudo duplicar': %s\n", strerror(errno));
        return;
    }

    p = NombreFicheroDescriptor(*ficheros, df);
    snprintf(aux, sizeof(aux), "dup %d (%.80s)", df, p);
    AnadirAFicherosAbiertos(duplicado, aux, ficheros);
}
