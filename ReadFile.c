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
    printf("  %s [fichier] [-rwnapl] [n]\n\n", fichier);
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

void InitHist(histogramme *h, InfoMem *i){
    h->mots = (char **)myMalloc(sizeof(char *) * MAX_LENGTH, i);
    h->occurrences = (int *)myMalloc(sizeof(int) * MAX_LENGTH, i);
    h->nbrMot = 0;
}

void InitInfoMem(InfoMem *i){
    i->cumul_alloc = 0;
    i->cumul_desalloc = 0;
    i->max_alloc = 0;
}

void ReadStrHist(histogramme h){
    int taille = h.nbrMot;
    for (int i = 0; i < taille; i++) {
        printf("%s\n", h.mots[i]);
    }
}

int InHist(histogramme h, char *mot){
    char **tab = h.mots;
    for (int i = 0; i < h.nbrMot; i++) {
        char *curr = tab[i];
        if (strcmp(curr, mot) == 0){return i;}
    }
    return -1;
}

int DivLine(char *line, int start, char *mot) {
    int i = start;
    while (line[i] && (line[i] == ' ' || line[i] == '\t' || line[i] == '\n')) {
        i++;
    }

    int j = 0;
    while (line[i] && line[i] != ' ' && line[i] != '\t' && line[i] != '\n') {
        mot[j++] = line[i++];
    }
    mot[j] = '\0';

    if (j>0){return i;}
    return -1;
}

void FileReader(FILE * fichier, histogramme *h, InfoMem *i) {
    size_t size = sizeof(char) * MAX_LENGTH;
    char *buffer = (char *)myMalloc(size, i);
    char mot[42];
    
    while (fgets(buffer, MAX_LENGTH, fichier) != NULL) {
        if (ferror(fichier)) {
            fprintf(stderr, "Reading error\n");
            break;
        }
        
        int pos = 0, taille = h->nbrMot;
        while ((pos = DivLine(buffer, pos, mot)) != -1) {
            int index = InHist(*h, mot);
            if (index != -1) {h->occurrences[index]++;} 
            else {
                h->mots[taille] = (char *)myMalloc(strlen(mot) + 1, i);
                strcpy(h->mots[taille], mot);
                h->occurrences[taille] = 1;
                taille++;
            }
        }
        h->nbrMot = taille;
    }

    myFree(buffer, i, size);
}

int main(int argc, const char * argv[]) {
    printf("Initialisation...\n");
    histogramme occ;
    InfoMem infomem;
    InitInfoMem(&infomem);
    InitHist(&occ, &infomem);

    printf("affichage de l'histogramme (normalement vide):\n");
    ReadStrHist(occ);

    printf("Le bordel du fgets:\n");
    if (argc < 2) {
        FileReader(stdin, &occ, &infomem);
        printf("\n");
    } else {
        FILE * f = fopen(argv[1], "r");
        if (f == NULL) {
            fprintf(stderr, "Erreur: impossible d'ouvrir le fichier '%s'\n", argv[2]);
        }
        FileReader(f, &occ, &infomem);
        fclose(f);
    }
    for(int i = 0; i < occ.nbrMot; i++){
        printf("%s : %d\n", occ.mots[i], occ.occurrences[i]);
    }
    
    return 0;
}
