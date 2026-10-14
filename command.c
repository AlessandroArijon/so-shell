/*Alumnos:
/ Miguel Alessandro Arijon Alvarez
/ Saul Raposo Raposo
*/

#include "command.h"

#ifndef S_ISVTX
#define S_ISVTX 01000
#endif

char LetraTF (mode_t m)
{
    switch (m&S_IFMT) {
        case S_IFSOCK: return 's';
        case S_IFLNK: return 'l';
        case S_IFREG: return '-';
        case S_IFBLK: return 'b';
        case S_IFDIR: return 'd';
        case S_IFCHR: return 'c';
        case S_IFIFO: return 'p';
        default: return '?';
    }
}

char * ConvierteModo (mode_t m, char *permisos)
{
    strcpy (permisos,"---------- ");

    permisos[0]=LetraTF(m);
    if (m&S_IRUSR) permisos[1]='r';
    if (m&S_IWUSR) permisos[2]='w';
    if (m&S_IXUSR) permisos[3]='x';
    if (m&S_IRGRP) permisos[4]='r';
    if (m&S_IWGRP) permisos[5]='w';
    if (m&S_IXGRP) permisos[6]='x';
    if (m&S_IROTH) permisos[7]='r';
    if (m&S_IWOTH) permisos[8]='w';
    if (m&S_IXOTH) permisos[9]='x';
    if (m&S_ISUID) permisos[3]='s';
    if (m&S_ISGID) permisos[6]='s';
    if (m&S_ISVTX) permisos[9]='t';

    return permisos;
}

int makefile(char *trozos[]){
    if (trozos[1] == NULL){
        fprintf(stderr, "Uso: makefile <nombre_del_fichero>\n");
        return -1;
    }

    FILE *file = fopen(trozos[1], "w");
    if (file == NULL){
        perror("Error al crear el fichero");
        return -1;
    }

    fclose(file);
    printf("Fichero '%s' creado.\n", trozos[1]);
    return 0;
}



int makedir(char *nombreDir){
    if (nombreDir == NULL){
        fprintf(stderr, "Uso: makedir <nombre_del_directorio>\n");
        return -1;
    }

    if (mkdir(nombreDir, S_IRWXU) == -1){
        perror("Error al crear el directorio");
        return -1;
    }

    printf("Directorio '%s' creado.\n", nombreDir);
    return 0;
}



void cwd(void){
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL){
        printf("%s\n", cwd);
    }
    else{
        printf("Error al obtener el directorio actual: %s\n", strerror(errno));
    }
}

void listfile(char *filename, int long_format, int show_link, int show_acc_time) {
    struct stat fileStat;
    char filepath[PATH_MAX];
    char permisos[12];
    char cwda[PATH_MAX];
    
    if (filename == NULL){
        cwd();
        return;
    }
    if (getcwd(cwda, sizeof(cwda)) == NULL){
        printf("Error al obtener directorio actual: %s\n", strerror(errno));
        return;
    }

    int written;
    if (filename[0] == '/') written = snprintf(filepath, sizeof(filepath), "%s", filename);
    else written = snprintf(filepath, sizeof(filepath), "%s/%s", cwda, filename);
    if (written < 0 || (size_t)written >= sizeof(filepath)) {
        fprintf(stderr, "Error: Ruta truncada (demasiado larga).\n");
        return;
    }

    if (lstat(filepath, &fileStat) == -1){
        printf("Error al obtener informacion del archivo: %s\n", strerror(errno));
        return;
    }

    if (long_format) {
        ConvierteModo(fileStat.st_mode, permisos);
        printf("%s ", permisos);
        if (show_acc_time) printf("%s", ctime(&fileStat.st_atime));
        else printf("%s", ctime(&fileStat.st_mtime));
    }

    printf("%lld\t%s", (long long)fileStat.st_size, filename);

    if (show_link && S_ISLNK(fileStat.st_mode)) {
        char link_target[512];
        ssize_t len = readlink(filepath, link_target, sizeof(link_target) - 1);
        if (len != -1) {
            link_target[len] = '\0';
            printf(" -> %s", link_target);
        }
    }
    printf("\n");
}


