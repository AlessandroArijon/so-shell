/*Alumnos:
/ Miguel Alessandro Arijon Alvarez
/ Saul Raposo Raposo
*/

#include "shell.h"

extern char **environ;

static const char *authors_names[] = {"Miguel Alessandro Arijón Alvarez", "Saul Raposo Raposo"};
static const char *authors_logins[] = {"alessandro.arijon", "saul.raposo"};

typedef struct {
    const char *command;
    const char *description;
} CommandHelp;

static const CommandHelp commands[] = {
    {"help", "help [cmd|-lt|-T]\tMuestra ayuda sobre los comandos\n\t-lt: lista topics de ayuda\n\t-T topic: lista comandos sobre ese topic\n\tcmd: info sobre el comando cmd\n"},
    {"authors", "authors [-n|-l]\tMuestra los nombres y/o logins de los autores\n"},
    {"date", "date [-d|-t]\tMuestra la fecha y/o la hora actual\n"},
    {"infosys", "infosys\tMuestra informacion de la maquina donde corre el shell\n"},
    {"historic", "historic [-c|-N|N]\tMuestra (o borra)el historico de comandos\n\t	-N: muestra los N primeros\n\t	-c: borra el historico\n\t	N: repite el comando N\n"},
    {"pid", "pid [-p]\tMuestra el pid del shell o de su proceso padre\n"},
    {"ppid", "ppid\tMuestra el pid del proceso padre del shell\n"},
    {"cwd", "cwd\tMuestra el directorio actual del shell\n"},
    {"open", "open fich m1 m2...\tAbre el fichero fich\n\ty lo anade a la lista de ficheros abiertos del shell\n\tm1, m2..es el modo de apertura (or bit a bit de los siguientes)\n\tcr: O_CREAT	ap: O_APPEND\n\tex: O_EXCL 	ro: O_RDONLY\n\trw: O_RDWR 	wo: O_WRONLY\n\ttr: O_TRUNC\n"},
    {"close", "close df\tCierra el descriptor df y elimina el correspondiente fichero de la lista de ficheros abiertos\n"},
    {"dup", "dup df\tDuplica el descriptor de fichero df y anade una nueva entrada a la lista ficheros abiertos\n"},
    {"makefile", "makefile [name]\tCrea un fichero de nombre name\n"},
    {"makedir", "makedir [name]\tCrea un directorio de nombre name\n"},
    {"listfile", "listfile [-long][-link][-acc] name1 name2 ..\tlista ficheros;\n\t-long: listado largo\n\t-acc: acesstime\n\t-link: si es enlace simbolico, el path contenido\n"},
    {"listdir", "listdir [-hid][-long][-link][-acc] n1 n2 ..\tlista contenidos de directorios\n\t-long: listado largo\n\t-hid: incluye los ficheros ocultos\n\t-acc: acesstime\n\t-link: si es enlace simbolico, el path contenido\n"},
    {"erase", "erase [name1 name2 ..]\tBorra ficheros o directorios vacios\n"},
    {"cd", "cd [ruta_del_directorio]\tCambia el directorio actual a la ruta especificada. Si no se proporciona ninguna ruta, cambia al directorio home del usuario.\n"},
    {"revlist","revlist [-hid][-long][-link][-acc] n1 n2 ..\tlista recursivamente contenidos de directorios (subdirs antes)\n\t-hid: incluye los ficheros ocultos\n\t-long: listado largo\n\t-acc: acesstime\n\t-link: si es enlace simbolico, el path contenido\n"},
    {"delrec", "delrec [name1 name2 ..]\tBorra ficheros o directorios no vacios recursivamente\n"},
    {"allocate", "allocate [-malloc|-shared|-createshared|-mmap] ..\tAsigna un bloque de memoria\n\t-malloc tam: asigna un bloque malloc de tamano tam\n\t-createshared cl tam: asigna (creando) el bloque de memoria compartida de clave cl y tamano tam\n\t-shared cl: asigna el bloque de memoria compartida (ya existente) de clave cl\n\t-mmap fich perm: mapea el fichero fich, perm son los permisos\n"},
    {"deallocate", "deallocate [-malloc|-shared|-delkey|-mmap|addr] ..\tDesasigna un bloque de memoria\n\t-malloc tam: desasigna el bloque malloc de tamano tam\n\t-shared cl: desasigna (desmapea) el bloque de memoria compartida de clave cl\n\t-delkey cl: elimina del sistema (sin desmapear) la clave de memoria cl\n\t-mmap fich: desmapea el fichero mapeado fich\n\taddr: desasigna el bloque de memoria en la direccion addr\n"},
    {"memfill", "memfill addr cont byte\tLlena la memoria a partir de addr con byte\n"},
    {"memdump", "memdump addr cont\tVuelca en pantallas los contenidos (cont bytes) de la posicion de memoria addr\n"},
    {"memory", "memory [-blocks|-funcs|-vars|-all|-pmap] ..\tMuestra muestra detalles de la memoria del proceso\n\t-blocks: los bloques de memoria asignados\n\t-funcs: las direcciones de las funciones\n\t-vars: las direcciones de las variables\n\t:-all: todo\n\t-pmap: muestra la salida del comando pmap(o similar)\n"},
    {"readfile", "readfile fiche addr cont\tLee cont bytes desde fich a la direccion addr\n"},
    {"writefile", "writefile [-o] fiche addr cont\tEscribe cont bytes desde la direccion addr a fich (-o sobreescribe)\n"},
    {"read", "read df addr cont\tTransfiere cont bytes del fichero descrito por df a la dirección addr\n"},
    {"write", "write df addr cont\tTransfiere cont bytes desde la dirección addr al fichero descrito por df\n"},
    {"recurse", "recurse [n]\tInvoca a la funcion recursiva n veces\n"},
    {"getuid", "getuid\tMuestra las credenciales del proceso que ejecuta el shell"},
    {"setuid", "setuid [-l] id\tCambia las credenciales del proceso que ejecuta el shell\n\tid: establece la credencial al valor numerico id\n\t-l id: establece la credencial a login id"},
    {"showvar", "showvar var\tMuestra el valor y las direcciones de la variable de entorno var"},
    {"changevar", "changevar [-a|-e|-p] var valor\tCambia el valor de una variable de entorno\n\t-a: accede por el tercer arg de main\n\t-e: accede mediante environ\n\t-p: accede mediante putenv"},
    {"environ", "environ[-environ|-addr]\tMuestra el entorno del proceso\n\t-environ: accede usando environ (en lugar del tercer arg de main)\n\t-addr: muestra el valor y donde se almacenan environ y el 3er arg main "},
    {"subsvar", "subsvar [-a|-e] var1 var2 valor\tSustituye la variable de entorno var1\n\tcon var2=valor\n\t-a: accede por el tercer arg de main\n\t-e: accede mediante environ"},
    {"fork", "fork\tEl shell hace fork y queda en espera a que su hijo termine"},
    {"search", "search [-add|-del|-clear|-path]..\tManipula o muestra la ruta de busqueda del shell (path)\n\t-add dir: aniade dir a la ruta de busqueda(equiv +dir)\n\t-del dir: elimina dir de la ruta de busqueda (equiv -dir)\n\t-clear: vacia la ruta de busqueda\n\t-path: importa el PATH en la ruta de busqueda"},
    {"exec", "exec VAR1 VAR2 ..prog args....[@pri]\tEjecuta, sin crear proceso,prog con argumentos\n\ten un entorno que contiene solo las variables VAR1, VAR2..."},
    {"execpri", "execpri prio prog args....\tEjecuta, sin crear proceso, prog con argumentos con la prioridad cambiada a prio"},
    {"fg", "fg prog args...\tCrea un proceso que ejecuta en primer plano prog con argumentos"},
    {"fgpri", "fgpri prio prog args...\tCrea un proceso que ejecuta en primer plano prog con argumentos  con la prioridad cambiada a prio"}
};


