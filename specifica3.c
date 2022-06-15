//Autore --> Dennis Turco
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

pthread_mutex_t mutex;
int cont = 0;

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

void *ThreadProcess(void *arg){
    int n = *(int*)arg;
    char nomefile1[] = "matriceA.txt";
    char nomefile2[] = "matriceB.txt";
    char nomefile3[] = "matriceC.txt";
    FILE *f1 = fopen(nomefile1, "r"); //non c'e' bisogno di eseguire alcun controllo (gia' fatto nel main)
    FILE *f2 = fopen(nomefile2, "r");
    FILE *f3;
    int A[n][n];
    int B[n][n];
    int risultato = 0;
    inizializzaMatrice(f1, n, A);
    inizializzaMatrice(f2, n, B);
    fclose(f1);
    fclose(f2);
    
    //algoritmo per evitare la race condition:  nessun thread potra' leggere mentre uno potra' eseguire
    //operazioni dentro a mutex
    pthread_mutex_lock(&mutex); /*lock chiuso*/
    f3 = fopen(nomefile3, "a");
    
    //operazioni
    for(int i=0; i<n; i++){
        risultato = risultato + A[cont][i] * B[i][cont];
    }
    fprintf(f3, "%d ", risultato);
    
    cont++;
    if(cont == n) cont = 0;

    fclose(f3);
    pthread_mutex_unlock(&mutex); /*lock aperto*/
}

int calcolaMatriceThread(int n){
    
    pthread_mutex_init(&mutex, NULL); //inizializzazione della mutex
    
    //creazione matrice nxn processi
    pthread_t Thread[n][n];
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            if(pthread_create(&Thread[i][j], NULL, ThreadProcess, &n) < 0) return EXIT_FAILURE;
        }
    }

    //thread join
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            if(pthread_join(Thread[i][j], NULL) < 0) return EXIT_FAILURE;
        }
    }

    pthread_mutex_destroy(&mutex); //distrugge l'oggetto mutex a cui fa riferimento

    return EXIT_SUCCESS;
}

int main(void){
    int n = 0;
    int val;
    char nomefile1[] = "matriceA.txt";
    char nomefile2[] = "matriceB.txt";
    char nomefile3[] = "matriceC.txt";
    FILE *f1 = fopen(nomefile1, "r");
    FILE *f2 = fopen(nomefile2, "r");
    FILE *f3 = fopen(nomefile3, "r");
    FILE *f4 = fopen(nomefile3, "w"); //serve a sovrascrivere il file tutte le volte che viene avviato il programma

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

    inizializzaMatrice(f1, n, matriceA);
    inizializzaMatrice(f2, n, matriceB);
    calcolaMatriceThread(n);
    inizializzaMatrice(f3, n, matriceC);
    stampa(n, matriceA, matriceB, matriceC);

    clock_t end = clock(); /*fine ciclo di clock*/
    float time_spent = (float)(end - begin)/(CLOCKS_PER_SEC / 1000);
    printf("\nTempo Impiegato: %f ms", time_spent);

    printf("\n----------------------------------------------------\n");

    //chiusura files
    fclose(f1);
    fclose(f2);
    fclose(f3);
    fclose(f4);
    
    return EXIT_SUCCESS;
}
