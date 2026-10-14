/*Alumnos:
/ Miguel Alessandro Arijon Alvarez
/ Saul Raposo Raposo
*/

#include "list.h"
#include "shell.h"
#include "open.h"
#include "command.h"
#include "dir_proc_list.h"

static char **copyEnvironment(char *envp[]) {
    size_t count = 0;
    while (envp[count] != NULL) count++;

    char **copy = calloc(count + 1, sizeof(char *));
    if (copy == NULL) return NULL;
    for (size_t i = 0; i < count; i++) {
        copy[i] = strdup(envp[i]);
        if (copy[i] == NULL) {
            freeEnvironmentCopy(copy);
            return NULL;
        }
    }
    return copy;
}

int main(int argc, char *argv[], char *envp[]){
    (void)argc;
    (void)argv;

    listHistorico listHist;
    listaFicheros listFich;
    listMemblocks mem;
    SearchList search;
    BackgroundProcessList bgList;
    char **mainEnv = copyEnvironment(envp);

    if (mainEnv == NULL) {
        perror("No se pudo copiar el entorno");
        return EXIT_FAILURE;
    }

    initHistorico(&listHist);
    initFicheros(&listFich);
    iniListMemblocks(&mem);
    initSearchList(&search);
    initBackgroundProcessList(&bgList);

    imprimirPrompt();
    char *input = NULL;
    while (true){
        input = leerEntrada();
        if(input == NULL) break;
        procesarEntrada(&listHist, input, &listFich, &mem, &search, &bgList, mainEnv);
        free(input);
        imprimirPrompt();
    }
    closeAllFiles(&listFich);

    freeCommandHistory(&listHist);
    freeFicheros(&listFich);
    vaciarListaMemblocks(&mem);
    clearSearchList(&search);
    freeBackgroundProcessList(&bgList);
    freeEnvironmentCopy(mainEnv);

    return 0;
}
