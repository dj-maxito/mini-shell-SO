#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_INPUT 1024
#define MAX_ARG 64

int main(int argc, char *argv[]){
    while(1){
        //String que almacenará los input del teclado
        char buf[MAX_INPUT];

        //Prompt
        printf("minish > ");

        //Agarra inputs del teclado
        fgets(buf, MAX_INPUT, stdin);

        //Quitamos la linea extra q se crea con \n
        char *nl = strchr(buf, '\n');
        if (nl){
            //Cambiamos la linea extra de \n por \0 (null)
            *nl = '\0';
        }

        //Dividimos buf en diferentes argumentos
        char *args[MAX_ARG]; //Punteros de los comandos
        int nargs = 0;

        args[nargs] = strtok(buf, " ");
        while (args[nargs] != NULL)
        {
            args[++nargs] = strtok(NULL, " ");
        }

        if(args[0] == NULL){
            continue;
        }
        
        if(strcmp(args[0], "cd") == 0){
            if(args[1] == NULL){
                printf("Expected argument after cd\n");
            }else{
                if(chdir(args[1]) != 0){
                    printf("cd failed\n");
                }
            }
        }

        if(strcmp(args[0], "exit") == 0){
            printf("Exiting shell...\n");
            exit(0);
        }

        //Fork & exec
        pid_t pid = fork();

        if(pid > 0){
            //Es el proceso padre
            wait(NULL);
        }else{
            //Es el proceso hijo
            if(execvp(args[0], args) < 0){

            //Si exec da error
            fprintf(stderr, "exec no pudo ejecutar %s\n", buf);
            }
            exit(1);
        }
    }
}