#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gererMem.h"

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
Cellule_mot* Supp_Cellule_mot(Cellule_mot** ppcell, InfoMem* infoMem) {

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

// Trouve et renvoie le prochain mot du fochier
char* Recherche_mot(FILE * fichier, InfoMem* infoMem) {
    
    char c;
    int taille = 3, nb_char = 0;
    char* mot = (char*) myMalloc(sizeof(char)*3, infoMem);

    if (!mot) {
        return NULL;
    }

    // Tant que l'on a un caractère
    while ((c = getc(fichier)) != -1) {
        // ASCII pour retour à la ligne, espace et point
        if (c != 10 && c != 32 && c != 46) {
            // Si il n'y a plus de place pour ajouté une lettre, on agrandi mot
            if (nb_char >= taille) {
                mot = (char*) myRealloc(mot, taille*2, infoMem, taille);
                if (!mot) {
                    return NULL;
                }
                taille *= 2;
            }
            mot[nb_char] = c;
            nb_char++;
        }
        else {
            break;
        }
    }
    if (nb_char >= taille) {
        mot = (char*) myRealloc(mot, taille*2, infoMem, taille);
        if (!mot) {
            return NULL;
        }
        taille *= 2;
    }
    if (c != -1) {
         mot[nb_char] = '\0';
        nb_char++;
        return mot;
    }
    return NULL;
   
}

void Affiche_mot(char* mot) {

    printf("%s\n", mot);
}


// Processus :
// 1) On regarde si le mot est dans la liste
// 2) Si oui, on supprime la cellule de la liste et on change l'ocurence. Si non, on crée une cellule
// 3) On ajoute la cellule à la liste trié

// A répéter jusqu'à la fin du fichier texte. On Obtient en début de liste les mot les plus occurent

int main(void) {

    InfoMem* infoMem = (InfoMem*) malloc(sizeof(InfoMem));
    if (!infoMem) {
        printf("probleme malloc infomem\n");
        return 0;
    }
    // Uniquement lors des test car les mot seront réccupéré du texte
    char * mot1 = (char*) myMalloc(sizeof(char) * strlen("bonjour"), infoMem);
    char * mot2 = (char*) myMalloc(sizeof(char) * strlen("bonsoir"), infoMem);
    char * mot3 = (char*) myMalloc(sizeof(char) * strlen("salut"), infoMem);
    char * mot4 = (char*) myMalloc(sizeof(char) * strlen("pharrel"), infoMem);
    if (!mot1) {
        printf("probleme malloc mot1\n");
        return 0;
    }
    mot1 = "bonjour";
    mot2 = "bonsoir";
    mot3 = "salut";
    mot4 = "pharrel";


    Liste lst_chaine = NULL;
    Cellule_mot* cell1 = Cree_Cellule_mot(mot1, infoMem);
    Cellule_mot* cell2 = Cree_Cellule_mot(mot2, infoMem);
    Cellule_mot* cell3 = Cree_Cellule_mot(mot3, infoMem);
    Cellule_mot* cell4 = Cree_Cellule_mot(mot4, infoMem);

    Add_Cellule_mot(&lst_chaine, cell1);
    Add_Cellule_mot(&lst_chaine, cell2);
    Add_Cellule_mot(&lst_chaine, cell3);

    printf("Etat initiale de la liste : ");
    Affiche_liste_chaine(&lst_chaine);
    printf("\n");

    if (Mot_in_liste(&lst_chaine, "bonjour")) {
        printf("le mot 'bonjour' est dans la liste\n");
        printf("On augmente le nombre d'occurence du mot 'bonjour'\n");
        Cellule_mot** pdeleted_cell = Mot_in_liste(&lst_chaine, "bonjour"); // pdeleted_cell est un pointeur de pointeur vers la case qui contient le mot (un pointeur sur le 'suivant' du mot précédent)
        Cellule_mot* cell = Supp_Cellule_mot(pdeleted_cell, infoMem);
        cell->nb_occ++;
        Add_Cellule_mot(&lst_chaine, cell);
        printf("Etat de la liste apres modif occ: ");
        Affiche_liste_chaine(&lst_chaine);
        printf("\n");
    }

    Add_Cellule_mot(&lst_chaine, cell4);
    printf("Ajout de 'pharrel'\n");
    printf("Etat de la liste apres ajout: ");
    Affiche_liste_chaine(&lst_chaine);
    printf("\n");


    Free_liste(&lst_chaine, infoMem);

    printf("malloc : %d, free : %d, max : %d\n", (int) infoMem->cumul_alloc, (int) infoMem->cumul_desalloc, (int) infoMem->max_alloc);
    free(infoMem);

    return 0;
}

