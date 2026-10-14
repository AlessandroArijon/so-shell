/*Alumnos:
/ Miguel Alessandro Arijon Alvarez
/ Saul Raposo Raposo
*/

#ifndef P0SO_COMMAND_H
#define P0SO_COMMAND_H

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

char LetraTF (mode_t m);

char * ConvierteModo (mode_t m, char *permisos);

int makefile(char *tr[]);

int makedir(char *nombreDir);

void listfile(char *filename, int long_format, int show_link, int show_acc_time);

void cwd(void);

void listdir(char *dir_name, int show_hidden, int long_format, int show_link, int show_acc_time);

void reclist(const char *dir_name, int show_hidden, int depth);

void revlist(char *dir_name, int show_hidden, int long_format, int show_link, int show_acc_time);

void erase(char *tr[]);

void delrec(const char *path);


#endif