void listdir(char *dir_name, int show_hidden, int long_format, int show_link, int show_acc_time){
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;
    char filepath[PATH_MAX];
    char permisos[12];

    if (dir_name == NULL){
        cwd();
        return;
    }

    if ((dir = opendir(dir_name)) == NULL){
        printf("No se puede abrir el directorio : %s\n", strerror(errno));;
        return;
    }
    printf("********%s\n", dir_name);
    while ((entry = readdir(dir)) != NULL){
        if (!show_hidden && entry->d_name[0] == '.');
        else{
            int written = snprintf(filepath, sizeof(filepath), "%s/%s", dir_name, entry->d_name);
            if (written < 0 || (size_t)written >= sizeof(filepath)) {
                fprintf(stderr, "Ruta demasiado larga: %s/%s\n", dir_name, entry->d_name);
                continue;
            }

            if (lstat(filepath, &fileStat) == -1){
                printf("Error al obtener informacion del directorio: %s\n", strerror(errno));;
                continue;
            }
            else{
                if (long_format){
                    ConvierteModo(fileStat.st_mode, permisos);
                    printf("%s ", permisos);
                    printf("%5lld ", (long long)fileStat.st_size);
                    if (show_acc_time){
                        printf("%s", ctime(&fileStat.st_atime));
                    }
                    else{
                        printf("%s", ctime(&fileStat.st_mtime));
                    }
                }

                printf("%s", entry->d_name);

                if (show_link && S_ISLNK(fileStat.st_mode)){
                    char link_target[512];
                    ssize_t len = readlink(filepath, link_target, sizeof(link_target) - 1);
                    if (len != -1){
                        link_target[len] = '\0';
                        printf(" -> %s", link_target);
                    }
                }
                printf("\n");
            }
        }
    }
    closedir(dir);
}

void reclist(const char *dir_name, int show_hidden, int depth) {
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;
    char filepath[PATH_MAX];

    if (dir_name == NULL){
        cwd();
        return;
    }
    if (dir_name == NULL){
        dir_name = ".";
    }

    if ((dir = opendir(dir_name)) == NULL){
        printf("No se pudo abrir el directorio : %s\n", strerror(errno));;
        return;
    }

    printf("************%s\n", dir_name);  

    while ((entry = readdir(dir)) != NULL){
        int is_hidden = !show_hidden && entry->d_name[0] == '.';

        if (!is_hidden){
            int written = snprintf(filepath, sizeof(filepath), "%s/%s", dir_name, entry->d_name);
            if (written < 0 || (size_t)written >= sizeof(filepath)) continue;
            
            if (lstat(filepath, &fileStat) == -1) {
                printf("Error al obtener informacion del archivo: %s\n", strerror(errno));;
                closedir(dir);
                return;
            }

            printf("%8lld  %s\n", (long long)fileStat.st_size, entry->d_name);

            if (S_ISDIR(fileStat.st_mode) && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                reclist(filepath, show_hidden, depth + 1);
            }
        }
    }
    closedir(dir);
}

void revlist(char *dir_name, int show_hidden, int long_format, int show_link, int show_acc_time) {
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;
    char filepath[PATH_MAX];

    if (dir_name == NULL){
        cwd();
        return;
    }

    if ((dir = opendir(dir_name)) == NULL){
        printf("No se pudo abrir el directorio: %s\n", strerror(errno));
        return;
    }

    printf("************%s\n", dir_name);

    while ((entry = readdir(dir)) != NULL){
        if (show_hidden || entry->d_name[0] != '.') {
            int written = snprintf(filepath, sizeof(filepath), "%s/%s", dir_name, entry->d_name);
            if (written < 0 || (size_t)written >= sizeof(filepath)) continue;
            if (lstat(filepath, &fileStat) != -1) {
                if (S_ISDIR(fileStat.st_mode) && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) revlist(filepath, show_hidden, long_format, show_link, show_acc_time);
            } 
            else   printf("Error al obtener informacion del archivo: %s\n", strerror(errno));
        }
    }

    rewinddir(dir);

    while ((entry = readdir(dir)) != NULL){
        if (show_hidden || entry->d_name[0] != '.') {
            int written = snprintf(filepath, sizeof(filepath), "%s/%s", dir_name, entry->d_name);
            if (written < 0 || (size_t)written >= sizeof(filepath)) continue;

            if (lstat(filepath, &fileStat) != -1) {
                if (!S_ISDIR(fileStat.st_mode)) printf("%8lld  %s\n", (long long)fileStat.st_size, entry->d_name);
            }
            else    printf("Error al obtener informacion del archivo: %s\n", strerror(errno));
        }
    }

    closedir(dir);
}



