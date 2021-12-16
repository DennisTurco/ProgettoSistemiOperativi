#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

const int BUFFER_SIZE = 25;
const int READ_END = 0;
const int WRITE_END = 1;

int main(void) {
    char write_msg[BUFFER_SIZE] = "Greetings";
    char read_msg[BUFFER_SIZE];
    int fd[2];
    pid_t pid;
    
    /* crea la pipe */
    if (pipe(fd) == -1) {
        fprintf(stderr, "Pipe failed");
        return 1;
    }
    /* crea tramite fork un processo figlio */
    pid = fork();
    if (pid < 0) { /* errore */
        fprintf(stderr, "Fork Failed");
        return 1;
    }
    if (pid > 0) { /* processo padre */
        /* chiude l’estremità inutilizzata della pipe */
        close(fd[READ_END]);
        /* scrive sulla pipe */
        write(fd[WRITE_END], write_msg, strlen(write_msg)+1);
        /* chiude l’estremità della pipe dedicata alla scrittura */
        close(fd[WRITE_END]);
        return 0;
    }
    else { /* processo figlio */
        /* chiude l’estremità inutilizzata della pipe */
        close(fd[WRITE_END]);
        /* legge dalla pipe */
        read(fd[READ_END], read_msg, BUFFER_SIZE);
        printf("read %s", read_msg);
        /* chiude l’estremità della pipe dedicata alla lettura */
        close(fd[READ_END]);
        return 0;
    }
    return 0;
}