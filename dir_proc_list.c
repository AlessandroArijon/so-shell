#include "dir_proc_list.h"
#include <errno.h>
#include <sys/wait.h>

void initBackgroundProcessList(BackgroundProcessList *list) {
    list->head = NULL;
}

void addBackgroundProcess(BackgroundProcessList *list, pid_t pid, const char *command_line) {
    BackgroundProcess *new_process = malloc(sizeof(BackgroundProcess));
    if (!new_process) {
        perror("Failed to allocate memory for background process");
        return;
    }
    new_process->pid = pid;
    new_process->launch_time = time(NULL);
    new_process->status = strdup("ACTIVE");
    new_process->command_line = strdup(command_line);
    if (new_process->status == NULL || new_process->command_line == NULL) {
        perror("strdup");
        free(new_process->status);
        free(new_process->command_line);
        free(new_process);
        return;
    }
    new_process->next = list->head;
    list->head = new_process;
}

void updateBackgroundProcess(BackgroundProcessList *list, pid_t pid, const char *status) {
    for (BackgroundProcess *curr = list->head; curr != NULL; curr = curr->next) {
        if (curr->pid == pid) {
            char *new_status = strdup(status);
            if (new_status == NULL) {
                perror("strdup");
                return;
            }
            free(curr->status);
            curr->status = new_status;
            return;
        }
    }
}

void printBackgroundProcessList(const BackgroundProcessList *list) {
    for (BackgroundProcess *curr = list->head; curr != NULL; curr = curr->next) {
        char date[32];
        struct tm *tm_info = localtime(&curr->launch_time);
        if (tm_info == NULL) strcpy(date, "fecha desconocida");
        else strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", tm_info);
        printf("PID: %d, inicio: %s, estado: %s, comando: %s\n",
               curr->pid, date, curr->status, curr->command_line);
    }
}

void refreshBackgroundProcesses(BackgroundProcessList *list) {
    for (BackgroundProcess *curr = list->head; curr != NULL; curr = curr->next) {
        if (strcmp(curr->status, "ACTIVE") != 0) continue;
        int status;
        pid_t result = waitpid(curr->pid, &status, WNOHANG);
        if (result == curr->pid) {
            if (WIFEXITED(status)) updateBackgroundProcess(list, curr->pid, "TERMINATED");
            else if (WIFSIGNALED(status)) updateBackgroundProcess(list, curr->pid, "SIGNALED");
        } else if (result == -1 && errno != EINTR && errno != ECHILD) {
            perror("waitpid");
        }
    }
}

void removeBackgroundProcessesByStatus(BackgroundProcessList *list, const char *status) {
    BackgroundProcess **link = &list->head;
    while (*link != NULL) {
        BackgroundProcess *curr = *link;
        if (strcmp(curr->status, status) == 0) {
            *link = curr->next;
            free(curr->status);
            free(curr->command_line);
            free(curr);
        } else {
            link = &curr->next;
        }
    }
}

void freeBackgroundProcessList(BackgroundProcessList *list) {
    BackgroundProcess *curr = list->head;
    while (curr != NULL) {
        BackgroundProcess *next = curr->next;
        free(curr->status);
        free(curr->command_line);
        free(curr);
        curr = next;
    }
    list->head = NULL;
}

void removeFinishedProcesses(BackgroundProcessList *list) {
    BackgroundProcess *curr = list->head;
    BackgroundProcess *prev = NULL;
    while (curr) {
        if (strcmp(curr->status, "TERMINATED") == 0 || strcmp(curr->status, "SIGNALED") == 0) {
            if (prev) {
                prev->next = curr->next;
            } else {
                list->head = curr->next;
            }
            free(curr->status);
            free(curr->command_line);
            free(curr);
            curr = (prev) ? prev->next : list->head;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void initSearchList(SearchList *list) {
    list->head = NULL;
}

void addSearchPath(SearchList *list, const char *directory) {
    SearchPath *newNode = (SearchPath *)malloc(sizeof(SearchPath));
    if (!newNode) {
        perror("Error al asignar memoria para el nodo de búsqueda");
        return;
    }

    newNode->directory = strdup(directory);
    if (!newNode->directory) {
        perror("Error al duplicar la cadena del directorio");
        free(newNode);  
        return;
    }

    newNode->next = list->head;
    list->head = newNode;

    printf("Añadido '%s' a la lista. Nueva cabeza: %p\n", directory, (void *)list->head);
}

void removeSearchPath(SearchList *list, const char *directory) {
    SearchPath *curr = list->head;
    SearchPath *prev = NULL;
    while (curr!=NULL) {
        if (strcmp(curr->directory, directory) == 0) {
            if (prev==NULL) {
                list->head = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr->directory);
            free(curr);            
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void clearSearchList(SearchList *list) {
    SearchPath *current = list->head;
    while (current != NULL) {
        SearchPath *next = current->next;
        free(current->directory); 
        free(current);            
        current = next;
    }
    list->head = NULL;  
}

int importFromPath(SearchList *list, const char *path_env) {
    char *path_copy = strdup(path_env);  
    if (path_copy == NULL) {
        return -1;
    }

    int count = 0;
    char *dir = strtok(path_copy, ":"); 
    while (dir != NULL) {
        addSearchPath(list, dir);
        count++;
        dir = strtok(NULL, ":");   
    }

    free(path_copy);  
    return count;
}


void printSearchList(const SearchList *list) {
    if (!list || !list->head) {
        printf("La lista de búsqueda está vacía.\n");
        return;
    }

    printf("Contenido de la lista de búsqueda:\n");
    for (SearchPath *curr = list->head; curr != NULL; curr = curr->next) {
        printf(" - %s (nodo: %p)\n", curr->directory, (void *)curr);
    }
}

