#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>

#define READ_END 0
#define WRITE_END 1

int main(void){
    int fd[2][2]; //file descriptor con 2 processi e 2 estremita' (read e write)

    //apertura pipe
    for(int i = 0; i < 2; i++)
        if (pipe(fd[i]) < 0) return 1; //se e' < 0 allora e' un errore
    
    int pid1 = fork(); //generiamo processo figlio 1
    if(pid1 < 0) return 2; //caso in cui la fork non ha avuto successo

    //esecuzione processo figlio 1
    if(pid1 == 0){ //caso in cui la fork ha avuto successo e il processo figlio e' stato generato
        close(fd[0][WRITE_END]);
        close(fd[1][READ_END]);

        int x;

        if(read(fd[0][READ_END], &x, sizeof(int)) < 0) return 3;

        printf("Sono il processo figlio 1, quanto aggiungo al valore %d: ", x);
        //scanf("%d", &y);

        x = x + 5;

        if(write(fd[1][WRITE_END], &x, sizeof(int)) < 0) return 4; //scrivo il valore di x modificato

        //chiusura file descriptor
        close(fd[0][READ_END]);
        close(fd[1][WRITE_END]);
        return 0;
    }

    //processo genitore
    close(fd[0][0]);
    close(fd[1][0]);

    int x;
    printf("Inserire valore di partenza: ");
    scanf("%d", &x);
    printf("entarto0\n");
    if(write(fd[0][WRITE_END], &x, sizeof(int)) < 0); return 5;
    printf("entrato1\n");
    if(read(fd[1][READ_END], &x, sizeof(int)) < 0) return 6;
    printf("entrato2\n");

    printf("Risultato: %d\n", x);

    //chiusura file descriptor
    close(fd[0][WRITE_END]);
    close(fd[1][READ_END]);

    waitpid(pid1, NULL, 0);

    return 0;
}