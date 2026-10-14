/*Alumnos:
/ Miguel Alessandro Arijon Alvarez
/ Saul Raposo Raposo
*/

#include "command2.h"

#define TAMANO 2048


void Recursiva (int n)
{
    char automatico[TAMANO];
    static char estatico[TAMANO];

    printf ("parametro:%3d(%p) array %p, arr estatico %p\n", n, (void *)&n,
            (void *)automatico, (void *)estatico);

    if (n>0)
        Recursiva(n-1);
}


void LlenarMemoria (void *p, size_t cont, unsigned char byte)
{
    unsigned char *arr=(unsigned char *) p;
    size_t i;

    for (i=0; i<cont;i++)
        arr[i]=byte;
}

int is_valid_hex_format(const char *addr) {
    if (addr[0] == '0' && addr[1] == 'x') {
        for (int i = 2; addr[i] != '\0'; i++) {
            if (!isxdigit(addr[i])) {
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

int is_valid_range(const void *p, size_t count, listMemblocks l) {
    uintptr_t ptr = (uintptr_t)p;

    MemNode *current = l;
    while (current != NULL) {
        uintptr_t start = (uintptr_t)current->address;
        if (ptr >= start && ptr - start <= current->size &&
            count <= current->size - (ptr - start)) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

void memfill(char *tr[], listMemblocks l){
    if (tr[1] == NULL){
        printf("Error: No se especificó una dirección de memoria.\n");
        return;
    }

    size_t n;
    char c;
    long val;
    char *endptr;

    if (tr[2] == NULL){
        n = 128;
        c = 'A';
    } 
    else if (tr[3] == NULL){
        c = 'A';
        errno = 0;
        val = strtol(tr[2], &endptr, 10);
        if (errno != 0 || *endptr != '\0' || val < 0){
            printf("Error: El tamaño de memoria no es un número válido.\n");
            return;
        }
        n = (size_t) val;
    } 
    else{
        errno = 0;
        val = strtol(tr[2], &endptr, 10);
        if (errno != 0 || *endptr != '\0' || val < 0){
            printf("Error: El tamaño de memoria no es un número válido.\n");
            return;
        }
        n = (size_t) val;

        if (strlen(tr[3]) == 1){
            c = tr[3][0];
        } 
        else if (strlen(tr[3]) > 2 && tr[3][0] == '0' && tr[3][1] == 'x'){
            val = strtol(tr[3], &endptr, 16);
            if (*endptr != '\0' || val < 0 || val > 255){
                printf("Error: El valor hexadecimal no es válido o está fuera de rango (0x00 a 0xFF).\n");
                return;
            }
            c = (char) val;
        } 
        else{
            printf("Error: El parámetro del byte no es un carácter único ni un valor hexadecimal válido.\n");
            return;
        }
    }

    errno = 0;
    unsigned long long int address = strtoull(tr[1], &endptr, 0);
    if (errno != 0 || *endptr != '\0'){
        printf("Error: Dirección de memoria no válida (no es hexadecimal).\n");
        return;
    }

    void *p = (void *) address;

    if (!is_valid_range(p, n, l)){
        printf("Error: La dirección de memoria no pertenece a un bloque válido o excede los límites asignados.\n");
        return;
    }

    printf("Llenando %zu bytes de memoria con el byte '%c' (0x%02X) a partir de la dirección %p\n", n, c, (unsigned char)c, p);

    LlenarMemoria(p, n, (unsigned char)c);
}




void memdump(void *p, size_t cont) {
    if (p == NULL) {
        printf("Error: Dirección de memoria nula.\n");
        return;
    }

    unsigned char *ptr = (unsigned char*)p;

    printf("Volcando %zu bytes desde la direccion %p\n", cont, p);

    for (size_t i = 0; i < cont; i++) {
        printf(" %c ", isprint(ptr[i]) ? ptr[i] : '.');
    }
    printf("\n");

    for (size_t i = 0; i < cont; i++) {
        printf("%02x ", ptr[i]);
    }
    printf("\n");
}



void memdump_aux(char *tr[], listMemblocks l) {
    if (tr[1] == NULL) {
        printf("Error: No se ha especificado una dirección de memoria.\n");
        return;
    }

    unsigned long long int address;
    size_t n;
    long val;
    char *endptr;

    if (tr[2] == NULL) {
        n = 25;
    } else {
        errno = 0;
        val = strtol(tr[2], &endptr, 10);
        if (errno != 0 || *endptr != '\0' || val < 0) {
            printf("El tamaño de memoria no es un número válido\n");
            return;
        }
        n = (size_t) val;
    }

    errno = 0;
    address = strtoull(tr[1], &endptr, 0);
    if (errno != 0 || *endptr != '\0') {
        printf("Error: Dirección de memoria no válida (no es hexadecimal).\n");
        return;
    }

    if (address == 0) {
        printf("Error: La dirección proporcionada es 0, lo que no es válido.\n");
        return;
    }

    if (!is_valid_range((void *)(uintptr_t)address, n, l)) {
        printf("Error: La dirección de memoria no es válida o no es accesible dentro de los bloques de memoria.\n");
        return;
    }

    void *p = (void *) address;
    
    memdump(p, n);
}


 
int global_inic1 = 1, global_inic2 = 2, global_inic3 = 3;
int global_no_inic1, global_no_inic2, global_no_inic3;

void memory(char *tr[], listMemblocks l){
    int is_all = (tr[1] == NULL || strcmp("-all", tr[1]) == 0);

    if (is_all || strcmp("-vars", tr[1]) == 0) {
        int local1, local2, local3;

        static int stat_inic1 = 1, stat_inic2 = 2, stat_inic3 = 3;
        static int stat_no_inic1, stat_no_inic2, stat_no_inic3;

        printf("Variables locales: %p, %p, %p\n", (void *)&local1, (void *)&local2, (void *)&local3);
        printf("Variables globales: %p, %p, %p\n", (void *)&global_inic1, (void *)&global_inic2, (void *)&global_inic3);
        printf("Var (N.I.)globales: %p, %p, %p\n", (void *)&global_no_inic1, (void *)&global_no_inic2, (void *)&global_no_inic3);
        printf("Variables estáticas: %p, %p, %p\n", (void *)&stat_inic1, (void *)&stat_inic2, (void *)&stat_inic3);
        printf("Var (N.I.)estáticas: %p, %p, %p\n", (void *)&stat_no_inic1, (void *)&stat_no_inic2, (void *)&stat_no_inic3);

        if (!is_all) return;
    }

    if (is_all || strcmp("-funcs", tr[1]) == 0) {
        union { int (*function)(char **); void *address; } prog1 = { .function = makefile };
        union { int (*function)(char *); void *address; } prog2 = { .function = makedir };
        union {
            void (*function)(char *, int, int, int);
            void *address;
        } prog3 = { .function = listfile };

        void *handle = dlopen(NULL, RTLD_LAZY);
        if (!handle) {
            fprintf(stderr, "Error al cargar la biblioteca: %s\n", dlerror());
            return;
        }

        void *libFunc1 = dlsym(handle, "printf");
        void *libFunc2 = dlsym(handle, "malloc");
        void *libFunc3 = dlsym(handle, "free");

        printf("Funciones programa: %p, %p, %p\n", prog1.address, prog2.address, prog3.address);
        printf("Funciones libreria: %p, %p, %p\n", libFunc1, libFunc2, libFunc3);

        dlclose(handle);
        if (!is_all) return;
    }

    if (is_all || strcmp("-blocks", tr[1]) == 0) {
        pid_t pid = getpid();
        MemNode *current = l;
        printf("****** Lista de bloques asignados para el proceso %d ******\n", pid);
        while (current != NULL) {
            printType(*current);
            current = current->next;
        }
        if (!is_all) return;
    }

    if (is_all || strcmp("-vmmap", tr[1]) == 0 || strcmp("-pmap", tr[1]) == 0) {
        pid_t pid = getpid();
        char pid_str[16];
        snprintf(pid_str, sizeof(pid_str), "%d", pid);

        printf("****** Mapa de memoria para el proceso %d ******\n", pid);

        (void)pid_str;
        Do_pmap();
        
        if (!is_all) return;
    }

    if (!is_all) {
        printf("Opción no reconocida: %s\n", tr[1]);
    }
}




void * ObtenerMemoriaShmget (key_t clave, size_t tam, listMemblocks *l)
{
    void * p;
    int aux,id,flags=0777;
    struct shmid_ds s;

    if (tam)
        flags=flags | IPC_CREAT | IPC_EXCL; 
    if (clave==IPC_PRIVATE)
    {errno=EINVAL; return NULL;}
    if ((id=shmget(clave, tam, flags))==-1)
        return (NULL);
    if ((p=shmat(id,NULL,0))==(void*) -1){
        aux=errno;
        if (tam)
            shmctl(id,IPC_RMID,NULL);
        errno=aux;
        return (NULL);
    }
    shmctl (id,IPC_STAT,&s);
    if (AnadirMemblock(SHARED, clave, s.shm_segsz, p, s.shm_ctime, l) == -1) {
        aux = errno;
        shmdt(p);
        if (tam) shmctl(id, IPC_RMID, NULL);
        errno = aux != 0 ? aux : ENOMEM;
        return NULL;
    }
    return (p);
}
void do_AllocateCreateshared (char *tr[], listMemblocks *l)
{
    key_t cl;
    size_t tam;
    void *p;

    if (tr[2]==NULL || tr[3]==NULL) {
        imprimirListaTipo(*l, SHARED);
        return;
    }

    char *endptr;
    errno = 0;
    unsigned long key_value = strtoul(tr[2], &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        fprintf(stderr, "Clave no válida\n");
        return;
    }
    cl = (key_t)key_value;
    errno = 0;
    unsigned long long size_value = strtoull(tr[3], &endptr, 10);
    if (errno != 0 || *endptr != '\0' || size_value == 0 || size_value > SIZE_MAX) {
        printf ("No se asignan bloques de 0 bytes\n");
        return;
    }
    tam = (size_t)size_value;
    if ((p=ObtenerMemoriaShmget(cl,tam, l))==NULL){
        fprintf(stderr, "Imposible asignar memoria compartida clave %d:%s\n",(int) cl, strerror(errno));
    }
    else printf("Asignados %zu bytes en %p\n", tam,p);
}

void do_AllocateShared (char *tr[], listMemblocks *l)
{
    key_t cl;
    void *p;

    if (tr[2]==NULL) {
        imprimirListaTipo(*l, SHARED);
        return;
    }

    char *endptr;
    errno = 0;
    unsigned long key_value = strtoul(tr[2], &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        fprintf(stderr, "Clave no válida\n");
        return;
    }
    cl = (key_t)key_value;

    if ((p=ObtenerMemoriaShmget(cl,0, l))==NULL){
        fprintf(stderr, "Imposible asignar memoria compartida clave %lu:%s\n",(unsigned long) cl, strerror(errno));
    }
    else printf("Memoria compartida de clave %d en %p\n",(int)cl ,p);
}


void * MapearFichero (char * fichero, int protection, listMemblocks *l, listaFicheros *fiche)
{
    int df, map=MAP_PRIVATE,modo=O_RDONLY;
    struct stat s;
    void *p;

    if (protection&PROT_WRITE)
        modo=O_RDWR;
    if (stat(fichero,&s)==-1 || s.st_size <= 0 || (df=open(fichero, modo))==-1)
        return NULL;
    if ((p=mmap (NULL,(size_t)s.st_size, protection,map,df,0))==MAP_FAILED) {
        close(df);
        return NULL;
    }

    if (AnadirAFicherosAbiertos(df, fichero, fiche) == -1 ||
        AnadirMemblockMmap(MMAP, fichero, (size_t)s.st_size, p, time(NULL), df, l) == -1) {
        EliminarFicherosAbiertos(fiche, df);
        munmap(p, (size_t)s.st_size);
        close(df);
        errno = ENOMEM;
        return NULL;
    }

    return p;
}

void do_AllocateMmap(char *arg[], listMemblocks *l, listaFicheros *fiche) {
    char *perm;
    void *p;
    int protection = 0;
    struct stat f;
    if (arg[2] == NULL) {
        imprimirListaTipo(*l, MMAP);
        return;
    }
        if ((perm = arg[3]) != NULL && strlen(perm) < 4) {
            if (strchr(perm, 'r') != NULL) protection |= PROT_READ;
            if (strchr(perm, 'w') != NULL) protection |= PROT_WRITE;
            if (strchr(perm, 'x') != NULL) protection |= PROT_EXEC;
        }
        else {
            fprintf(stderr, "Imposible procesar los permisos. Use una combinación de r, w y x.\n");
            return;
        }
        if(stat(arg[2], &f) == -1) {
            fprintf(stderr, "No se pudo acceder a los datos del archivo: %s\n", strerror(errno));
            return;
        }
        if ((p = MapearFichero(arg[2], protection, l, fiche)) == NULL)
            fprintf(stderr, "Imposible mapear fichero: %s\n",strerror(errno));
        else
            printf("Fichero %s mapeado en %p\n", arg[2], p);
}

void *cadtop(const char *str){
    return (void *)strtoul(str, NULL, 0);
}

ssize_t LeerFichero(char *f, void *p, size_t cont)
{
    struct stat s;
    ssize_t n;
    int df, aux;

    if (stat(f, &s) == -1){
        perror("stat fallo");
        return -1;
    }

    if ((df = open(f, O_RDONLY)) == -1){
        perror("open fallo");
        return -1;
    }

    if (cont == (size_t)-1)
        cont = (size_t)s.st_size;

    if ((n = read(df, p, cont)) == -1){
        aux = errno;
        close(df);
        errno = aux;
        return -1;
    }

    close(df);
    return n;
}

void Cmd_ReadFile(char *ar[], listMemblocks l) {
    void *p;
    size_t cont = (size_t)-1;
    ssize_t n;

    if (ar[0] == NULL || ar[1] == NULL) {
        printf("Faltan parámetros\n");
        return;
    }

    FILE *file = fopen(ar[1], "r");
    if (file == NULL) {
        printf("El archivo '%s' no existe o no es accesible: %s\n", ar[1], strerror(errno));
        return;
    }
    fclose(file);

    if (ar[2] == NULL) {
        printf("Falta el parámetro de dirección\n");
        return;
    }

    char *endptr;
    errno = 0;
    p = (void *)(uintptr_t)strtoull(ar[2], &endptr, 0);
    if (errno != 0 || *endptr != '\0') {
        fprintf(stderr, "Dirección no válida\n");
        return;
    }

    if (ar[3] != NULL) {
        errno = 0;
        cont = (size_t)strtoull(ar[3], &endptr, 10);
        if (errno != 0 || *endptr != '\0') {
            fprintf(stderr, "Tamaño no válido\n");
            return;
        }
    } else {
        struct stat st;
        if (stat(ar[1], &st) == -1 || st.st_size < 0) {
            perror("stat");
            return;
        }
        cont = (size_t)st.st_size;
    }

    if (!is_valid_range(p, cont, l)) {
        fprintf(stderr, "La dirección y el tamaño exceden un bloque asignado\n");
        return;
    }

    if ((n = LeerFichero(ar[1], p, cont)) == -1) {
        perror("Imposible leer fichero");
    } else {
        printf("Leídos %ld bytes de %s en %p\n", (long)n, ar[1], p);
    }
}



void Do_pmap (void)
{ pid_t pid;
    char elpid[32];
    char *argv[4]={"pmap",elpid,NULL};

    snprintf(elpid, sizeof(elpid), "%d", (int)getpid());
    if ((pid=fork())==-1){
        perror ("Imposible crear proceso");
        return;
    }
    if (pid==0){
        if (execvp(argv[0],argv)==-1)
            perror("cannot execute pmap (linux, solaris)");

        argv[0]="procstat"; argv[1]="vm"; argv[2]=elpid; argv[3]=NULL;
        if (execvp(argv[0],argv)==-1)
            perror("cannot execute procstat (FreeBSD)");

        argv[0]="procmap",argv[1]=elpid;argv[2]=NULL;
        if (execvp(argv[0],argv)==-1)  
            perror("cannot execute procmap (OpenBSD)");

        argv[0]="vmmap"; argv[1]="-interleave"; argv[2]=elpid;argv[3]=NULL;
        if (execvp(argv[0],argv)==-1)
            perror("cannot execute vmmap (Mac-OS)");
        _exit(1);
    }
    if (waitpid(pid, NULL, 0) == -1) perror("waitpid");
}

void writefile(char *file, void *addr, size_t cont, int overwrite, listMemblocks l){
    if (file == NULL || addr == NULL || cont == 0){
        fprintf(stderr, "Error: Parámetros inválidos.\n");
        return;
    }

    if (!is_valid_range(addr, cont, l)) {
        fprintf(stderr, "La dirección y el tamaño exceden un bloque asignado.\n");
        return;
    }

    if (!overwrite && access(file, F_OK) != -1) {
        fprintf(stderr, "Imposible escribir fichero: File exists\n");
        return;
    }

    int flags = O_WRONLY | O_CREAT | (overwrite ? O_TRUNC : O_EXCL);
    int fd = open(file, flags, 0644);
    if (fd == -1){
        perror("Error al abrir el archivo");
        return;
    }

    ssize_t bytes_written = write(fd, addr, cont);
    if (bytes_written == -1){
        perror("Error al escribir en el archivo");
        close(fd);
        return;
    }

    if ((size_t)bytes_written != cont) {
        fprintf(stderr, "Advertencia: Solo se escribieron %ld bytes de %zu.\n", bytes_written, cont);
    } else {
        printf("Se escribieron %zu bytes desde la dirección %p al archivo %s\n", cont, addr, file);
    }

    close(fd);
}


void read_descriptor(int df, void *addr, size_t cont, listMemblocks l) {
    if (addr == NULL) {
        fprintf(stderr, "Error: Dirección de memoria inválida.\n");
        return;
    }

    if (cont == 0) {
        fprintf(stderr, "Error: Tamaño a leer no puede ser 0.\n");
        return;
    }

    if (df < 0) {
        fprintf(stderr, "Error: Descriptor de archivo inválido.\n");
        return;
    }

    if (!is_valid_range(addr, cont, l)) {
        fprintf(stderr, "La dirección y el tamaño exceden un bloque asignado.\n");
        return;
    }

    ssize_t bytes_read = read(df, addr, cont);
    if (bytes_read == -1) {
        perror("Error al leer desde el descriptor de archivo");
        return;
    }

    if ((size_t)bytes_read < cont) {
        fprintf(stderr, "Advertencia: Solo se leyeron %zd bytes de %zu.\n", bytes_read, cont);
    } else {
        printf("Se leyeron %zu bytes desde el descriptor de archivo %d hacia la dirección %p\n", cont, df, addr);
    }
}


void write_descriptor(int df, void *addr, size_t cont, listMemblocks l){
    if (addr == NULL || cont == 0){
        fprintf(stderr, "Error: Dirección de memoria o tamaño inválido.\n");
        return;
    }

    if (df < 0){
        fprintf(stderr, "Error: Descriptor de archivo inválido.\n");
        return;
    }


    if (!is_valid_range(addr, cont, l)) {
        fprintf(stderr, "La dirección y el tamaño exceden un bloque asignado.\n");
        return;
    }

    ssize_t bytes_written = write(df, addr, cont);
    if (bytes_written == -1){
        perror("Error al escribir desde el descriptor de archivo");
        return;
    }

    if ((size_t)bytes_written != cont) fprintf(stderr, "Advertencia: Solo se escribieron %ld bytes de %zu.\n", bytes_written, cont);
    else printf("Se escribieron %zu bytes desde la dirección %p al descriptor de archivo %d\n", cont, addr, df); 
}

void recursive_function(int n){
    char local_array[2048];
    static char static_array[2048];
    printf("parametro: %d(%p) array %p, arr estatico %p\n", n, (void *)&n, (void *)local_array, (void *)static_array);

    if (n <= 0) return;

    recursive_function(n - 1);
}

void recurse_command(char *tr[]) {
    if (tr[1] == NULL) {
        fprintf(stderr, "Uso: recurse n\n");
        return;
    }

    char *endptr;
    errno = 0;
    long n = strtol(tr[1], &endptr, 10);

    if (errno != 0 || *endptr != '\0' || n < 0 || n > INT_MAX) {
        fprintf(stderr, "Error: '%s' no es un número entero válido o está fuera de rango.\n", tr[1]);
        return;
    }

    recursive_function((int)n);
}


void allocate_malloc(char* tr[], listMemblocks *l) {
    if(tr[2] == NULL) {
        imprimirListaTipo(*l, MALLOC);
        return;
    }
    unsigned long long val;
    char *endptr;
    size_t n;
    errno = 0;
    val = strtoull(tr[2], &endptr, 10);
    if (errno != 0 || *endptr != '\0' || val == 0 || val > SIZE_MAX) {
        printf("El tamaño de memoria no es un numero valido\n");
        return;
    }
    n = (size_t) val;
    void *ptr = malloc(n);
    if (ptr == NULL) {
        perror("Error al asignar memoria con malloc");
        return;
    }
    if (AnadirMemblock(MALLOC, 0, n, ptr, time(NULL), l) == -1) {
        free(ptr);
        return;
    }
    printf("Asignados %zu bytes en %p\n",n,ptr);
}


void allocate(char *tr[], listMemblocks *l, listaFicheros *fiche) {

    if(tr[1] == NULL) {
        tr[1] = "-blocks";
        memory(tr, *l);
        return;
    }
    if (strcmp("-malloc", tr[1]) == 0) {
        allocate_malloc(tr, l);
        return;
    }
    if (strcmp("-createshared", tr[1]) == 0) {
        do_AllocateCreateshared(tr, l);
        return;
    }
    if (strcmp("-shared", tr[1]) == 0) {
        do_AllocateShared(tr, l);
        return;
    }
    if (strcmp("-mmap", tr[1]) == 0) {
        do_AllocateMmap(tr, l, fiche);
        return;
    }
    printf("uso: allocate [-malloc|-shared|-createshared|-mmap] ....\n");
    return;

}


static void unlink_memnode(listMemblocks *l, MemNode *current, MemNode *prev) {
    if (prev != NULL) prev->next = current->next;
    else *l = current->next;
    free(current->filename);
    free(current);
}

void deallocate_malloc(char* n, listMemblocks *l){
    if (n == NULL){
        imprimirListaTipo(*l, MALLOC);
        return;
    }
    
    MemNode *current = *l;
    MemNode *prev = NULL;
    long val;
    size_t tam;
    char *endptr;
    
    errno = 0;
    val = strtol(n, &endptr, 10);
    if (errno != 0 || *endptr != '\0' || val <= 0){
        printf("El tamaño de memoria no es un número válido.\n");
        return;
    }
    
    tam = (size_t)val;
    
    while (current != NULL && (current->type != MALLOC || current->size != tam)){
        prev = current;
        current = current->next;
    }

    if (current == NULL){
        printf("No hay bloque de ese tamaño asignado con malloc.\n");
        return;
    }

    printf("Bloque %p de tamaño %zu eliminado\n", current->address, tam);
    free(current->address);
    unlink_memnode(l, current, prev);
}

void deallocate_shared(char *cl_char, listMemblocks *l) {
    if(cl_char == NULL) {
        imprimirListaTipo(*l, SHARED);
        return;
    }
    MemNode *current = *l;
    MemNode *prev = NULL;
    long val;
    key_t cl;
    char *endptr;
    errno = 0;
    val = strtol(cl_char, &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        printf("El tamaño de memoria no es un numero valido.\n");
        return;
    }
    cl = (key_t) val;
    while (current != NULL && (current->type != SHARED || current->key != cl)){
        prev = current;
        current = current->next;
    }
    if(current == NULL) {
        fprintf(stderr, "Imposible encontrar un bloque con clave %d :%s\n", cl, strerror(errno));
        return;
    }
    if (shmdt(current->address) == -1) {
        fprintf(stderr, "Error al desvincular la memoria compartida con clave %d: %s\n", cl, strerror(errno));
        return;
    }
    printf("Bloque %p con clave %d desasignado\n", current->address, cl);
    unlink_memnode(l, current, prev);

}

void deallocate_delkey(char *cl_char, listMemblocks *l) {
    if(cl_char == NULL) {
        imprimirListaTipo(*l, SHARED);
        return;
    }
    (void)l;
    long val;
    key_t cl;
    char *endptr;
    errno = 0;
    val = strtol(cl_char, &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        printf("El tamaño de memoria no es un numero valido.\n");
        return;
    }
    cl = (key_t) val;
    int shm_id = shmget(cl, 0, 0666);
    if (shm_id == -1) {
        fprintf(stderr, "Error al obtener el ID de memoria compartida con clave %d: %s\n", cl, strerror(errno));
        return;
    }

    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        fprintf(stderr, "Error al eliminar la clave de memoria compartida con clave %d: %s\n", cl, strerror(errno));
        return;
    }
    printf("Clave de memoria compartida %d eliminada\n", cl);

}

void deallocate_addr(char* addr, listMemblocks *l, listaFicheros *ficheros) {
    MemNode *current = *l;
    MemNode *prev = NULL;
    char *endptr;
    errno = 0;
    void *address = (void *)(uintptr_t)strtoull(addr, &endptr, 0);

    if (errno != 0 || *endptr != '\0') {
        fprintf(stderr, "Dirección no válida: %s\n", addr);
        return;
    }

    while (current != NULL && current->address != address){
        prev = current;
        current = current->next;
    }

    if(current == NULL) {
        fprintf(stderr, "Imposible encontrar un bloque con direccion %p\n", address);
        return;
    }
    if(current->type == SHARED) {
        if (shmdt(current->address) == -1) {
            fprintf(stderr, "Error al desvincular la memoria compartida con la direccion %p: %s\n", current->address, strerror(errno));
            return;
        }
    }
    else if(current->type == MALLOC) {
        free(current->address);
    }
    else if (current->type == MMAP) {
        if (munmap(current->address, current->size) == -1) {
            fprintf(stderr, "Error al liberar el bloque de memoria %p: %s\n", current->address, strerror(errno));
            return;
        }
        if (close(current->df) == -1) perror("close");
        EliminarFicherosAbiertos(ficheros, current->df);
    }
    printf("Bloque con direccion %p eliminado\n", address);
    unlink_memnode(l, current, prev);

}

void deallocate_mmap(char *filename, listMemblocks *l, listaFicheros *ficheros) {
    if (filename == NULL) {
        imprimirListaTipo(*l, MMAP);
        return;
    }

    MemNode *current = *l;
    MemNode *prev = NULL;

    while (current != NULL &&
           (current->type != MMAP || current->filename == NULL || strcmp(current->filename, filename) != 0)) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        fprintf(stderr, "Error: bloque de memoria con fichero %s no encontrado.\n", filename);
        return;
    }

    if (munmap(current->address, current->size) == -1) {
        fprintf(stderr, "Error al liberar el bloque de memoria %p: %s\n", current->address, strerror(errno));
        return;
    }

    if (close(current->df) == -1) perror("close");
    EliminarFicherosAbiertos(ficheros, current->df);
    printf("Bloque con direccion %p eliminado\n", current->address);
    unlink_memnode(l, current, prev);
}


void deallocate(char *tr[], listMemblocks *l, listaFicheros *ficheros) {
    if (tr[1] == NULL || strcmp(tr[1], "-blocks") == 0) {
        memory(tr, *l);
        return;
    }

    if (strcmp("-malloc", tr[1]) == 0) {
        deallocate_malloc(tr[2], l);
        return;
    }
    if (strcmp("-shared", tr[1]) == 0) {
        deallocate_shared(tr[2], l);
        return;
    }

    if (strcmp("-delkey", tr[1]) == 0) {
        deallocate_delkey(tr[2], l);
        return;
    } 

    if (strcmp("-mmap", tr[1]) == 0) {
        deallocate_mmap(tr[2], l, ficheros);
        return;
    }

    deallocate_addr(tr[1], l, ficheros);
}
