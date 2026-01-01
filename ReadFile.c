#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gererMem.h"

#define MAX_LENGTH 200

typedef struct {
    char **mots;
    int *occurrences; 
    int nbrMot;
} histogramme;

void usage(char * fichier) {
    printf("SYNOPSIS:\n");
    printf("  %s [fichier] [-rwnapl] [...]\n\n", fichier);
    printf("DESCRIPTION:\n");
    printf("  Affiche le nombre et les mots dans les fichiers fournis.\n");
    printf("  Si aucun fichier n'est fourni, lit depuis l'entree standard.\n\n");
    printf("OPTIONS:\n");
    printf("  -r    affiche le resultat\n");
    printf("  -w    ecris le resultat dans un fichier\n");
    printf("  -n    choix du nombre de mots a afficher\n");
    printf("  -a    choix de l'algorithme\n");
    printf("  -p    affiche les performances de l'algorithme\n");
    printf("  -l    ecris les performances dans un fichier\n");
}

void OpenStrTab(histogramme h){
    int taille = h->nbrMot;
    for (int i = 0; i < taille; i++) {
        printf("%s\n", h->mots[i]);
    }
}

int InTab(histogramme h){
    char **tab = h->mots;
    char *Current = h->mots[0];
    for (int i = 0; i < 50; i++) {
        if (Current){return 1;}
    }
    return 0;
}

int FileReader(FILE * fichier, histogramme *h) {

    int mots = 0, inWord = 0;
    int c;
    char *buffer = (char *)myMalloc(sizeof( char )*MAX_LENGTH);
    while (!(feof(fichier))) {
        if (c == ' ' || c == '\n' || c == '\t') {
            if (inWord) {mots++; inWord = 0;}
        } else {inWord = 1;}
    } if (inWord) {mots++;}
    return mots;
}

int main(int argc, const char * argv[]) {
    int afficherMots = 0;
    
    if (argc > 1 && argv[1][0] == '-') {
        const char * options = argv[1] + 1;
        
        int optionValide = 1;
        for (int i = 0; options[i] != '\0'; i++) {
            if (options[i] != 'm') {
                optionValide = 0; break;
            }
        }
        if (!optionValide) {
            fprintf(stderr, "Erreur: option invalide '%s'\n\n", argv[1]);
            usage((char *)argv[0]); return 1;
        }
        for (int i = 0; options[i] != '\0'; i++) {
            if (options[i] == 'm') {afficherMots = 1;}
        }
    }

    if (argc < 2) {
        int mots = FileReader(stdin);
        if (afficherMots) printf("%d ", mots);
        printf("\n");
    } else {
        FILE * f = fopen(argv[2], "r");
        if (f == NULL) {
            fprintf(stderr, "Erreur: impossible d'ouvrir le fichier '%s'\n", argv[2]);
        }
        int mots = FileReader(f);
        fclose(f);
        printf("Dans le fichier '%s':\n", argv[2]);
        if (afficherMots) {printf("Il y a %d mots dans le fichier %s.\n", mots, argv[2]);}
    }
    
    return 0;
}
