#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "gererMem.h"
#include "algo_lst_chaine.h"


// Fonction de base pour la manipulation de liste chainée

// Cree une cellule pour un nouveau mot
Cellule_mot* Cree_Cellule_mot(char* mot, InfoMem* infoMem) {
    Cellule_mot* new_cell = (Cellule_mot*) myMalloc(sizeof(Cellule_mot), infoMem);
    if (!new_cell) {
        return NULL;
    }
    new_cell->mot = mot;
    new_cell->nb_occ = 1;
    new_cell->suivant = NULL;
    return new_cell;
}

// Supprime et renvoie la cellule 
Cellule_mot* Supp_Cellule_mot(Cellule_mot** ppcell) {
    if (!(*ppcell)) {
        return NULL;
    }
    Cellule_mot* tmp = (*ppcell);
    *ppcell = ((*ppcell)->suivant);
    return tmp;
}

// Vérifie si un mot est dans la liste, si oui il renvoie l'adresse de la cellule précédente pour le suprrimer
Cellule_mot** Mot_in_liste(Cellule_mot** plst, char* mot) {
    // On parcours toute la liste 
    for (; *plst; plst = &((*plst)->suivant)) {
        // Si on retrouve le mot, on renvoie l'adresse de la cellule 
        if (strcmp(mot, (*plst)->mot) == 0) {
            return plst;
        }
    }
    return NULL;
}

// Ajoute une nouvelle cellule au bonne endroit dans la liste
void Add_Cellule_mot(Cellule_mot** plst, Cellule_mot* cell) {
    for (; *plst; plst = &((*plst)->suivant)) {
        if ((*plst)->nb_occ <= cell->nb_occ) {
            break;
        }
    }
    cell->suivant = (*plst);
    *plst = cell;
}

// Free toute une liste
void Free_liste(Cellule_mot** plst, InfoMem* infoMem) {
    Cellule_mot* next_cell;
    for (; *plst; plst = &(next_cell)) {
        next_cell = (*plst)->suivant;
        myFree((*plst)->mot, infoMem, sizeof(char)*strlen((*plst)->mot));
        myFree(*plst, infoMem, sizeof(Cellule_mot));
        *plst = NULL;
    }
}

// Affiche tout les mot de la liste chainée
void Affiche_liste_chaine(Cellule_mot** plst) {

    for (; *plst; plst = &((*plst)->suivant)) {
        printf("%s (%d)-> ",(*plst)->mot, (*plst)->nb_occ);
    }
    printf("X\n");
}

void Affiche_n_liste_chaine(Cellule_mot** plst, int n) {
    for (int i = 0; (*plst) && (i < n); (plst = &((*plst)->suivant)), i++) {
        printf("%s (%d)-> ",(*plst)->mot, (*plst)->nb_occ);
    }
    printf("...\n");
}

// Fonction de manipulation de fichier

// Parcours une ligne du fichier et renvoie le prochain mot
int recherche_mot(char* ligne, int index_ligne, char** mot, int *taille_buffer_mot, InfoMem* infoMem) {

    // Parcour jusqu'à trouver une lettre
    while (ligne[index_ligne] && (ligne[index_ligne] == ' ' || ligne[index_ligne] == '\t' || ligne[index_ligne] == '\n' || ligne[index_ligne] == ',' || ligne[index_ligne] == '.')) {
        index_ligne++;
    }

    // Parcours du mot
    int index_mot = 0;
    while (ligne[index_ligne] && (ligne[index_ligne] != ' ' && ligne[index_ligne] != '\t' && ligne[index_ligne] != '\n' && ligne[index_ligne] != ',' && ligne[index_ligne] != '.')) {
        // si il n'y a plus de place dans le buffer du mot, on l'agrandi
        if (index_mot + 1 >= (*taille_buffer_mot)) {
            char* tmp = (char*) myRealloc(*mot, sizeof(char)*((*taille_buffer_mot)*2), infoMem, sizeof(char)*(*taille_buffer_mot));
            if (!tmp) {
                return -1;
            }
            *mot = tmp;
            *taille_buffer_mot *= 2;
        }
        // *mot car mot est passé par pointeur de pointeur
        (*mot)[index_mot] = ligne[index_ligne];
        index_ligne++;
        index_mot++;
    }
    // si il n'y a plus de place dans le buffer du mot, on l'agrandi
    if (index_mot + 1 >= (*taille_buffer_mot)) {
        char* tmp = (char*) myRealloc(*mot, sizeof(char)*((*taille_buffer_mot)*2), infoMem, sizeof(char)*(*taille_buffer_mot));
        if (!tmp) {
            return -1;
        }
        *mot = tmp;
        *taille_buffer_mot *= 2;
    }
    // Ajout du crarctère null pour les chaines de caractères
    (*mot)[index_mot] = '\0';
    if (index_mot>0) {
        return index_ligne;
    }
    return -1;
}

