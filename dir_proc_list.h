#ifndef DIR_PROC_LIST_H
#define DIR_PROC_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>


typedef struct BackgroundProcess {
    pid_t pid;                   
    time_t launch_time;          
    char *status;                
    char *command_line;          
    struct BackgroundProcess *next;
} BackgroundProcess;

typedef struct {
    BackgroundProcess *head;
} BackgroundProcessList;

void initBackgroundProcessList(BackgroundProcessList *list);
void addBackgroundProcess(BackgroundProcessList *list, pid_t pid, const char *command_line);
void updateBackgroundProcess(BackgroundProcessList *list, pid_t pid, const char *status);
void printBackgroundProcessList(const BackgroundProcessList *list);
void removeFinishedProcesses(BackgroundProcessList *list);
void refreshBackgroundProcesses(BackgroundProcessList *list);
void removeBackgroundProcessesByStatus(BackgroundProcessList *list, const char *status);
void freeBackgroundProcessList(BackgroundProcessList *list);


typedef struct SearchPath {
    char *directory;             
    struct SearchPath *next;
} SearchPath;

typedef struct {
    SearchPath *head;
} SearchList;

void initSearchList(SearchList *list);
void addSearchPath(SearchList *list, const char *directory);
void removeSearchPath(SearchList *list, const char *directory);
void clearSearchList(SearchList *list);
int importFromPath(SearchList *list, const char *path_env);
void printSearchList(const SearchList *list);

#endif