static const int num_commands = sizeof(commands) / sizeof(commands[0]);


void imprimirPrompt(void){
    printf("saul&alex:~$ ");
}

char* leerEntrada(void) {
    char *input = malloc(MAX_INPUT_SIZE * sizeof(char));
    if(input == NULL) {
        printf("Error al asignar memoria: %s\n", strerror(errno));
        return NULL;
    }
    if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
        free(input);
        return NULL;
    }
    input[strcspn(input, "\n")] = '\0';

    return input;
}

int TrocearCadena(char *input, char *trozos[]){
    int count = 0;
    char *token = strtok(input, " \n\t");
    while (token != NULL && count < MAX_TROZOS - 1) {
        trozos[count++] = token;
        token = strtok(NULL, " \n\t");
    }
    trozos[count] = NULL;
    return count;
}

void printLastNCommands(listHistorico list, int N) {
    if (N < 0) return;
    if (N > list.count) N = list.count;

    CommandNode *current = list.head;
    for (int i = 0; i < N && current != NULL; i++) {
        printf("%d: %s\n", i + 1, current->command);
        current = current->next;
    }
}

char* getCommandByNumber(listHistorico list, int N) {
    if (N > list.count || N < 1) return NULL;
    CommandNode *current = list.head;
    for (int i = 1; i < N; i++) {
        current = current->next;
    }
    return current->command;
}


