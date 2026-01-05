#ifndef READFILE_H
#define READFILE_H

#include <stdio.h>
#include "gererMem.h"

#define MAX_LENGTH 200

typedef struct {
    char **mots;
    int *occurrences;
    int nbrMot;
    int taille_allouee;
} histogramme;

void InitHist(histogramme *h, InfoMem *i);

void ReadStrHist(histogramme h);

void AfficherHistogramme(histogramme occ, int n);

int InHist(histogramme h, char *mot);

int DivLine(char *line, int start, char *mot);

void MaxSizePlus(histogramme *h, InfoMem *i);

int HistWordCount(histogramme h);

void FileReader(FILE * fichier, histogramme *h, InfoMem *i);

void FreeHistogramme(histogramme *h, InfoMem *i);

void TopNmot(histogramme *h, int n, InfoMem *i);

void swap(histogramme* h, int i, int j);

int RechercheMax(histogramme* h, int debut);

void TrieHistogramme(histogramme* h, InfoMem *i);

int Compte_mot_hist(histogramme h);

void Ecrit_resultats_hist(FILE* fichier, histogramme* h, int nb_mot, char* algo);

void Ecrit_perf_hist(FILE* fichier, InfoMem* infoMem, int nb_mot, char* algo);


#endif
