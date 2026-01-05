#ifndef ALGO_LST_CHAINE_H
#define ALGO_LST_CHAINE_H

#include <stdio.h>
#include "gererMem.h"

#define MAX_LENGTH 200

typedef struct cellule_mot {
    char* mot;
    size_t taille_mot;
    int nb_occ;
    struct cellule_mot* suivant;
} Cellule_mot, *Liste;

Cellule_mot* Cree_Cellule_mot(char* mot, InfoMem* infoMem);

Cellule_mot* Supp_Cellule_mot(Cellule_mot** ppcell);

Cellule_mot** Mot_in_liste(Cellule_mot** plst, char* mot);

void Add_Cellule_mot(Cellule_mot** plst, Cellule_mot* cell);

void Free_liste(Cellule_mot** plst, InfoMem* infoMem);

void Affiche_liste_chaine(Cellule_mot** plst);

void Affiche_n_liste_chaine(Cellule_mot** plst, int n);

int DivLine2(char *line, int start, char *mot);

void Algo_lst_chaine(FILE* fichier, Cellule_mot** plst, InfoMem* infoMem);

int Compte_mot(Cellule_mot** plst);

void Ecrit_resultats(FILE* fichier, Cellule_mot** plst, int nb_mot_choisi);

void Ecrit_performances(FILE * fichier, InfoMem* infoMem, int nb_mot);

#endif
