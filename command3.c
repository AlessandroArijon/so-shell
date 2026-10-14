#include "command3.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

//Código de ayuda:

/*el siguiente codigo se da como ayuda por si se quiere utilizar*/
/*NO ES OBLIGATORIO USARLO*/
/*y pueden usarse funciones enteras o parte de funciones */
 
/*Este fichero, ayudaP3.c no está pensado para ser compilado separadamente */
/*, entre otras cosas, no contiene los includes necesarios*/
/*y las constantes utilizadas, no están definidas en él*/

void Cmd_fork (char *tr[])
{
	(void)tr;
	pid_t pid;
	
	if ((pid=fork())==0){
/*		VaciarListaProcesos(&LP); Depende de la implementación de cada uno*/
		printf ("ejecutando proceso %d\n", getpid());
	}
	else if (pid!=-1)
		waitpid (pid,NULL,0);
}

int BuscarVariable (char * var, char *e[])  /*busca una variable en el entorno que se le pasa como parámetro*/
{                                           /*devuelve la posicion de la variable en el entorno, -1 si no existe*/
  int pos=0;
  char aux[MAXVAR];
  
  if (snprintf(aux, sizeof(aux), "%s=", var) >= (int)sizeof(aux)) {
    errno = ENAMETOOLONG;
    return -1;
  }
  
  while (e[pos]!=NULL)
    if (!strncmp(e[pos],aux,strlen(aux)))
      return (pos);
    else 
      pos++;
  errno=ENOENT;   /*no hay tal variable*/
  return(-1);
}


int CambiarVariable(char * var, char * valor, char *e[]) /*cambia una variable en el entorno que se le pasa como parámetro*/
{                                                        /*lo hace directamente, no usa putenv*/
  int pos;
  char *aux;
   
  if ((pos=BuscarVariable(var,e))==-1)
    return(-1);
 
  if ((aux=(char *)malloc(strlen(var)+strlen(valor)+2))==NULL)
	return -1;
  strcpy(aux,var);
  strcat(aux,"=");
  strcat(aux,valor);
  e[pos]=aux;
  return (pos);
}

// Función que devuelve el primer directorio de la lista de búsqueda
char * Ejecutable (char *s, SearchList *list)
{
    static char path[PATH_MAX];

    if (s == NULL) return NULL;
    if (strchr(s, '/') != NULL)
        return access(s, X_OK) == 0 ? s : NULL;

    if (list != NULL && list->head != NULL) {
        for (SearchPath *node = list->head; node != NULL; node = node->next) {
            int written = snprintf(path, sizeof(path), "%s/%s", node->directory, s);
            if (written >= 0 && (size_t)written < sizeof(path) && access(path, X_OK) == 0)
                return path;
        }
        return NULL;
    }

    const char *path_env = getenv("PATH");
    if (path_env == NULL) return NULL;
    char *copy = strdup(path_env);
    if (copy == NULL) return NULL;
    char *saveptr = NULL;
    for (char *dir = strtok_r(copy, ":", &saveptr); dir != NULL;
         dir = strtok_r(NULL, ":", &saveptr)) {
        int written = snprintf(path, sizeof(path), "%s/%s", dir, s);
        if (written >= 0 && (size_t)written < sizeof(path) && access(path, X_OK) == 0) {
            free(copy);
            return path;
        }
    }
    free(copy);
    return NULL;
}

int Execpve(char *tr[], char **NewEnv, int *pprio, SearchList *searchList) {
    char *p;  // Almacena el camino del ejecutable

    // Verifica si el comando es NULL o si el ejecutable no fue encontrado
    if (tr[0] == NULL || (p = Ejecutable(tr[0], searchList)) == NULL) {
        errno = EFAULT;
        return -1;
    }

    // Si se proporciona una prioridad, intenta cambiarla
    if (pprio != NULL) {
        if (setpriority(PRIO_PROCESS, 0, *pprio) == -1) {
            // Error al cambiar la prioridad
            printf("Imposible cambiar prioridad: %s\n", strerror(errno));
            return -1;
        }
    }

    // Si NewEnv es NULL, usa execv, de lo contrario, usa execve
    if (NewEnv == NULL) {
        return execv(p, tr);  // Ejecuta el programa sin cambiar el entorno
    } else {
        return execve(p, tr, NewEnv);  // Ejecuta el programa con el entorno proporcionado
    }
}


