//pipe comunication

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/wait.h>
#include<unistd.h>


int main (void){

    //fd[0] - read 
    //fd[1] - write
    int fd[2]; //file descriptor, è una sorta di chiave per accedere alla pipe
    if (pipe(fd) == -1){  //aprire una pipe. se restituisce valore 0 vuol dire che ha avuto successo, -1 altrimenti
        printf("An error occurred with opening the pipe\n");
        return 1;
    }

    int id = fork();
    if (id == -1){
        printf("An error occurred with fork\n");
        return 4;
    }

    if (id == 0){  //chiamata al processo figlio
        close(fd[0]);
        int x;
        printf("input a number: ");
        scanf("%d", &x);
        if (write(fd[1], &x, sizeof(int)) == -1){
            printf("An error occurred with writing to the pipe\n");
            return 2;
        }
        close(fd[1]);
    }
    else {  //chiamata al processo padre
        close(fd[1]);
        int y;
        if (read(fd[0], &y, sizeof(int)) == -1){
            printf("An error occurred with reading from the pipe\n");
            return 3;
        }
        y = y * 3;
        close(fd[0]);
        printf("Got from child process %d\n", y);
    }

    return 0;
}