// Algo entier avec les listes chainées
void Algo_lst_chaine(FILE* fichier, Cellule_mot** plst, InfoMem* infoMem) {
    infoMem->temps_debut = time(NULL);
    // Création du buffer pour les lignes
    char* ligne = (char*) myMalloc(sizeof(char)*MAX_LENGTH, infoMem);
    if (!ligne) {
        fprintf(stderr, "Problème allocation pour mot\n");
    }

    // Création du buffer pour les mots
    int taille_buffer_mot = 10;
    char* buffer_mot = (char*) myMalloc(sizeof(char)*taille_buffer_mot, infoMem);
    if (!buffer_mot) {
        fprintf(stderr, "Problème allocation pour mot\n"); 
    }

    // Tant que l'on a pas atteint la fin du fichier
    while (!feof(fichier)) {
        // On vérifie si la ligne à bien été lu
        char* ligne_lu = fgets(ligne, sizeof(char)*MAX_LENGTH, fichier);
        if (!ligne_lu) {
            break;
        }

        // Tant que l'on a pas réccupéré un mot, on parcours la ligne
        int index_ligne = 0;
        while((index_ligne = recherche_mot(ligne, index_ligne, &buffer_mot, &taille_buffer_mot, infoMem)) != -1) {
            // Copie du mot autre part pour ne pas le perdre
            char* mot = (char*) myMalloc(sizeof(char)*(strlen(buffer_mot) + 1), infoMem);
            strcpy(mot, buffer_mot);
            Cellule_mot** pcell_supp = Mot_in_liste(plst, mot);
            // Le mot est dans la liste
            if (pcell_supp) {
                // Une autre copie du mot existe déjà, on libère l'espace de la copie
                myFree(mot, infoMem, sizeof(char)*strlen(mot));
                Cellule_mot* cell_supp = Supp_Cellule_mot(pcell_supp);
                cell_supp->nb_occ ++;
                Add_Cellule_mot(plst, cell_supp);
            }
            // Le mot n'est pas dans la liste
            else {
                Cellule_mot* new_cell = Cree_Cellule_mot(mot, infoMem);
                if (!new_cell) {
                    fprintf(stderr, "Problème allocation pour cellule mot\n");
                }
                Add_Cellule_mot(plst, new_cell);
            }
        }
    }
    myFree(ligne, infoMem, sizeof(char)*MAX_LENGTH);
    myFree(buffer_mot, infoMem, sizeof(char)*taille_buffer_mot);
    infoMem->temps_fin = time(NULL);
    infoMem->cumul_temps += infoMem->temps_fin - infoMem->temps_debut;
    infoMem->temps_debut = 0; infoMem->temps_fin = 0;
}

// Compte le nombre de mot dans le fichier avec la liste
int Compte_mot(Cellule_mot** plst) {

    int nb_mot = 0;
    for (; *plst; plst = &((*plst)->suivant)) {
        nb_mot += (*plst)->nb_occ;
    }
    return nb_mot;
}

void Ecrit_resultats(FILE* fichier, Cellule_mot** plst, int nb_mot_choisi) {
    int nb_mot = 0;
    for (; *plst && nb_mot < nb_mot_choisi; plst = &((*plst)->suivant), nb_mot++) {
        fprintf(fichier, "%s %d\n", (*plst)->mot, (*plst)->nb_occ);
    }
}

void Ecrit_performances(FILE * fichier, InfoMem* infoMem, int nb_mot) {
    fprintf(fichier, "%d\n", nb_mot);
    fprintf(fichier, "%ld\n", infoMem->cumul_temps);
    fprintf(fichier, "%zu\n", infoMem->cumul_alloc);
    fprintf(fichier, "%zu\n", infoMem->cumul_desalloc);
    fprintf(fichier, "%zu\n", infoMem->max_alloc);
}