struct SEN {
    char *nombre; // Nombre de la señal (por ejemplo, "HUP")
    int senal;    // Número de la señal (por ejemplo, SIGHUP)
};

/*las siguientes funciones nos permiten obtener el nombre de una senal a partir
del número y viceversa */
static struct SEN sigstrnum[]={   
	{"HUP", SIGHUP},
	{"INT", SIGINT},
	{"QUIT", SIGQUIT},
	{"ILL", SIGILL}, 
	{"TRAP", SIGTRAP},
	{"ABRT", SIGABRT},
#ifdef SIGIOT
	{"IOT", SIGIOT},
#endif
	{"BUS", SIGBUS},
	{"FPE", SIGFPE},
	{"KILL", SIGKILL},
	{"USR1", SIGUSR1},
	{"SEGV", SIGSEGV},
	{"USR2", SIGUSR2}, 
	{"PIPE", SIGPIPE},
	{"ALRM", SIGALRM},
	{"TERM", SIGTERM},
	{"CHLD", SIGCHLD},
	{"CONT", SIGCONT},
	{"STOP", SIGSTOP},
	{"TSTP", SIGTSTP}, 
	{"TTIN", SIGTTIN},
	{"TTOU", SIGTTOU},
	{"URG", SIGURG},
	{"XCPU", SIGXCPU},
	{"XFSZ", SIGXFSZ},
	{"VTALRM", SIGVTALRM},
	{"PROF", SIGPROF},
#ifdef SIGWINCH
	{"WINCH", SIGWINCH},
#endif
#ifdef SIGIO
	{"IO", SIGIO},
#endif
	{"SYS", SIGSYS},
/*senales que no hay en todas partes*/
#ifdef SIGPOLL
	{"POLL", SIGPOLL},
#endif
#ifdef SIGPWR
	{"PWR", SIGPWR},
#endif
#ifdef SIGEMT
	{"EMT", SIGEMT},
#endif
#ifdef SIGINFO
	{"INFO", SIGINFO},
#endif
#ifdef SIGSTKFLT
	{"STKFLT", SIGSTKFLT},
#endif
#ifdef SIGCLD
	{"CLD", SIGCLD},
#endif
#ifdef SIGLOST
	{"LOST", SIGLOST},
#endif
#ifdef SIGCANCEL
	{"CANCEL", SIGCANCEL},
#endif
#ifdef SIGTHAW
	{"THAW", SIGTHAW},
#endif
#ifdef SIGFREEZE
	{"FREEZE", SIGFREEZE},
#endif
#ifdef SIGLWP
	{"LWP", SIGLWP},
#endif
#ifdef SIGWAITING
	{"WAITING", SIGWAITING},
#endif
 	{NULL,-1},
	};    /*fin array sigstrnum */


int ValorSenal(char * sen)  /*devuelve el numero de senial a partir del nombre*/ 
{ 
  int i;
  for (i=0; sigstrnum[i].nombre!=NULL; i++)
  	if (!strcmp(sen, sigstrnum[i].nombre))
		return sigstrnum[i].senal;
  return -1;
}


char *NombreSenal(int sen)  /*devuelve el nombre senal a partir de la senal*/ 
{			/* para sitios donde no hay sig2str*/
 int i;
  for (i=0; sigstrnum[i].nombre!=NULL; i++)
  	if (sen==sigstrnum[i].senal)
		return sigstrnum[i].nombre;
 return ("SIGUNKNOWN");
}



//Comandos:

