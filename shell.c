#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]){
    while(1){
        //String que almacenará los input del teclado
        char buf[1024];

        //Prompt
        printf("minish > ");

        //Agarra inputs del teclado
        fgets(buf, 1024, stdin);

        //Quitamos la linea extra q se crea con \n
        char *nl = strchr(buf, '\n');
        if (nl){
            //Cambiamos la linea extra de \n por \0 (null)
            *nl = '\0';
        }

        //Dividimos buf en diferentes argumentos
        char *args[20];
        int nargs = 0;

        args[nargs] = strtok(buf, " ");
        while (args[nargs] != NULL)
        {
            args[++nargs] = strtok(NULL, " ");
        }

        if(args[0] == NULL){
            continue;
        }

        if(strcmp(args[0], "exit") == 0){
            exit(0);
        }

        //Fork & exec
        pid_t pid = fork();

        if(pid > 0){
            //Es el proceso padre
            wait(NULL);
        }else{
            //Es el proceso hijo
            execvp(args[0], args);

            //Si exec da error
            fprintf(stderr, "exec no pudo ejecutar %s\n", buf);
        }
    }
}