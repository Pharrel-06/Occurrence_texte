#ifndef ALGO_LST_CHAINE_H
#define ALGO_LST_CHAINE_H

#include <stdio.h>
#include "gererMem.h"

#define MAX_LENGTH 200

typedef struct cellule_mot {
    char* mot;
    int nb_occ;
    struct cellule_mot* suivant;
} Cellule_mot, *Liste;

Cellule_mot* Cree_Cellule_mot(char* mot, InfoMem* infoMem);

Cellule_mot* Supp_Cellule_mot(Cellule_mot** ppcell);

Cellule_mot** Mot_in_liste(Cellule_mot** plst, char* mot);

void Add_Cellule_mot(Cellule_mot** plst, Cellule_mot* cell);

void Free_liste(Cellule_mot** plst, InfoMem* infoMem);

void Affiche_liste_chaine(Cellule_mot** plst);

int recherche_mot(char* ligne, int index_ligne, char** mot, int *taille_buffer_mot, InfoMem* infoMem);

int Algo_lst_chaine(FILE* fichier, Cellule_mot** plst, InfoMem* infoMem);


#endif