void executeGetUID(void) {
    uid_t real_uid = getuid();
    uid_t effective_uid = geteuid();

    printf("Credencial real: %d\n", real_uid);
    printf("Credencial efectiva: %d\n", effective_uid);
}

void executeSetUID(int argc, char *argv[]){
    if (argc < 2 || argc > 3){
        executeGetUID();
        return;
    }

    uid_t uid;
    if (argc == 3 && strcmp(argv[1], "-l") == 0){
        struct passwd *pwd = getpwnam(argv[2]);
        if (pwd == NULL){
            fprintf(stderr, "Error: usuario '%s' no encontrado.\n", argv[2]);
            return;
        }
        uid = pwd->pw_uid;
    }
    else if (argc == 2){
        char *endptr;
        uid = (uid_t)strtol(argv[1], &endptr, 10);
        if (*endptr != '\0'){
            fprintf(stderr, "Error: '%s' no es un ID de usuario válido.\n", argv[1]);
            return;
        }
    } 
    else{
        fprintf(stderr, "Uso: setuid [-l] id\n");
        return;
    }

    if (setuid(uid) == -1) fprintf(stderr, "Error al cambiar credencial: %s\n", strerror(errno));
    else printf("Credencial del proceso cambiada a UID: %d\n", uid);
}

extern char **environ;

void showvar(int argc, char *argv[], char *envp[]) {
    if (argc == 1) {
        fprintf(stderr, "Uso: showvar var [var...]\n");
        return;
    }

    for (int i = 1; i < argc; i++) {
        int pos_arg3 = BuscarVariable(argv[i], envp);
        int pos_environ = BuscarVariable(argv[i], environ);
        char *value = getenv(argv[i]);

        if (pos_arg3 >= 0)
            printf("Con arg3 main: %s (%p) @%p\n", envp[pos_arg3],
                   (void *)envp[pos_arg3], (void *)&envp[pos_arg3]);
        if (pos_environ >= 0)
            printf("Con environ: %s (%p) @%p\n", environ[pos_environ],
                   (void *)environ[pos_environ], (void *)&environ[pos_environ]);
        if (value != NULL)
            printf("Con getenv: %s=%s (%p)\n", argv[i], value, (void *)value);
        if (pos_arg3 < 0 && pos_environ < 0 && value == NULL) {
            printf("Variable %s no definida\n", argv[i]);
        }
    }
}

static int env_name_matches(const char *entry, const char *name) {
    size_t len = strlen(name);
    return strncmp(entry, name, len) == 0 && entry[len] == '=';
}

static int replace_env_entry(char *env[], const char *old_name,
                             const char *new_name, const char *value, int free_old) {
    for (size_t i = 0; env[i] != NULL; i++) {
        if (env_name_matches(env[i], old_name)) {
            size_t size = strlen(new_name) + strlen(value) + 2;
            char *replacement = malloc(size);
            if (replacement == NULL) return -1;
            snprintf(replacement, size, "%s=%s", new_name, value);
            if (free_old) free(env[i]);
            env[i] = replacement;
            return 0;
        }
    }
    errno = ENOENT;
    return -1;
}

void changevar(char *option, char *var, char *val, char **arg3){
    if (option == NULL || var == NULL || val == NULL) {
        fprintf(stderr, "Uso: changevar [-a|-e|-p] var valor\n");
        return;
    }

    if (strcmp(option, "-a") == 0) {
        if (replace_env_entry(arg3, var, var, val, 1) == -1)
            fprintf(stderr, "Variable %s no encontrada en arg3\n", var);
        else
            printf("Variable %s cambiada en arg3\n", var);
    } else if (strcmp(option, "-e") == 0) {
        if (getenv(var) == NULL)
            fprintf(stderr, "Variable %s no encontrada en environ\n", var);
        else if (setenv(var, val, 1) == -1)
            perror("setenv");
        else
            printf("Variable %s cambiada en environ\n", var);
    } else if (strcmp(option, "-p") == 0) {
        if (setenv(var, val, 1) == -1) perror("setenv");
        else printf("Variable %s cambiada con setenv\n", var);
    } else {
        fprintf(stderr, "Opción inválida. Use -a, -e o -p.\n");
    }
}

