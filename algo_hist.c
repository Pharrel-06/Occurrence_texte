#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gererMem.h"
#include "algo_hist.h"

void InitHist(histogramme *h, InfoMem *i){
    h->mots = (char **)myMalloc(sizeof(char *) * MAX_LENGTH, i);
    h->occurrences = (int *)myMalloc(sizeof(int) * MAX_LENGTH, i);
    h->nbrMot = 0;
    h->taille_allouee = MAX_LENGTH;
}

void ReadStrHist(histogramme h){
    int taille = h.nbrMot;
    for (int i = 0; i < taille; i++) {
        printf("%s\n", h.mots[i]);
    }
}

void AfficherHistogramme(histogramme occ, int n) {
    printf("\n=== Résultats ===\n");
    for(int i = 0; i < n && i < occ.nbrMot ; i++){
        printf("%s : %d\n", occ.mots[i], occ.occurrences[i]);
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

int isSeparator(char c) {
    return (
        c == ' '  || c == '\t' || c == '\n' || 
        c == ','  || c == ';'  || c == '('  || 
        c == ')'  || c == '{'  || c == '}'  || 
        c == '['  || c == ']'  || c == ':'  || 
        c == '?'  || c == '!'  || c == '.'
    );
}

int DivLine(char *line, int start, char *mot) {
    int i = start;
    while (line[i] && isSeparator(line[i])){
        i++;
    }

    int j = 0;
    while (line[i] && !isSeparator(line[i])){
        if (j < 41)
            mot[j++] = line[i];
        i++;
    }
    mot[j] = '\0';

    if (j > 0){return i;}
    return -1;
}

void MaxSizePlus(histogramme *h, InfoMem *i){
    int nouvelle_taille = h->taille_allouee * 2;
    h->mots = (char **)myRealloc(h->mots, sizeof(char *) * nouvelle_taille, i, sizeof(char *) * h->taille_allouee);
    h->occurrences = (int *)myRealloc(h->occurrences, sizeof(int) * nouvelle_taille, i, sizeof(int) * h->taille_allouee);
    h->taille_allouee = nouvelle_taille;
}

int HistWordCount(histogramme h){
    int count = 0;
    for(int i = 0; i < h.nbrMot; i++){
        count += h.occurrences[i];
    }
    return count;
}

void FileReader(FILE * fichier, histogramme *h, InfoMem *i, Curse *curse) {
    size_t size = sizeof(char) * MAX_LENGTH;
    char *buffer = (char *)myMalloc(size, i);
    char mot[42];
    i->temps_debut = time(NULL);
    
    while (fgets(buffer, MAX_LENGTH, fichier) != NULL) {
        if (ferror(fichier)) {
            fprintf(stderr, "Reading error\n");
            break;
        }
        
        int pos = 0;
            while ((pos = DivLine(buffer, pos, mot)) != -1) {
                if (strlen(mot) == 0) {
                    continue;
                }

                if (curse && EstInterdit(curse, mot)) {
                    continue;
                }

                int index = InHist(*h, mot);
                if (index != -1) {
                    h->occurrences[index]++;
                } else {
                    if (h->nbrMot >= h->taille_allouee) {
                        MaxSizePlus(h, i);
                    }
                    h->mots[h->nbrMot] = (char *)myMalloc(strlen(mot) + 1, i);
                    strcpy(h->mots[h->nbrMot], mot);
                    h->occurrences[h->nbrMot] = 1;
                    h->nbrMot++;
                }
            }
    }
    myFree(buffer, i, size);
    i->temps_fin = time(NULL);
    i->cumul_temps += i->temps_fin - i->temps_debut;
    i->temps_debut = 0; i->temps_fin = 0;
}

void FreeHistogramme(histogramme *h, InfoMem *i) {
    if (h->mots != NULL) {
        for (int j = 0; j < h->nbrMot; j++) {
            if (h->mots[j] != NULL) {
                myFree(h->mots[j], i, strlen(h->mots[j]) + 1);
                h->mots[j] = NULL;
            }
        }
        myFree(h->mots, i, sizeof(char *) * h->taille_allouee);
        h->mots = NULL;
    }
    if (h->occurrences != NULL) {
        myFree(h->occurrences, i, sizeof(int) * h->taille_allouee);
        h->occurrences = NULL;
    }
    h->nbrMot = 0;
    h->taille_allouee = 0;
}

void TopNmot(histogramme *h, int n, InfoMem *inf) {
    inf->temps_debut = time(NULL);

    if (n <= 0 || n > h->nbrMot) {fprintf(stderr, "Paramètre n invalide\n"); return;}

    int *indices = myMalloc(sizeof(int) * n, inf);
    int size = 0;
    for (int i = 0; i < h->nbrMot; i++) {
        if (size == n &&
            h->occurrences[i] <= h->occurrences[indices[size - 1]]) {
            continue;
        }

        int pos = size;
        if (pos > n - 1) {pos = n - 1;}

        while (pos > 0 && h->occurrences[i] > h->occurrences[indices[pos - 1]]) {
            indices[pos] = indices[pos - 1]; pos--;
        }
        indices[pos] = i;
        if (size < n) {size++;}
    }

    printf("\n=== Résultats ====\n");
    printf("=== Top %d mots ===\n", size);

    for (int i = 0; i < size; i++) {
        printf("%s : %d\n", h->mots[indices[i]], h->occurrences[indices[i]]);
    }

    myFree(indices, inf, sizeof(int) * n);

    inf->temps_fin = time(NULL);
    inf->cumul_temps += inf->temps_fin - inf->temps_debut;
    inf->temps_debut = 0;
    inf->temps_fin = 0;
}


// Echange 2 mots et leurs occurence dans l'histogramme
void swap(histogramme* h, int i, int j) {
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
void TrieHistogramme(histogramme* h, InfoMem *i) {
    i->temps_debut = time(NULL);

    int index_max;
    int current_index = 0;
    while (current_index < h->nbrMot) {
        index_max = RechercheMax(h, current_index);
        swap(h, current_index, index_max);
        current_index++;
    }

    i->temps_fin = time(NULL);
    i->cumul_temps += i->temps_fin - i->temps_debut;
    i->temps_debut = 0; i->temps_fin = 0;
}

int Compte_mot_hist(histogramme h) {
    int nb_mot = 0;
    for(int i = 0; i < h.nbrMot; i++) {
        nb_mot += h.occurrences[i];
    }
    return nb_mot;
}

void Ecrit_resultats_hist(FILE* fichier, histogramme* h, int nb_mot, char* algo) {
    fprintf(fichier, "%s\n", algo);
    for (int i = 0; i < nb_mot; i++) {
        fprintf(fichier, "%s %d\n", h->mots[i], h->occurrences[i]);
    }
}

void Ecrit_perf_hist(FILE* fichier, InfoMem* infoMem, int nb_mot, char* algo) {
    fprintf(fichier, "%s\n", algo);
    fprintf(fichier, "%d\n", nb_mot);
    fprintf(fichier, "%ld\n", infoMem->cumul_temps);
    fprintf(fichier, "%zu\n", infoMem->cumul_alloc);
    fprintf(fichier, "%zu\n", infoMem->cumul_desalloc);
    fprintf(fichier, "%zu\n", infoMem->max_alloc);
}
