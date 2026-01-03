#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algo_hist.h"
#include "gererMem.h"

void usage(char * fichier) {
    printf("SYNOPSIS:\n");
    printf("  %s [-rwnapl] [n] [fichier] [...]\n\n", fichier);
    printf("DESCRIPTION:\n");
    printf("  Affiche le nombre et les mots dans les fichiers fournis.\n");
    printf("  Si aucun fichier n'est fourni, lit depuis l'entree standard.\n\n");
    printf("OPTIONS:\n");
    printf("  -r    affiche le resultat\n");
    printf("  -w    ecris le resultat dans un fichier\n");
    printf("  -n    choix du nombre de mots a afficher\n");
    printf("  -a    choix de l'algorithme\n");
    printf("  -p    affiche les performances de l'algorithme\n");
    printf("  -l    ecris les performances dans un fichier\n");
}

void AfficherHistogramme(histogramme occ) {
    printf("\n=== Résultats ===\n");
    for(int i = 0; i < occ.nbrMot; i++){
        printf("%s : %d\n", occ.mots[i], occ.occurrences[i]);
    }
}

int main(int argc, const char * argv[]) {
    histogramme occ;
    InfoMem infomem;
    InitInfoMem(&infomem);
    InitHist(&occ, &infomem);

    if (argc < 2) {
        printf("Lecture depuis l'entree standard...\n");
        FileReader(stdin, &occ, &infomem);
    } else {
        FILE * f = fopen(argv[1], "r");
        if (f == NULL) {
            fprintf(stderr, "Erreur: impossible d'ouvrir le fichier '%s'\n", argv[1]);
            return EXIT_FAILURE;
        }
        printf("Lecture du fichier '%s'...\n", argv[1]);
        FileReader(f, &occ, &infomem);
        fclose(f);
    }

    AfficherHistogramme(occ);

    FreeHistogramme(&occ, &infomem);

    printf("\n=== Statistiques mémoire ===\n");
    printf("Mémoire allouée: %zu bytes\n", infomem.cumul_alloc);
    printf("Mémoire libérée: %zu bytes\n", infomem.cumul_desalloc);
    printf("Pic d'allocation: %zu bytes\n", infomem.max_alloc);

    
    return 0;
}