void environCommand(char *option, char **arg3, int argc){
    if (option == NULL) {
        printf("Uso: environ [-environ|-addr]\n");
        return;
    }

    if (strcmp(option, "-environ") == 0) {
        printf("Entorno del proceso (usando environ):\n");
        printf("-------------------------------------\n");
        for (int i = 0; environ[i] != NULL; i++) {
            printf("%s\n", environ[i]);
        }
    } 
    else if (strcmp(option, "-addr") == 0) {
        printf("Direcciones del entorno del proceso:\n");
        printf("-------------------------------------\n");
        printf("Dirección de environ: %p\n", (void *)&environ);
        printf("Dirección de arg3:    %p\n", (void *)&arg3);

        printf("\nContenido de arg3 (variables pasadas al proceso):\n");
        (void)argc;
        for (int i = 0; arg3[i] != NULL; i++) {
            printf("arg3[%d]: %s (dirección: %p)\n", i, arg3[i], (void *)arg3[i]);
        }
    } 
    else {
        printf("Opción no válida. Uso: environ [-environ|-addr]\n");
    }
}

void subsvar(char *option, char *var1, char *var2, char *val, char *arg3[]) {
    char **target;
    if (strcmp(option, "-a") == 0) target = arg3;
    else if (strcmp(option, "-e") == 0) {
        if (getenv(var1) == NULL) {
            fprintf(stderr, "Variable %s no encontrada\n", var1);
        } else if (setenv(var2, val, 1) == -1 ||
                   (strcmp(var1, var2) != 0 && unsetenv(var1) == -1)) {
            perror("subsvar");
        } else {
            printf("Variable %s sustituida por %s=%s\n", var1, var2, val);
        }
        return;
    }
    else {
        fprintf(stderr, "Uso: subsvar [-a|-e] var1 var2 valor\n");
        return;
    }

    if (replace_env_entry(target, var1, var2, val, 1) == -1)
        fprintf(stderr, "Variable %s no encontrada\n", var1);
    else
        printf("Variable %s sustituida por %s=%s\n", var1, var2, val);
}


void procesarFork(char *args[]) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("Error al hacer fork");
        return;
    } else if (pid == 0) {
        printf("ejecutando proceso %d\n", getpid());

        execvp(args[0], args);

        perror("Error al ejecutar el comando");
        _exit(EXIT_FAILURE);
    } else {
        printf("Proceso hijo %d ejecutando %s\n", pid, args[0]);

        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return;
        }

        if (WIFEXITED(status)) {
            printf("Hijo terminado con código de salida %d\n", WEXITSTATUS(status));
        } else {
            printf("El hijo terminó con error\n");
        }
    }
}

void processSearchCommand(SearchList *list, int argc, char *argv[]) {
    if (argc < 2) {
        printSearchList(list);
        return;
    }

    if (strcmp(argv[1], "-add") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Error: Se requiere un directorio para -add.\n");
            return;
        }
        addSearchPath(list, argv[2]);

    } else if (strcmp(argv[1], "-del") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Error: Se requiere un directorio para -del.\n");
            return;
        }
        removeSearchPath(list, argv[2]);

    } else if (strcmp(argv[1], "-clear") == 0) {
        clearSearchList(list);
        printf("La lista de búsqueda ha sido vaciada.\n");

    } else if (strcmp(argv[1], "-path") == 0) {
        const char *path_env = getenv("PATH");
        if (!path_env) {
            fprintf(stderr, "Error: No se pudo obtener la variable de entorno PATH.\n");
            return;
        }
        int count = importFromPath(list, path_env);
        printf("%d entradas importadas desde PATH.\n", count);

    } else {
        fprintf(stderr, "Comando no reconocido: %s\n", argv[1]);
        fprintf(stderr, "Uso: search [-add dir | -del dir | -clear | -path]\n");
    }

    printSearchList(list);
}

