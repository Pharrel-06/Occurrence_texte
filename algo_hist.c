#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gererMem.h"
#include "algo_hist.h"

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
        
        int pos = 0;
        while ((pos = DivLine(buffer, pos, mot)) != -1) {
            int index = InHist(*h, mot);
            if (index != -1) {
                h->occurrences[index]++;
            } else {
                h->mots[h->nbrMot] = (char *)myMalloc(strlen(mot) + 1, i);
                strcpy(h->mots[h->nbrMot], mot);
                h->occurrences[h->nbrMot] = 1;
                h->nbrMot++;
            }
        }
    }

    myFree(buffer, i, size);
}

void FreeHistogramme(histogramme *h, InfoMem *i) {
    for (int j = 0; j < h->nbrMot; j++) {
        myFree(h->mots[j], i, strlen(h->mots[j]) + 1);
    }
    myFree(h->mots, i, sizeof(char *) * MAX_LENGTH);
    myFree(h->occurrences, i, sizeof(int) * MAX_LENGTH);
    h->nbrMot = 0;
}

// Echange 2 mots et leurs occurence dans l'histogramme
void sawp(histogramme* h, int i, int j) {
    int tmp_occ = h->occurrences[i];
    char* tmp_mot = h->mots[i];
    h->occurrences[i] = h->occurrences[j];
    h->mots[i] = h->mots[j];
    h->occurrences[j] = tmp_occ;
    h->mots[j] = tmp_mot;
}

// Trouve et renvoie l'indice de l'occurrence la plus grande dans la partie non triée du tableau
int RechercheMax(histogramme* h, int debut) {
    int index_max = debut;
    for (int i = debut; i < h->nbrMot; i++) {
        if (h->occurrences[i] > h->occurrences[index_max]) {
            index_max = i;
        }
    }
    return index_max;
}

// Trie l'histogramme par ordre décroissant
void TrieHistogramme(histogramme* h) {
    int index_max;
    int current_index = 0;
    while (current_index < h->nbrMot) {
        index_max = RechercheMax(h, current_index);
        sawp(h, current_index, index_max);
        current_index++;
    }
}
