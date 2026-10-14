/*Alumnos:
/ Miguel Alessandro Arijon Alvarez
/ Saul Raposo Raposo
*/

#ifndef P0SO_LISTNODES_H
#define P0SO_LISTNODES_H

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



typedef enum { MALLOC, SHARED, MMAP } MemType;

typedef struct MemNode {
   void *address;
   size_t size;
   time_t time;
   MemType type;
   key_t key;
   char *filename;
   int df;
   struct MemNode * next;
}MemNode;



typedef MemNode *listMemblocks;

void iniListMemblocks(listMemblocks *l);

int AnadirMemblock(MemType type, key_t key, size_t size, void *p, time_t time, listMemblocks *mem);

void printType(MemNode current);

void imprimirListaTipo(listMemblocks l, MemType type);

void vaciarListaMemblocks(listMemblocks *l);

#endif //P0SO_LISTNODES_H