void execCommand(char *tr[], SearchList *searchList) {
    char *new_env[MAXENV];
    int env_count = 0;

    while (tr[env_count] != NULL && env_count < MAXENV - 1) {
        int pos = BuscarVariable(tr[env_count], environ);
        if (pos < 0 && strchr(tr[env_count], '=') == NULL) break;
        new_env[env_count] = pos >= 0 ? environ[pos] : tr[env_count];
        env_count++;
    }
    new_env[env_count] = NULL;

    if (tr[env_count] == NULL) {
        fprintf(stderr, "Uso: exec [VAR ...] programa [args...]\n");
        return;
    }

    char *path = Ejecutable(tr[env_count], searchList);
    if (path == NULL) {
        fprintf(stderr, "No se encontró el ejecutable %s\n", tr[env_count]);
        return;
    }

    if (env_count > 0) execve(path, &tr[env_count], new_env);
    else execv(path, &tr[env_count]);
    perror("exec");
}

void Cmd_execpri(char *args[]) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "Uso: execpri prioridad comando [args...]\n");
        return;
    }

    char *endptr;
    long prioridad = strtol(args[1], &endptr, 10);
    if (*endptr != '\0' || prioridad < -20 || prioridad > 19) {
        fprintf(stderr, "La prioridad debe estar entre -20 y 19.\n");
        return;
    }

    if (setpriority(PRIO_PROCESS, 0, (int)prioridad) == -1) {
        perror("Error al establecer la prioridad");
        return;
    }
    execvp(args[2], &args[2]);
    perror("exec");
}

void Cmd_fg(char *tr[], char *envp[], SearchList *searchList) {
    pid_t pid;
    char *new_env[MAXENV];
    int env_count = 0;

    while (tr[env_count] != NULL && env_count < MAXENV - 1) {
        int pos = BuscarVariable(tr[env_count], envp);
        if (pos < 0) break;
        new_env[env_count] = envp[pos];
        env_count++;
    }
    new_env[env_count] = NULL;

    if (tr[env_count] == NULL) {
        fprintf(stderr, "Error: falta el ejecutable.\n");
        return;
    }
    char *path = Ejecutable(tr[env_count], searchList);
    if (path == NULL) {
        fprintf(stderr, "No se encontró el ejecutable %s\n", tr[env_count]);
        return;
    }

    if ((pid = fork()) == 0) {
        if (env_count > 0) execve(path, &tr[env_count], new_env);
        else execv(path, &tr[env_count]);
        perror("exec");
        _exit(EXIT_FAILURE);
    } else if (pid > 0) {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return;
        }
        if (WIFEXITED(status)) {
            printf("Process %d exited with status %d\n", pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Process %d killed by signal %d\n", pid, WTERMSIG(status));
        }
    } else {
        perror("fork");
    }
}

void Cmd_fgpri(char *tr[], char *envp[], SearchList *searchList) {
    pid_t pid;
    char *endptr;

    if (tr[0] == NULL || tr[1] == NULL) {
        fprintf(stderr, "Uso: fgpri prio prog args...\n");
        return;
    }

    long prio = strtol(tr[0], &endptr, 10);
    if (*endptr != '\0' || prio < -20 || prio > 19) {
        fprintf(stderr, "Prioridad no válida: %s\n", tr[0]);
        return;
    }

    char *path = Ejecutable(tr[1], searchList);
    if (path == NULL) {
        fprintf(stderr, "No se encontró el ejecutable %s\n", tr[1]);
        return;
    }

    if ((pid = fork()) == 0) {
        if (setpriority(PRIO_PROCESS, 0, (int)prio) == -1) {
            perror("Error al establecer la prioridad");
            _exit(EXIT_FAILURE);
        }

        if (execve(path, &tr[1], envp) == -1) {
            perror("Error al ejecutar el programa");
            _exit(EXIT_FAILURE);
        }
    } else if (pid > 0) {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return;
        }
        if (WIFEXITED(status)) {
            printf("El proceso %d terminó con el estado %d\n", pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("El proceso %d fue terminado por la señal %d\n", pid, WTERMSIG(status));
        }
    } else {
        perror("Error en fork");
    }
}