void auxList(int numPalabras,char* trozos[], int which) {
    int show_hidden = 0, long_format = 0, show_link = 0, show_acc_time = 0;
    int paths = 0;
    for (int i = 1; i < numPalabras; i++) {
        if (strcmp(trozos[i], "-hid") == 0) {
            show_hidden = 1;
        }
        else if (strcmp(trozos[i], "-long") == 0) {
            long_format = 1;
        }
        else if (strcmp(trozos[i], "-link") == 0) {
            show_link = 1;
        }
        else if (strcmp(trozos[i], "-acc") == 0) {
            show_acc_time = 1;
        }
        else paths++;
    }

    if (paths == 0) {
        cwd();
        return;
    }
    for (int i = 1; i < numPalabras; i++) {
        if (trozos[i][0] == '-' &&
            (strcmp(trozos[i], "-hid") == 0 || strcmp(trozos[i], "-long") == 0 ||
             strcmp(trozos[i], "-link") == 0 || strcmp(trozos[i], "-acc") == 0)) continue;
        if(which == 1) listfile(trozos[i], long_format, show_link, show_acc_time);
        else if(which == 2) listdir(trozos[i], show_hidden, long_format, show_link, show_acc_time);
        else if(which == 3) reclist(trozos[i], show_hidden, 0);
        else if(which == 4) revlist(trozos[i], show_hidden, long_format, show_link, show_acc_time);
    }
}

void erase(char *tr[]) {
    char cwda[PATH_MAX];
    char full_path[PATH_MAX];
    
    if (tr[1] == NULL) {
        cwd();
        return;
    }

    if (getcwd(cwda, sizeof(cwda)) == NULL) {
        printf("Error obteniendo el directorio de trabajo actual: %s\n", strerror(errno));
        return;
    }

    for (int i = 1; tr[i] != NULL; i++) {
        int written;
        if (tr[i][0] == '/') written = snprintf(full_path, sizeof(full_path), "%s", tr[i]);
        else written = snprintf(full_path, sizeof(full_path), "%s/%s", cwda, tr[i]);
        if (written < 0 || (size_t)written >= sizeof(full_path)) {
            fprintf(stderr, "Error: Ruta truncada (demasiado larga).\n");
            continue;
        }
        if (remove(full_path) == 0) printf("Elemento eliminado: %s\n", full_path);
        else printf("Error eliminando %s: %s\n", full_path, strerror(errno));
    }
}

void delrec(const char *path) {
    struct stat path_stat;
    struct dirent *entry;
    DIR *dir;
    
    if (lstat(path, &path_stat) != 0) {
        printf("Error al acceder al path: %s\n", strerror(errno));
        return;
    }

    if (S_ISDIR(path_stat.st_mode)) {
        dir = opendir(path);
        if (!dir) {
            printf("Error al abrir el directorio: %s\n", strerror(errno));
            return;
        }

        while ((entry = readdir(dir)) != NULL) {
            char new_path[1024];
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                int written = snprintf(new_path, sizeof(new_path), "%s/%s", path, entry->d_name);
                if (written < 0 || (size_t)written >= sizeof(new_path)) {
                    fprintf(stderr, "Ruta demasiado larga: %s/%s\n", path, entry->d_name);
                    continue;
                }
                delrec(new_path);
            }
        }
        closedir(dir);

        if (rmdir(path) != 0) printf("Error al eliminar el directorio: %s\n", strerror(errno));
        else printf("Directorio eliminado: %s\n", path);
    } 
    else {
        if (unlink(path) != 0) printf("Error al eliminar el larchivo: %s\n", strerror(errno));
        else printf("Archivo eliminado: %s\n", path);
    }
}
