#ifndef READFILE_H
#define READFILE_H

#include <stdio.h>
#include "gererMem.h"

#define MAX_LENGTH 200

typedef struct {
    char **mots;
    int *occurrences; 
    int nbrMot;
} histogramme;

void InitHist(histogramme *h, InfoMem *i);

void InitInfoMem(InfoMem *i);

void ReadStrHist(histogramme h);

int InHist(histogramme h, char *mot);

int DivLine(char *line, int start, char *mot);

void FileReader(FILE * fichier, histogramme *h, InfoMem *i);

void FreeHistogramme(histogramme *h, InfoMem *i);

#endif
