//Autori --> Dennis Turco, Adrian Tunea
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<math.h>
#include<time.h>

void stampaSingola(int i, int n, int X[n][n]){
    printf("  | ");
    for (int j=0; j<n; j++){
        printf("%d ", X[i][j]);
    }
    printf("|  ");
    return;
}

void stampaTrattino(int n){
    printf(" ");
    printf(" ");
    for (int i=0; i<n; i++){
        printf("%c", 218);
        for(int j=0; j < n*2+1-2; j++){
            printf(" ");
        }
        printf("  %c", 191);

        for(int j = 0; j<5; j++){
            printf(" ");
        }
    }
    printf("\n");
}

void stampaTrattino2(int n){
    printf(" ");
    printf(" ");
    for (int i=0; i<n; i++){
        printf("%c", 192);
        for(int j=0; j < n*2+1-2; j++){
            printf(" ");
        }
        printf("  %c", 217);

        for(int j = 0; j<5; j++){
            printf(" ");
        }
    }
    printf("\n");
}

void stampa(int n, int A[n][n], int B[n][n], int C[n][n]){
    bool inserito = false;
    bool inserito2 = false;
    for (int i=0; i<n; i++){

        if (i == 0) stampaTrattino(n);

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
    stampaTrattino2(n);
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

void calcolaMatrice(int n, int A[n][n], int B[n][n], int C[n][n]){
    int cont = 0;
    for(int i=0; i<n; i++){
        while (cont < n){
            printf("posizione %d, %d --> ", i, cont);
            C[i][cont] = 0;
            for(int j=0; j<n; j++){
                C[i][cont] = C[i][cont] + A[i][j] * B[j][cont];
                printf("%d * %d + ", A[i][j],  B[j][cont]);
            }
            printf("tot= %d", C[i][cont]);
            cont++;
            printf("\n");
        }
        cont = 0;      
    }
}

int main(void){
    int n = 0;
    int val;
    char nomefile1[] = "matriceA.txt";
    char nomefile2[] = "matriceB.txt";
    FILE *f1 = fopen(nomefile1, "r");
    FILE *f2 = fopen(nomefile2, "r");

    //controllo su possibili fallimenti di apertura dei files
    if(f1 == NULL) return 1;
    if(f2 == NULL) return 1;

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

    inizializzaMatrice(f1, n, matriceA);
    inizializzaMatrice(f2, n, matriceB);
    calcolaMatrice(n, matriceA, matriceB, matriceC);
    stampa(n, matriceA, matriceB, matriceC);

    clock_t end = clock(); /*fine ciclo di clock*/
    float time_spent = (float)(end - begin);
    printf("\nTempo Impiegato: %f ms", time_spent);

    printf("\n----------------------------------------------------");

    //chiusura files
    fclose(f1);
    fclose(f2);

    return 0;
}