//Autori --> Dennis Turco
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>

#define READ_END 0
#define WRITE_END 1

void stampaSingola(int i, int n, int X[n][n]){
    printf("  | ");
    for (int j=0; j<n; j++){
        printf("%d ", X[i][j]);
    }
    printf("|  ");
    return;
}

void stampa(int n, int A[n][n], int B[n][n], int C[n][n]){
    bool inserito = false;
    bool inserito2 = false;
    for (int i=0; i<n; i++){

        stampaSingola(i, n, A);
        
        if(i >= n/2 && inserito ==  false){
            printf("x");
            inserito = true;
        } else printf(" ");

        stampaSingola(i, n, B);
        
        if(i >= n/2 && inserito2 == false){
            printf("=");
            inserito2 = true;
        } else printf(" ");
        
        stampaSingola(i, n, C);
        printf("\n");
    }
    return;
}

void inizializzaMatrice(FILE *f, int n, int X[n][n]){
    int val;
    for (int i=0; i<n; i++){
        for (int j=0; j<n; j++){
            fscanf(f, "%d", &val);
            X[i][j] = val;
        }
    }
    return;    
}

int calcolaMatricePipe(int n, int A[n][n], int B[n][n], int C[n][n]){
    int fd[n][n][2]; //2 perchè 2 sono le estremita' della pipe (read-end, write-end)
    int x;
    int PosX;
    int PosY;

    //creazione pipe
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++)
            if (pipe(fd[i][j]) < 0) return EXIT_FAILURE; //se e' < 0 allora e' un errore
    }

    //creazione processi figli
    for(int cont1=0; cont1<n; cont1++){
        for(int cont2=0; cont2<n; cont2++){

            int pid = fork();

            if(pid < 0) return EXIT_FAILURE;

            //esecuzione Processi Figli
            if(pid == 0){
                close(fd[0][0][WRITE_END]);
                close(fd[PosX][PosY][READ_END]);

                //operazioni read e write
                if(read(fd[0][0][READ_END], &x, sizeof(int)) < 0){
                    close(fd[0][0][READ_END]);
                    return 1;
                }
                if(write(fd[PosX][PosY][WRITE_END], &x, sizeof(int)) < 0){
                    close(fd[PosX][PosY][WRITE_END]);
                    return 1;
                }
                
                //close
                close(fd[0][0][READ_END]);
                close(fd[PosX][PosY][WRITE_END]);
                
                return EXIT_SUCCESS;
            }

            //processo genitore
            else{
                int cont = 0;
                int indice = 0;
                for(int b = 0; b < n; b++){
                    for(int c = 0; c < n; c++){
                        for(int i = 0; i < n; i++){
                            for(int j = 0; j < n; j++){
                                if(i == j){
                                    indice = j+c+b;
                                    while(indice >= n){
                                        indice = indice - n;
                                    }
                                    C[c][j] = C[c][j] + A[c][indice] * B[indice][cont];
                                    PosX = c;
                                    PosY = j;
                                    if(write(fd[PosX][PosY][WRITE_END], &C[PosX][PosY], sizeof(int)) < 0) {
                                        close(fd[PosX][PosY][WRITE_END]);
                                        return EXIT_FAILURE;
                                    }

                                    waitpid(pid, NULL, 0);

                                    cont++;
                                }
                            }
                        }
                        cont = 0;
                    }
                }

                //varie close
                for(int i = 0; i < n; i++){
                    for(int j = 0; j < n; j++){
                        close(fd[i][j][WRITE_END]);
                        close(fd[i][j][READ_END]);
                    }
                }

                return EXIT_SUCCESS;

            }
        }
    }

    return EXIT_SUCCESS;
}

int main(void){
    int n = 0;  //indice della matrice
    int val;
    char nomefile1[] = "matriceA.txt";
    char nomefile2[] = "matriceB.txt";
    FILE *f1 = fopen(nomefile1, "r");
    FILE *f2 = fopen(nomefile2, "r");

    //controllo su possibili fallimenti di apertura dei files
    if(f1 == NULL) return EXIT_FAILURE;
    if(f2 == NULL) return EXIT_FAILURE;

    clock_t begin = clock(); /*inizio ciclo di clock*/

    //ottengo il numero di righe e colonne
    while (!feof(f1)){
        fscanf(f1, "%d", &val);
        n++;
    }
    n = sqrt(n);
    fclose(f1);
    f1 = fopen(nomefile1, "r");

    int matriceA[n][n];
    int matriceB[n][n];
    int matriceC[n][n];

    //inizializzo la matrice C a valori 0 per evitare valori indefiniti
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            matriceC[i][j] = 0;
        }
    }

    inizializzaMatrice(f1, n, matriceA);
    inizializzaMatrice(f2, n, matriceB);
    calcolaMatricePipe(n, matriceA, matriceB, matriceC);
    stampa(n, matriceA, matriceB, matriceC);

    clock_t end = clock(); /*fine ciclo di clock*/
    float time_spent = (float)(end - begin)/(CLOCKS_PER_SEC / 1000);
    printf("\nTempo Impiegato: %f ms", time_spent);

    printf("\n----------------------------------------------------\n");

    //chiusura files
    fclose(f1);
    fclose(f2);
    
    return EXIT_SUCCESS;
}
