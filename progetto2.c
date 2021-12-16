//Autori --> Dennis Turco, Adrian Tunea
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<math.h>
#include<time.h>
#include<sys/wait.h>
#include<unistd.h>

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

    //apertura pipe
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++)
            if (pipe(fd[i][j]) < 0) return EXIT_FAILURE; //se e' < 0 allora e' un errore
    }

    //creazione processi figli
    int pid[n][n];
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            pid[i][j] = fork();
            if(pid[i][j] < 0) return EXIT_FAILURE;
        }
    }

    //operazioni dei processi figli
    for(int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            if(pid[i][j] == 0){
                
                //vari close
                for(int z = 0; z<n; z++){
                    for(int y = 0; y < n; z++){
                        if(z != i || y != j){
                            close(fd[z][y][READ_END]);
                            close(fd[z][y][WRITE_END]);
                        }
                    }
                }
                close(fd[0][0][WRITE_END]);
                close(fd[i][j][READ_END]);

                //operazioni read e write
                if(read(fd[0][0][READ_END], &x, sizeof(int)) < 0){
                    close(fd[0][0][READ_END]);
                    return 1;
                } 
                if(write(fd[i][j][WRITE_END], &x, sizeof(int)) < 0){
                    close(fd[i][j][WRITE_END]);
                    return 1;
                } 

                //vari close
                close(fd[0][0][READ_END]);
                close(fd[i][j][WRITE_END]);
                return 0;
            }
        }
    }

    //processo genitore
    int cont = 0;
    int indice = 0;
    for(int b = 0; b < n; b++){
        for(int c = 0; c < n; c++){
            for(int i = 0; i < n; i++){
                for(int j = 0; j < n; j++){
                    if(i == j){
                        
                        //chiusure servono qui???

                        indice = j+c+b;
                        while(indice >= n){
                            indice = indice - n;   
                        }
                        C[c][j] = C[c][j] + A[c][indice] * B[indice][cont];
                        printf("posizione A(%d ,%d) B(%d ,%d) --> %d * %d = ", c+1, indice+1, indice+1, cont+1, A[c][indice], B[indice][cont]);
                        printf("%d \n", C[c][j]);
                        if(write(fd[c][j][WRITE_END], &C[c][j], sizeof(int)) < 0) return EXIT_FAILURE;
                        cont++;
                    }
                }
            }
            printf("\n");
            cont = 0;
        }
        printf("----------------------------------\n");
    }
    
    //solo per vedere se funziona:
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            if(write(fd[i][j][READ_END], &C[i][j], sizeof(int)) < 0) return EXIT_FAILURE;
            printf("%d ", x);
        }
    }

    //varie close
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            close(fd[i][j][WRITE_END]);
            close(fd[i][j][READ_END]);
        }
    }

    //chiusura pipe
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            waitpid(pid[i][j], NULL, 0);
        }
    }
    
    return EXIT_SUCCESS;
}



int main(void){
    int n = 0;
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
    float time_spent = (float)(end - begin)/CLOCKS_PER_SEC;
    printf("\nTempo Impiegato: %f ms", time_spent);

    printf("\n----------------------------------------------------\n");

    //chiusura files
    fclose(f1);
    fclose(f2);

    return EXIT_SUCCESS;
}
