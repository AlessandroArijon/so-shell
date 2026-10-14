/*Alumnos:
/ Miguel Alessandro Arijon Alvarez
/ Saul Raposo Raposo
*/

#ifndef P0SO_COMMAND2_H
#define P0SO_COMMAND2_H


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
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <ctype.h>
#include <limits.h>
#include <stdint.h>
#include <sys/wait.h>

#include "listNodes.h"
#include "list.h"
#include "command.h"

void allocate(char *tr[], listMemblocks *l, listaFicheros *fiche);
int AnadirMemblockMmap(MemType type, char *filename, size_t size, void *p, time_t time, int df, listMemblocks *mem);
void deallocate(char *tr[], listMemblocks *l, listaFicheros *ficheros);
void memory(char *tr[], listMemblocks l);
void memfill(char *tr[], listMemblocks l);
void memdump_aux(char *tr[], listMemblocks l);
void Cmd_ReadFile(char *ar[], listMemblocks l);
void writefile(char *file, void *addr, size_t cont, int overwrite, listMemblocks l);
void read_descriptor(int df, void *addr, size_t cont, listMemblocks l);
void write_descriptor(int df, void *addr, size_t cont, listMemblocks l);
void recurse_command(char *tr[]);
int is_valid_range(const void *p, size_t count, listMemblocks l);
void Do_pmap(void);



#endif //P0SO_COMMAND2_H