void printDateAndTime(int showDate, int showTime) {
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    if (showDate) {
        strftime(buffer, sizeof(buffer), "%d/%m/%Y", timeinfo);
        printf("Fecha actual: %s\n", buffer);
    }

    if (showTime) {
        strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
        printf("Hora actual: %s\n", buffer);
    }
}


void printSystemInfo(void) {
    struct utsname sys_info;

    if (uname(&sys_info) == -1) {
        printf("Error obteniendo informacion del sistema: %s\n", strerror(errno));
        return;
    }
    printf("Información del sistema:\n");
    printf("Sistema operativo: %s\n", sys_info.sysname);
    printf("Nombre del nodo (hostname): %s\n", sys_info.nodename);
    printf("Versión del kernel: %s\n", sys_info.release);
    printf("Versión del sistema operativo: %s\n", sys_info.version);
    printf("Arquitectura de la máquina: %s\n", sys_info.machine);
}


void printAuthors(int printNames, int printLogins) {
    int num_authors = sizeof(authors_names) / sizeof(authors_names[0]);
    if (printNames && printLogins) {
        for (int i = 0; i < num_authors; i++) {
            printf("Nombre: %s, Login: %s\n", authors_names[i], authors_logins[i]);
        }
    } else if (printNames) {
        for (int i = 0; i < num_authors; i++) {
            printf("Nombre: %s\n", authors_names[i]);
        }
    } else if (printLogins) {
        for (int i = 0; i < num_authors; i++) {
            printf("Login: %s\n", authors_logins[i]);
        }
    }
}

void procesarAuthorsCommand(int numPalabras, char *trozos[]) {
    if (numPalabras == 1) {
        printAuthors(1, 1);
    } else if (numPalabras == 2) {
        if (strcmp(trozos[1], "-n") == 0) {
            printAuthors(1, 0);
        } else if (strcmp(trozos[1], "-l") == 0) {
            printAuthors(0, 1);
        } else {
            printf("Opción no válida. Usa -n para nombres o -l para logins.\n");
        }
    } else {
        printf("Uso incorrecto del comando. Sintaxis: authors [-n|-l]\n");
    }
}

void procesarDateCommand(int numPalabras, char *trozos[]) {
    if (numPalabras == 1) {
        printDateAndTime(1, 1);
    } else if (numPalabras == 2) {
        if (strcmp(trozos[1], "-d") == 0) {
            printDateAndTime(1, 0);
        } else if (strcmp(trozos[1], "-t") == 0) {
            printDateAndTime(0, 1);
        } else {
            printf("Opción no válida. Usa -d para la fecha o -t para la hora.\n");
        }
    } else {
        printf("Uso incorrecto del comando. Sintaxis: date [-d|-t]\n");
    }
}

void printHelp(const char *cmd) {
    if (cmd == NULL) {
        printf("Lista de comandos disponibles:\n");
        for (int i = 0; i < num_commands; i++) {
            printf("%s: %s\n", commands[i].command, commands[i].description);
        }
    } else {
        for (int i = 0; i < num_commands; i++) {
            if (strcmp(commands[i].command, cmd) == 0) {
                printf("Ayuda para el comando '%s':\n%s", cmd, commands[i].description);
                return;
            }
        }
        printf("Error: Comando '%s' no encontrado. Usa 'help' para ver la lista de comandos.\n", cmd);
    }
}

