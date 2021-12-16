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

void calcolaMatriceThread(int n, int A[n][n], int B[n][n], int C[n][n]){

    return;
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
    calcolaMatriceThread(n, matriceA, matriceB, matriceC);
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
