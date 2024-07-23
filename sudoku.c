#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "lib/sudoku.h"

#define NUM_THREADS 8



typedef struct {
    int **sudoku;
    int row;
    int col;
    int value;
} ThreadArgs;

int placeVide(int **grid, int *row, int *col) {
    for (*row = 0; *row < 9; (*row)++) {
        for (*col = 0; *col < 9; (*col)++) {
            if (grid[*row][*col] == 0) {
                return 1; // Case vide trouvée
            }
        }
    }
    return 0; // Aucune case vide trouvée
}

int valide(int **grid, int row, int col, int num) {
    // Vérification de la ligne
    for (int i = 0; i < 9; i++) {
        if (grid[row][i] == num) {
            return 0; // Valeur déjà présente dans la ligne
        }
    }

    // Vérification de la colonne
    for (int i = 0; i < 9; i++) {
        if (grid[i][col] == num) {
            return 0; // Valeur déjà présente dans la colonne
        }
    }

    // Vérification de la sous-grille 3x3
    int debutLigne = row - row % 3;
    int debutCol = col - col % 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (grid[i + debutLigne][j + debutCol] == num) {
                return 0; // Valeur déjà présente dans la sous-grille
            }
        }
    }

    return 1; 
}

int corre_sudoku(int **grid) {
    int row, col;

    // Trouver une case vide dans la grille
    if (!placeVide(grid, &row, &col))
        return 1; // Si aucune case vide n'est trouvée, la grille est résolue

    // Essayer de placer les chiffres de 1 à 9 dans la case vide
    for (int num = 1; num <= 9; num++) {
        // Vérifier si le chiffre peut être placé dans cette case
        if (valide(grid, row, col, num)) {
            // Placer le chiffre dans la case
            grid[row][col] = num;

            // Résoudre récursivement le reste de la grille
            if (corre_sudoku(grid))
                return 1; // Si la grille est résolue, retourner 1

            // Si la grille ne peut pas être résolue avec ce chiffre, annuler le placement
            grid[row][col] = 0;
        }
    }

    // Aucun chiffre ne peut être placé dans cette case, retourner 0
    return 0;
}

void genere_sudoku(int **sudoku) {
    corre_sudoku(sudoku);
}

void *resoudre(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    int **sudoku = args->sudoku;
    int row = args->row;
    int col = args->col;
    int valeur_originale = args->value;
    int * retour = (int *)malloc(sizeof(int));
    *retour = 0;


    for (int value = 1; value <= 9; value++) {

        if (value == valeur_originale) {
            continue;
        }

        if (valide(sudoku, row, col, value)) {
            pthread_exit((void *)retour); 
        }

    }


    *retour = 1;
    pthread_exit((void*)retour);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Il faut mettre un seul argument");
        return 1;
    }

    int **sudoku = sudoku_init();


    int N = atoi(argv[1]);
    if (N < 40 || N > 50) {
        printf("Pour jouer, N doit être compris entre 40 et 50 inclus.\n");
        // Génération d'une grille de sudoku remplie et résolue
    	genere_sudoku(sudoku);
    	printf("************************ GRILLE ************************\n");
    	sudoku_print(sudoku);   
    }else{
    
    

       srand(time(NULL));
    // Génération d'une grille de sudoku remplie et résolue
    	genere_sudoku(sudoku);
    	printf("************************ SOLUTION ************************\n");
    	sudoku_print(sudoku);   

    

    

    // Variables pour les threads
    pthread_t threads[NUM_THREADS];
    ThreadArgs args[NUM_THREADS];

    int restant = 81 - N;
    
    while (restant > 0) {
        int ligne_supprime, col_supprime;
 	do {
       
        ligne_supprime = rand() % 9;
        col_supprime = rand() % 9;
        
    } while (sudoku_get(sudoku, ligne_supprime, col_supprime) == 0);

  
        for (int i = 0; i < NUM_THREADS; i++) {
            args[i] = (ThreadArgs){sudoku, ligne_supprime, col_supprime, sudoku_get(sudoku, ligne_supprime, col_supprime)};
            pthread_create(&threads[i], NULL, resoudre, (void *)&args[i]);
            
            
        }
        

        int *retour;
        
        for (int i = 0; i < NUM_THREADS; i++) {
        	pthread_join(threads[i], (void **)&retour);
        	if (*retour) {
            		restant--; 
        		sudoku_set(sudoku, ligne_supprime, col_supprime, 0); 
           		 break; 
        	}
        	 free(retour);
        	
    }

    
    }
    

    // Affichage de la grille finale
    printf("****************** A VOUS DE JOUER ! :) ******************\n");
    sudoku_print(sudoku);

    // Libération de la mémoire
    sudoku_free(sudoku);



    return 0;
}
}

