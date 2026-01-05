#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "gererMem.h"

#define MAX_LENGTH 200

// Algorithme qui utilise des listes chainée ordonnée pour compter le nombre d'occurence des mot du texte


// Structure de la liste chainée
typedef struct cellule_mot {
    char* mot;
    size_t taille_mot;
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
    new_cell->taille_mot = strlen(mot) + 1;
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
    printf("Recherche du mot : %s\n", mot);

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
void Free_liste(Cellule_mot **plst, InfoMem *infoMem)
{
    Cellule_mot *courant = *plst;
    Cellule_mot *suivant;

    while (courant) {
        suivant = courant->suivant;
        myFree(courant->mot, infoMem, courant->taille_mot);
        myFree(courant, infoMem, sizeof(Cellule_mot));
        courant = suivant;
    }
    *plst = NULL;
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
int DivLine(char *line, int start, char *mot) {
    int i = start;
    while (line[i] && (line[i] == ' ' || line[i] == '\t' || line[i] == '\n')) {
        i++;
    }

    int j = 0;
    while (line[i] && line[i] != ' ' && line[i] != '\t' && line[i] != '\n') {
        if (j < 41) {
            mot[j++] = line[i];
        }
        i++;
    }
    mot[j] = '\0';

    if (j > 41) {return i;}
    if (j > 0) {return i;}
    return -1;
}

// Algo entier avec les listes chainées
int Algo_lst_chaine(FILE* fichier, Cellule_mot** plst, InfoMem* infoMem) {

    // Création du buffer pour les lignes
    char* ligne = (char*) myMalloc(sizeof(char)*MAX_LENGTH, infoMem);
    if (!ligne) {
        printf("Problème allocation pour ligne\n");
        return -1;
    }

    char* buffer_mot = myMalloc(sizeof(char)*42, infoMem);
    if (!buffer_mot) {
        printf("Problème allocation pour buffer_mot\n");
        myFree(ligne, infoMem, sizeof(char)*MAX_LENGTH);
        return -1;
    }

    // Tant que l'on a pas atteint la fin du fichier
    while (fgets(ligne, MAX_LENGTH, fichier) != NULL) {
        if (ferror(fichier)) {
            fprintf(stderr, "Reading error\n");
            break;
        }
        // Tant que l'on a pas réccupéré un mot, on parcours la ligne
        int index_ligne = 0;
        while((index_ligne = DivLine(ligne, index_ligne, buffer_mot)) != -1) {
            char* mot = (char*) myMalloc(sizeof(char)*(strlen(buffer_mot) + 1), infoMem);
            if (!mot) {
                    printf("Problème allocation pour mot\n");
                    myFree(ligne, infoMem, sizeof(char)*MAX_LENGTH);
                    myFree(buffer_mot, infoMem, sizeof(char)*42);
                    return -1;
            }
            strcpy(mot, buffer_mot);
            Cellule_mot** pcell_supp = Mot_in_liste(plst, mot);
            // Le mot est dans la liste
            if (pcell_supp) {
                Cellule_mot* cell_supp = Supp_Cellule_mot(pcell_supp);
                cell_supp->nb_occ ++;
                Add_Cellule_mot(plst, cell_supp);
            }
            // Le mot n'est pas dans la liste
            else {
                //char* mot = (char*) myMalloc(sizeof(char)*(strlen(buffer_mot) + 1), infoMem);
                //if (!mot) {
                //    printf("Problème allocation pour mot\n");
                //    myFree(ligne, infoMem, sizeof(char)*MAX_LENGTH);
                //    myFree(buffer_mot, infoMem, sizeof(char)*42);
                //    return -1;
                // }
                //strcpy(mot, buffer_mot);
                Cellule_mot* new_cell = Cree_Cellule_mot(mot, infoMem);
                if (!new_cell) {
                    printf("Problème allocation pour cellule mot\n");
                    myFree(mot, infoMem, sizeof(char)*(strlen(mot) + 1));
                    myFree(ligne, infoMem, sizeof(char)*MAX_LENGTH);
                    myFree(buffer_mot, infoMem, sizeof(char)*42);
                    return -1;
                }
                Add_Cellule_mot(plst, new_cell);
            }
        }
    }
    myFree(ligne, infoMem, sizeof(char)*MAX_LENGTH);
    myFree(buffer_mot, infoMem, sizeof(char)*42);
    return 1;
}

// Compte le nombre de mot dans le fichier avec la liste
int Compte_mot(Cellule_mot** plst) {

    int nb_mot = 0;
    for (; *plst; plst = &((*plst)->suivant)) {
        nb_mot += (*plst)->nb_occ;
    }
    return nb_mot;
}

// Ecrit les résultats de performance dans le fichier
void Ecrit_resultats(FILE* fichier, Cellule_mot** plst, int nb_mot_choisi) {

    int nb_mot = 0;
    for (; *plst && nb_mot < nb_mot_choisi; plst = &((*plst)->suivant), nb_mot++) {
        fprintf(fichier, "%s %d\n", (*plst)->mot, (*plst)->nb_occ);
    }
}

void Ecrit_performances(FILE * fichier, InfoMem* infoMem, int nb_mot, time_t debut, time_t fin) {

    fprintf(fichier, "%d\n", nb_mot);
    //fprintf(fichier, "%lld\n", fin - debut);
    fprintf(fichier, "%zu\n", infoMem->cumul_alloc);
    fprintf(fichier, "%zu\n", infoMem->cumul_desalloc);
    fprintf(fichier, "%zu\n", infoMem->max_alloc);
}

int main(void) {

    if (peut_ouvrir_fichier("corpus_10k.txt")) {

        FILE* fichier = fopen("corpus_10k.txt", "r");
        Liste lst = NULL;
        InfoMem infoMem = {0, 0, 0};   // <-- initialiser les compteurs à 0
        InfoMem* pInfoMem = &infoMem;

        printf("Etat initial de la liste (NULL) :");
        Affiche_liste_chaine(&lst);

        printf("\nAppel de l'algo\n");
        Algo_lst_chaine(fichier, &lst, pInfoMem);

        //printf("\nEtat final de la liste :");
        //Affiche_liste_chaine(&lst);

        printf("fin algo\n");

        Free_liste(&lst, pInfoMem);

        fclose(fichier);

        printf("\n=== Statistiques memoire ===\n");
        printf("Memoire allouee: %zu bytes\n", pInfoMem->cumul_alloc);
        printf("Memoire liberee: %zu bytes\n", pInfoMem->cumul_desalloc);
        printf("Pic d'allocation: %zu bytes\n", pInfoMem->max_alloc);

    }
    return 0;
}

