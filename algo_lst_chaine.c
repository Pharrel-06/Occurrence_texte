#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gererMem.h"

#define MAX_LENGTH 200

// Algorithme qui utilise des listes chainée ordonnée pour compter le nombre d'occurence des mot du texte


// Structure de la liste chainée
typedef struct cellule_mot {
    char* mot;
    int nb_occ;
    struct cellule_mot* suivant;
} Cellule_mot, *Liste;


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


// Fonction de manipulation de fichier

// Vérifie si un fichier peut être ouvert
int peut_ouvrir_fichier(char * chemin) {

    FILE * ouvert = fopen(chemin, "r");
    if (ouvert == NULL) {
        return 0;
    }
    fclose(ouvert);
    return 1;
}

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
int Algo_lst_chaine(FILE* fichier, Cellule_mot** plst, InfoMem* infoMem) {

    // Création du buffer pour les lignes
    char* ligne = (char*) myMalloc(sizeof(char)*MAX_LENGTH, infoMem);
    if (!ligne) {
        printf("Problème allocation pour ligne\n");
        return 0;
    }

    // Création du buffer pour les mots
    int taille_buffer_mot = 10;
    char* buffer_mot = (char*) myMalloc(sizeof(char)*taille_buffer_mot, infoMem);
    if (!buffer_mot) {
        printf("Problème allocation pour mot\n");
        return -1;
    }

    // Tant que l'on a pas attaint la fin du fichier
    while (!feof(fichier)) {
        // On vérifie si la ligne à bien été lu
        char* ligne_lu = fgets(ligne, sizeof(char)*MAX_LENGTH, fichier);
        if (!ligne_lu) {
            printf("Erreur de lecture\n");
            return -1;
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
                    printf("Problème allocation pour cellule mot\n");
                    return -1;
                }
                Add_Cellule_mot(plst, new_cell);
            }

            printf("Nouvelle etat de la liste : ");
            Affiche_liste_chaine(plst);
        }
    }
    return 1;
}

int main(void) {

    if (peut_ouvrir_fichier("test.txt")) {
        FILE* fichier = fopen("test.txt", "r");
        Liste lst = NULL;
        InfoMem* infoMem = malloc(sizeof(InfoMem));
        if (!infoMem) {
            printf("Probleme malloc infoMem\n");
            return 0;
        }
        printf("Etat initial de la liste (NULL) :");
        Affiche_liste_chaine(&lst);

        printf("\nAppel de l'algo\n");
        Algo_lst_chaine(fichier, &lst, infoMem);

        printf("\nEtat final de la liste :");
        Affiche_liste_chaine(&lst);

        Free_liste(&lst, infoMem);

        fclose(fichier);
    }
    return 0;
}

