#ifndef COMMAND3_H
#define COMMAND3_H

#include "dir_proc_list.h"
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <signal.h>
#include <sys/stat.h>

#define MAXVAR 256
#define MAXENV 128
#define MAXARGS 128

void executeGetUID(void);
void executeSetUID(int argc, char *argv[]);
void showvar(int argc, char *argv[], char *envp[]);
void changevar(char *option, char *var, char *val, char **arg3);
void environCommand(char *option, char **arg3, int argc);
void subsvar(char *option, char *var1, char *var2, char *val, char *trozos[]);
void procesarFork(char *args[]);
void processSearchCommand(SearchList *list, int argc, char *argv[]);
void execCommand(char *tr[], SearchList *searchList);
void Cmd_execpri(char *args[]);
void Cmd_fg(char *tr[], char *envp[], SearchList *searchList);
void Cmd_fgpri(char *tr[], char *envp[], SearchList *searchList);
void Cmd_back(char *tr[], char *envp[], BackgroundProcessList *list, SearchList *searchList);
void Cmd_backpri(int prio, char *tr[], char *envp[], BackgroundProcessList *list, SearchList *searchList);
void listjobs(BackgroundProcessList *bgList);
void deljobs(BackgroundProcessList *bgList, const char *status);

#endif
