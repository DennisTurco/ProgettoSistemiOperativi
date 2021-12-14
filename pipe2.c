#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>

/**
 * https://www.youtube.com/watch?v=NkfIUo_Qq4c&ab_channel=CodeVault
 * in questo esempio abbiamo:
 * 3 pipes, tra cui
 * 1 pipe padre e 2 figli
 * Siccome abbiamo 3 pipes necessitiamo di 6 files descriptor (2 per ogni pipe)
 */

int main(void){
    int fd[3][2]; //fd = file descriptor è una 3x2 perchè ogni pipe ha 2 estremità (read end e write end)
    
    for (int i = 0; i < 3; i++){
        if(pipe(fd[i]) < 0) //caso d'errore, se non si verifica l'errore si ha aperto la pipe
            return 1;
    }
    
    int pid1 = fork();

    if (pid1 < 0) 
        return 2;
    
    //esecuzione processo figlio 1
    if (pid1 == 0){
        //child process 1
        close(fd[0][1]);
        close(fd[1][0]);
        close(fd[2][0]);
        close(fd[2][1]);
        int x;
        if(read(fd[0][0], &x, sizeof(int)) < 0)
            return 3;
        x = x+5;
        if(write(fd[1][1], &x, sizeof(int)) < 0)
            return 4; 
        
        //ora bisogna chiudere tutti i file descriptor prima che il processo finisca la sua esecuzione
        close(fd[0][0]);
        close(fd[1][1]);
        
        return 0;
    }

    int pid2 = fork();
    if (pid2 < 0)
        return 5;
    
    //esecuzione processo figlio 2
    if (pid2 == 0){
        //child process 2
        close(fd[0][0]);
        close(fd[0][1]);
        close(fd[1][1]);
        close(fd[2][0]);
        int x;
        if(read(fd[1][0], &x, sizeof(int)) < 0)
            return 6;
        x = x+5;
        if(write(fd[2][1], &x, sizeof(int)) < 0)
            return 7; 
        
        //ora bisogna chiudere tutti i file descriptor prima che il processo finisca la sua esecuzione
        close(fd[1][0]);
        close(fd[2][1]);
        
        return 0;

    }

    //parent process
    close(fd[0][0]);
    close(fd[1][0]);
    close(fd[1][1]);
    close(fd[2][1]);
    
    int x = 0;
    if(write(fd[0][1], &x, sizeof(int)) < 0)
        return 8;
    if(read(fd[2][0], &x, sizeof(int)) < 0)
        return 9;
    printf("Result: %d\n", x);
    close(fd[0][1]);
    close(fd[2][0]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}