void procesarHelpCommand(int numPalabras, char *trozos[]) {
    if (numPalabras == 1) {
        printHelp(NULL);
    } else if (numPalabras == 2) {
        printHelp(trozos[1]);
    } else {
        printf("Uso incorrecto del comando. Sintaxis: help [cmd]\n");
    }
}
void limpiarRecursos(listHistorico *hist, listaFicheros *ficheros, listMemblocks *mem) {
    freeCommandHistory(hist);
    vaciarListaMemblocks(mem);
    closeAllFiles(ficheros);
}

void freeEnvironmentCopy(char *envp[]) {
    if (envp == NULL) return;
    for (size_t i = 0; envp[i] != NULL; i++) free(envp[i]);
    free(envp);
}

void ejecutarComandoExterno(char *comando, char *args[]) {
    pid_t pid = fork();

    if (pid == 0) {
        if (execvp(comando, args) == -1) {
            perror("exec");
            _exit(EXIT_FAILURE);
        }
    } else if (pid < 0) {
        perror("fork");
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return;
        }

        if (WIFEXITED(status)) {
            printf("Proceso finalizado con código de salida %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Proceso finalizado por señal %d\n", WTERMSIG(status));
        }
    }
}


void procesarEntrada(listHistorico *hist, char *input, listaFicheros *ficheros, listMemblocks *mem, SearchList *searchList, BackgroundProcessList *bgList, char *envp[]) {
    char *trozos[MAX_TROZOS];
    char *original = strdup(input);
    static unsigned int replay_depth = 0;

    if (original == NULL) {
        perror("strdup");
        return;
    }
    int numPalabras = TrocearCadena(input, trozos);

    if(numPalabras == 0) {
        free(original);
        return;
    }
    addCommand(hist, original);
    free(original);

    if (strcmp(trozos[0], "quit") == 0 || strcmp(trozos[0], "exit") == 0 || strcmp(trozos[0], "bye") == 0) {
        printf("Saliendo del shell...\n");
        clearSearchList(searchList);
        freeBackgroundProcessList(bgList);
        limpiarRecursos(hist, ficheros, mem);
        freeEnvironmentCopy(envp);
        exit(EXIT_SUCCESS);
    }
    else if (strcmp(trozos[0], "back") == 0) {
        if (numPalabras < 2) {
            fprintf(stderr, "Uso: back <comando> [args]\n");
        } else {
            Cmd_back(trozos + 1, environ, bgList, searchList);
        }
    }
    else if (strcmp(trozos[0], "backpri") == 0) {
        if (numPalabras < 3) {
            fprintf(stderr, "Uso: backpri <prioridad> <comando> [args]\n");
        } else {
            char *endptr;
            long prio = strtol(trozos[1], &endptr, 10);
            if (*endptr != '\0' || prio < -20 || prio > 19)
                fprintf(stderr, "Prioridad no válida: %s\n", trozos[1]);
            else
                Cmd_backpri((int)prio, trozos + 2, environ, bgList, searchList);
        }
    }
    else if(strcmp(trozos[0], "allocate") == 0) {
        allocate(trozos, mem, ficheros);
    }
    else if(strcmp(trozos[0], "deallocate") == 0){
        deallocate(trozos, mem, ficheros);
    }
    else if(strcmp(trozos[0], "memory") == 0) {
        memory(trozos, *mem);
    }
    else if(strcmp(trozos[0], "memfill") == 0) {
        memfill(trozos, *mem);
    }
    else if(strcmp(trozos[0], "memdump") == 0) {
        memdump_aux(trozos, *mem);
    }
    else if (strcmp(trozos[0], "open") == 0) {
        Cmd_open(trozos, ficheros);
    }
    else if (strcmp(trozos[0], "close") == 0) {
        Cmd_close(trozos, ficheros);
    }
    else if (strcmp(trozos[0], "dup") == 0) {
        Cmd_dup(trozos, ficheros);
    }
    else if(strcmp(trozos[0], "erase") == 0) {
        erase(trozos);
    }
    else if(strcmp(trozos[0], "makefile") == 0) {
        makefile(trozos);
    }
    else if(strcmp(trozos[0], "makedir") == 0) {
        makedir(trozos[1]);
    }
    else if(strcmp(trozos[0], "delrec") == 0) {
        if (numPalabras > 1) {
            for (int i = 1; i < numPalabras; i++) delrec(trozos[i]);
        }
        else cwd();
    }
    else if(strcmp(trozos[0], "listfile") == 0) {
        auxList(numPalabras, trozos, 1);
    }
    else if(strcmp(trozos[0], "listdir") == 0) {
        auxList(numPalabras, trozos, 2);
    }
    else if(strcmp(trozos[0], "reclist") == 0) {
        auxList(numPalabras, trozos, 3);
    }
    else if(strcmp(trozos[0], "revlist") == 0){
        auxList(numPalabras, trozos, 4);
    }
    else if(strcmp(trozos[0], "help") == 0){
        procesarHelpCommand(numPalabras, trozos);
    }
    else if (strcmp(trozos[0], "pid") == 0){
        printf("PID del proceso: %d\n", getpid());
    }
    else if(strcmp(trozos[0], "date") == 0) {
        procesarDateCommand(numPalabras, trozos);
    }
    else if (strcmp(trozos[0], "ppid") == 0){
        printf("PPID del proceso: %d\n", getppid());
    }
    else if (strcmp(trozos[0], "infosys") == 0){
        printSystemInfo();
    }
    else if(strcmp(trozos[0], "authors") == 0){
        procesarAuthorsCommand(numPalabras, trozos);
    }
    else if(strcmp(trozos[0], "readfile") == 0){
        Cmd_ReadFile(trozos, *mem);
    }
    else if(strcmp(trozos[0], "cwd") == 0){
        cwd();
    }
    else if(strcmp(trozos[0], "recurse") == 0){
        recurse_command(trozos);
    }
    else if(strcmp(trozos[0], "getuid") == 0){
        executeGetUID();
    }
    else if(strcmp(trozos[0], "setuid") == 0){
        executeSetUID(numPalabras, trozos);
    }
    else if(strcmp(trozos[0], "showvar") == 0){
        showvar(numPalabras, trozos, envp);
    }
    else if (strcmp(trozos[0], "changevar") == 0) {
        if (numPalabras < 4) {
            fprintf(stderr, "Uso: changevar [-a|-e|-p] var val\n");
        } 
        else {
            char *option = trozos[1];
            char *var = trozos[2];
            char *val = trozos[3];

            if (strcmp(option, "-a") != 0 && strcmp(option, "-e") != 0 && strcmp(option, "-p") != 0) {
                fprintf(stderr, "Opción inválida: %s. Uso: changevar [-a|-e|-p] var val\n", option);
            } 
            else {
                changevar(option, var, val, envp);
            }
        }
    }
    else if(strcmp(trozos[0], "environ") == 0){
        if (numPalabras < 2) {
            fprintf(stderr, "Uso: environ [-environ|-addr]\n");
        }
        else {
            char *option = trozos[1];
            environCommand(option, envp, numPalabras);
        }
    }
    else if (strcmp(trozos[0], "subsvar") == 0) {
        if (numPalabras < 5) {
            fprintf(stderr, "Uso: subsvar [-a|-e] var1 var2 valor\n");
        }
        else {
            char *option = trozos[1];
            char *var1 = trozos[2];
            char *var2 = trozos[3];
            char *val = trozos[4];

            subsvar(option, var1, var2, val, envp);
        }
    }
    else if (strcmp(trozos[0],"fork") == 0){
        if (numPalabras < 2) {
            fprintf(stderr, "Uso: fork <comando>\n");
        } else {
            procesarFork(trozos + 1);
        }
    }
    else if (strcmp(trozos[0], "search") == 0){
        processSearchCommand(searchList, numPalabras, trozos);
    }
    else if (strcmp(trozos[0], "exec") == 0){
        execCommand(trozos + 1, searchList);
    }
    else if (strcmp(trozos[0], "execpri") == 0){
        Cmd_execpri(trozos);
    }
    else if (strcmp(trozos[0], "fg") == 0){
        if (numPalabras < 2) fprintf(stderr, "Uso: fg prog args...\n");
        else Cmd_fg(trozos + 1, environ, searchList);
    }
    else if (strcmp(trozos[0], "fgpri") == 0){
        if (numPalabras < 2) fprintf(stderr, "Uso: fgpri prio prog args...\n");
        else Cmd_fgpri(trozos + 1, environ, searchList);
    }
    else if (strcmp(trozos[0], "listjobs") == 0) {
        listjobs(bgList);
    } 
    else if (strcmp(trozos[0], "deljobs") == 0) {
    if (numPalabras < 2) {
        printf("Se requiere un argumento para 'deljobs' (-term o -sig).\n");
    } else {
        char *arg = trozos[1];
        if (strcmp(arg, "-term") == 0) {
            deljobs(bgList, "-term");
        } else if (strcmp(arg, "-sig") == 0) {
            deljobs(bgList, "-sig");
        } else {
            printf("Argumento inválido para 'deljobs'. Uso: deljobs -term|-sig.\n");
        }
    }
}

    else if (strcmp(trozos[0], "writefile") == 0){
        int offset = (numPalabras > 1 && strcmp(trozos[1], "-o") == 0) ? 1 : 0;
        if (numPalabras < 4 + offset) {
            fprintf(stderr, "Uso: writefile [-o] file addr cont\n");
        } else {
            char *endptr;
            void *addr = (void *)(uintptr_t)strtoull(trozos[2 + offset], &endptr, 0);
            if (*endptr != '\0') fprintf(stderr, "Dirección no válida\n");
            else {
                size_t cont = (size_t)strtoull(trozos[3 + offset], &endptr, 10);
                if (*endptr != '\0') fprintf(stderr, "Tamaño no válido\n");
                else writefile(trozos[1 + offset], addr, cont, offset, *mem);
            }
        }
    }
    else if (strcmp(trozos[0], "read") == 0){
        if (trozos[1] == NULL || trozos[2] == NULL || trozos[3] == NULL){
            fprintf(stderr, "Uso: read df addr cont\n");
        } else {
            char *endptr;
            long df_value = strtol(trozos[1], &endptr, 10);
            if (*endptr != '\0') fprintf(stderr, "Descriptor no válido\n");
            else {
                void *addr = (void *)(uintptr_t)strtoull(trozos[2], &endptr, 0);
                if (*endptr != '\0') fprintf(stderr, "Dirección no válida\n");
                else {
                    size_t cont = (size_t)strtoull(trozos[3], &endptr, 10);
                    if (*endptr != '\0') fprintf(stderr, "Tamaño no válido\n");
                    else read_descriptor((int)df_value, addr, cont, *mem);
                }
            }
        }
    }
    else if (strcmp(trozos[0], "cd") == 0){
        if (numPalabras > 1){
            if (chdir(trozos[1]) != 0){
                printf("Error al cambiar de directorio: %s\n", strerror(errno));
            }
        }
        else{
            cwd();
        }
    }
    else if (strcmp(trozos[0], "write") == 0){
        if (trozos[1] == NULL || trozos[2] == NULL || trozos[3] == NULL){
            fprintf(stderr, "Uso: write df addr cont\n");
        } else {
            char *endptr;
            long df_value = strtol(trozos[1], &endptr, 10);
            if (*endptr != '\0') fprintf(stderr, "Descriptor no válido\n");
            else {
                void *addr = (void *)(uintptr_t)strtoull(trozos[2], &endptr, 0);
                if (*endptr != '\0') fprintf(stderr, "Dirección no válida\n");
                else {
                    size_t cont = (size_t)strtoull(trozos[3], &endptr, 10);
                    if (*endptr != '\0') fprintf(stderr, "Tamaño no válido\n");
                    else write_descriptor((int)df_value, addr, cont, *mem);
                }
            }
        }
    }

    else if (strcmp(trozos[0], "historic") == 0) {
        if (numPalabras == 1) {
            printHistory(*hist);
        } else if (numPalabras == 2) {
            if (strcmp(trozos[1], "-c") == 0) {
                freeCommandHistory(hist);
            } else if (trozos[1][0] == '-') {
                int N = atoi(trozos[1] + 1);
                printLastNCommands(*hist, N);
            } else {
                int N = atoi(trozos[1]);
                char *history_command = getCommandByNumber(*hist, N);
                if (history_command != NULL) {
                    printf("Ejecutando comando %d: %s\n", N, history_command);
                    char *copy = strdup(history_command);
                    if (copy == NULL) perror("strdup");
                    else if (replay_depth >= 32) {
                        fprintf(stderr, "Límite de repetición del historial alcanzado\n");
                        free(copy);
                    } else {
                        replay_depth++;
                        procesarEntrada(hist, copy, ficheros, mem, searchList, bgList, envp);
                        replay_depth--;
                        free(copy);
                    }
                } else {
                    printf("No existe el comando %d en el historial\n", N);
                }
            }
        }
    }
    else ejecutarComandoExterno(trozos[0], trozos);
}
