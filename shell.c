#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/resource.h>//esto es necesario para usar RUSAGE

#define MAX_INPUT 1024
#define MAX_ARG 64

//variables para el resumen
int total_procesos = 0;

// Función para imprimir el resumen antes de salir
void mostrar_resumen() {
    struct rusage usage;

    printf("\nResumen de Sesion de la Shell\n");
    printf("Procesos ejecutados (programas externos): %d\n", total_procesos);

    // Obtener uso de memoria de la shell y sus hijos
    //getrusage nos ayuda a obtener los datos de los procesos
    if (getrusage(RUSAGE_CHILDREN, &usage) == 0) {
        printf("Memoria maxima residente de procesos hijos: %ld KB\n", usage.ru_maxrss);//aca entrega los datos de los procesos hijos(CHILDREN) con el tamaño maximo
    }
    if (getrusage(RUSAGE_SELF, &usage) == 0) {//RUSAGE_SELF obtiene lo mismo que el RUSAGE_CHILDREN pero para los hilos(la suma de estos)
        printf("Memoria maxima residente del proceso Shell: %ld KB\n", usage.ru_maxrss);
    }
}

//funcion para mostrar memoria
void mostrar_memoria(){
    FILE *archivo = fopen("/proc/meminfo", "r");
    char linea[128];
    long memoria = -1;

    if(archivo == NULL){
        perror("No se pudo abrir /proc/meminfo");
        return;
    }

    //buscar memoria
    while(fgets(linea, sizeof(linea), archivo) != NULL){
        if(sscanf(linea, "MemAvailable: %ld kB", &memoria) == 1){
            break;
        }
    }

    fclose(archivo);

    if(memoria != -1){
        printf("Memoria disponible: %ld kB\n", memoria);
    }else{
        printf("No se pudo obtener la memoria\n");
    }
}

//funcion para los commando incorporados
int BuiltIn(char **args){

    if (args[0] == NULL) {
        return 1; // comando vacio
    }

    //comando para salir
    if(strcmp(args[0], "exit") == 0){
        printf("Exiting shell...\n");
        mostrar_resumen();
        exit(0);
    }

    //cambio de directorio
    if(strcmp(args[0], "cd") == 0){
        if(args[1] != NULL && args[2] != NULL){
            printf("Muchos argumentos\n");
            return 1;
        }

        char *dir = args[1];

        if(dir == NULL){
            dir = getenv("HOME");
        }

        if(dir == NULL){
            printf("No se encontro HOME\n");
        }else{
            if(chdir(dir) != 0){
                perror("cd");
            }
        }
        return 1;
    }

    //comando help
    if (strcmp(args[0], "help") == 0) {
        printf("\nSimple Shell Help\n");
        printf("comando propios:\n");
        printf("  cd <dir>\n  exit\n  help\n");
        printf("Soportados: ls | grep txt\n");
        return 1;
    }
    return 0;

}

int main(void){
    while(1){
        //String que almacenará los input del teclado
        char buf[MAX_INPUT];

        //Prompt
        printf("minish > ");

        //Agarra inputs del teclado e impide que se rompa si uno hace ctrl+D
        if (fgets(buf, MAX_INPUT, stdin) == NULL) {
            printf("\nExiting shell...\n");
            mostrar_resumen();
            break; // Rompe el bucle y permite que main retorne 0 de forma limpia
        }

        //Quitamos la linea extra q se crea con \n
        char *nl = strchr(buf, '\n');
        if (nl){
            //Cambiamos la linea extra de \n por \0 (null)
            *nl = '\0';
        }

        //Dividimos buf en diferentes argumentos
        char *args[MAX_ARG] = {NULL};
        int nargs = 0;

        args[nargs] = strtok(buf, " ");
        while(args[nargs] != NULL && nargs < MAX_ARG - 1){
            args[++nargs] = strtok(NULL, " ");
        }

        if(args[nargs] != NULL){
            printf("Demasiados argumentos\n");
            continue;
        }

        //comprobacion de comando incorporado
        if (BuiltIn(args)) {
            continue;
        }

        //Fork & exec
        pid_t pid = fork();

        if(pid > 0){
            //el proceso padre incrementa el contador y espera
            total_procesos++;
            //Es el proceso padre
            if(waitpid(pid, NULL, 0) != -1){
                mostrar_memoria();
            }else{
                perror("waitpid");
            }
        }else if(pid == 0){
            //Es el proceso hijo
            if(execvp(args[0], args) < 0){
                //Si exec da error
                fprintf(stderr, "exec no pudo ejecutar %s\n", args[0]);
            }
            exit(1);
        }else{
            //Si fork da error
            perror("fork");
        }
    }
}