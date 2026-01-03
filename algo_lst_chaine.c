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
int recherche_mot(char* ligne, int index_ligne, char* mot, int taille_mot, InfoMem* infoMem) {

    // Parcour jusqu'à trouver une lettre
    while (ligne[index_ligne] && (ligne[index_ligne] == ' ' || ligne[index_ligne] == '\t' || ligne[index_ligne] == '\n' || ligne[index_ligne] == ',' || ligne[index_ligne] == '.')) {
        index_ligne++;
    }

    // Parcours du mot
    int index_mot = 0;
    while (ligne[index_ligne] && (ligne[index_ligne] != ' ' && ligne[index_ligne] != '\t' && ligne[index_ligne] != '\n' && ligne[index_ligne] != ',' && ligne[index_ligne] != '.')) {
        if (index_mot >= taille_mot) {
            mot = (char*) myRealloc(mot, sizeof(char)*taille_mot*2, infoMem, sizeof(char)*taille_mot);
            if (!mot) {
                return -1;
            }
            taille_mot *= 2;
        }
        mot[index_mot] = ligne[index_ligne];
        index_ligne++;
        index_mot++;
    }
    if (index_mot >= taille_mot) {
        mot = (char*) myRealloc(mot, sizeof(char)*taille_mot*2, infoMem, sizeof(char)*taille_mot);
        if (!mot) {
            return -1;
        }
        taille_mot *= 2;
    }
    mot[index_mot] = '\0';

    if (index_mot>0) {
        return index_ligne;
    }
    return -1;
}

// Algo entier avec les listes chainées
int Algo_lst_chaine(FILE* fichier, Cellule_mot** plst, InfoMem* infoMem) {

    char* ligne = (char*) myMalloc(sizeof(char)*MAX_LENGTH, infoMem);
    if (!ligne) {
        printf("Problème allocation pour ligne\n");
        return 0;
    }

    char* buffer_mot = (char*) myMalloc(sizeof(char)*50, infoMem);
    if (!buffer_mot) {
        printf("Problème allocation pour mot\n");
        return 0;
    }

    while (!feof(fichier)) {
        char* ligne_lu = fgets(ligne, sizeof(char)*MAX_LENGTH, fichier);
        if (!ligne_lu) {
            printf("Erreur de lecture\n");
            return 0;
        }

        int index_ligne = 0;
        while((index_ligne = recherche_mot(ligne, index_ligne, buffer_mot, 50, infoMem)) != -1) {

            char* mot = (char*) myMalloc(sizeof(char)*(strlen(buffer_mot) + 1), infoMem);
            strcpy(mot, buffer_mot);
            Cellule_mot** pcell_supp = Mot_in_liste(plst, mot);
            // Mot dans liste
            if (pcell_supp) {
                Cellule_mot* cell_supp = Supp_Cellule_mot(pcell_supp);
                cell_supp->nb_occ ++;
                Add_Cellule_mot(plst, cell_supp);
            }
            // Mot pas dans liste
            else {
                Cellule_mot* new_cell = Cree_Cellule_mot(mot, infoMem);
                if (!new_cell) {
                    printf("Problème allocation pour cellule mot\n");
                    return 0;
                }
                Add_Cellule_mot(plst, new_cell);
            }

            printf("Nouvelle etat de la liste : ");
            Affiche_liste_chaine(plst);
        }
    }
    return 1;
}

// Processus :
// 1) On regarde si le mot est dans la liste
// 2) Si oui, on supprime la cellule de la liste et on change l'ocurence. Si non, on crée une cellule
// 3) On ajoute la cellule à la liste trié

// A répéter jusqu'à la fin du fichier texte. On Obtient en début de liste les mot les plus occurent

// Test simple pour les fonction de liste chainée
//InfoMem* infoMem = (InfoMem*) malloc(sizeof(InfoMem));
// if (!infoMem) {
//     printf("probleme malloc infomem\n");
//     return 0;
// }
// // Uniquement lors des test car les mot seront réccupéré du texte
// char * mot1 = (char*) myMalloc(sizeof(char) * strlen("bonjour"), infoMem);
// char * mot2 = (char*) myMalloc(sizeof(char) * strlen("bonsoir"), infoMem);
// char * mot3 = (char*) myMalloc(sizeof(char) * strlen("salut"), infoMem);
// char * mot4 = (char*) myMalloc(sizeof(char) * strlen("pharrel"), infoMem);
// if (!mot1) {
//     printf("probleme malloc mot1\n");
//     return 0;
// }
// mot1 = "bonjour";
// mot2 = "bonsoir";
// mot3 = "salut";
// mot4 = "pharrel";


// Liste lst_chaine = NULL;
// Cellule_mot* cell1 = Cree_Cellule_mot(mot1, infoMem);
// Cellule_mot* cell2 = Cree_Cellule_mot(mot2, infoMem);
// Cellule_mot* cell3 = Cree_Cellule_mot(mot3, infoMem);
// Cellule_mot* cell4 = Cree_Cellule_mot(mot4, infoMem);

// Add_Cellule_mot(&lst_chaine, cell1);
// Add_Cellule_mot(&lst_chaine, cell2);
// Add_Cellule_mot(&lst_chaine, cell3);

// printf("Etat initiale de la liste : ");
// Affiche_liste_chaine(&lst_chaine);
// printf("\n");

// if (Mot_in_liste(&lst_chaine, "bonjour")) {
//     printf("le mot 'bonjour' est dans la liste\n");
//     printf("On augmente le nombre d'occurence du mot 'bonjour'\n");
//     Cellule_mot** pdeleted_cell = Mot_in_liste(&lst_chaine, "bonjour"); // pdeleted_cell est un pointeur de pointeur vers la case qui contient le mot (un pointeur sur le 'suivant' du mot précédent)
//     Cellule_mot* cell = Supp_Cellule_mot(pdeleted_cell);
//     cell->nb_occ++;
//     Add_Cellule_mot(&lst_chaine, cell);
//     printf("Etat de la liste apres modif occ: ");
//     Affiche_liste_chaine(&lst_chaine);
//     printf("\n");
// }

// Add_Cellule_mot(&lst_chaine, cell4);
// printf("Ajout de 'pharrel'\n");
// printf("Etat de la liste apres ajout: ");
// Affiche_liste_chaine(&lst_chaine);
// printf("\n");


// Free_liste(&lst_chaine, infoMem);

// printf("malloc : %d, free : %d, max : %d\n", (int) infoMem->cumul_alloc, (int) infoMem->cumul_desalloc, (int) infoMem->max_alloc);
// free(infoMem);

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