void listjobs(BackgroundProcessList *bgList) {
    refreshBackgroundProcesses(bgList);
    if (bgList->head == NULL) {
        printf("No hay procesos en segundo plano.\n");
        return;
    }

    printf("Lista de procesos en segundo plano:\n");
    printBackgroundProcessList(bgList);
}


void deljobs(BackgroundProcessList *bgList, const char *option) {
    refreshBackgroundProcesses(bgList);
    if (bgList->head == NULL) {
        printf("No hay procesos en segundo plano para eliminar.\n");
        return;
    }

    if (strcmp(option, "-term") == 0) {
        removeBackgroundProcessesByStatus(bgList, "TERMINATED");
    } else if (strcmp(option, "-sig") == 0) {
        removeBackgroundProcessesByStatus(bgList, "SIGNALED");
    } else {
        printf("Comando no reconocido para 'deljobs'. Usa '-term' o '-sig'.\n");
    }
}


char *joinArgs(char *tr[]) {
    size_t total_length = 0;
    for (int i = 0; tr[i] != NULL; i++) {
        total_length += strlen(tr[i]) + 1; // +1 para el espacio o terminador nulo
    }

    char *result = malloc(total_length + 1); // Reservar memoria
    if (!result) {
        perror("Error al reservar memoria para el comando");
        return NULL;
    }

    result[0] = '\0'; // Inicializar la cadena vacía

    for (int i = 0; tr[i] != NULL; i++) {
        strcat(result, tr[i]);
        if (tr[i + 1] != NULL) strcat(result, " "); // Agregar espacio entre argumentos
    }

    return result;
}


void Cmd_back(char *tr[], char *envp[], BackgroundProcessList *list, SearchList *searchList) {
    pid_t pid;
    char *exec_path = Ejecutable(tr[0], searchList);

    if (exec_path == NULL) {
        fprintf(stderr, "Error: no se encontró el programa %s\n", tr[0]);
        return;
    }

    if ((pid = fork()) == -1) {
        perror("Error al hacer fork");
        return;
    }

    if (pid == 0) {
        if (execve(exec_path, tr, envp) == -1) {
            perror("Error al ejecutar el programa");
            _exit(EXIT_FAILURE);
        }
    } else {
        char *command_line = joinArgs(tr);
        printf("Proceso en segundo plano con PID: %d\n", pid);
        if (command_line != NULL) {
            addBackgroundProcess(list, pid, command_line);
            free(command_line);
        }
    }
}

void Cmd_backpri(int prio, char *tr[], char *envp[], BackgroundProcessList *list, SearchList *searchList) {
    pid_t pid;
    char *exec_path = Ejecutable(tr[0], searchList);

    if (exec_path == NULL) {
        fprintf(stderr, "Error: no se encontró el programa %s\n", tr[0]);
        return;
    }

    if ((pid = fork()) == -1) {
        perror("Error al hacer fork");
        return;
    }

    if (pid == 0) {
        // Proceso hijo: Cambiar prioridad y ejecutar
        if (setpriority(PRIO_PROCESS, 0, prio) == -1) {
            perror("Error al establecer la prioridad");
            _exit(EXIT_FAILURE);
        }

        if (execve(exec_path, tr, envp) == -1) {
            perror("Error al ejecutar el programa");
            _exit(EXIT_FAILURE);
        }
    } else {
        // Proceso padre
        char *command_line = joinArgs(tr); // Manejo dinámico del comando
        printf("Proceso en segundo plano con PID: %d y prioridad %d\n", pid, prio);
        if (command_line != NULL) {
            addBackgroundProcess(list, pid, command_line);
            free(command_line);
        }
    }
}
