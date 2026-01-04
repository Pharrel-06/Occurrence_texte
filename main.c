#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algo_hist.h"
#include "algo_lst_chaine.h"
#include "gererMem.h"

void usage(char * fichier) {
    fprintf(stdout, "SYNOPSIS:\n");
    fprintf(stdout, "  %s [-n int] [-a algo1|algo2|algo3|…] [—help] [-r] [-w fileRes] [-p] [-l filePerf] [fichier] [...]\n\n", fichier);
    fprintf(stdout, "DESCRIPTION:\n");
    fprintf(stdout, "  Affiche le nombre et les mots dans les fichiers fournis.\n");
    fprintf(stdout, "  Si aucun fichier n'est fourni, lit depuis l'entree standard.\n\n");
    fprintf(stdout, "OPTIONS:\n");
    fprintf(stdout, "  -n    choix du nombre de mots a afficher\n");
    fprintf(stdout, "  -a    choix de l'algorithme\n");
    fprintf(stdout, "  -help affiche le synopsis\n");
    fprintf(stdout, "  -r    affiche le resultat\n");
    fprintf(stdout, "  -w    ecris le resultat dans un fichier\n");
    fprintf(stdout, "  -p    affiche les performances de l'algorithme\n");
    fprintf(stdout, "  -l    ecris les performances dans un fichier\n");
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
        int nbr = 10, help = 0, showres = 0, logres = 0, showperf = 0, logperf = 0;
        char * choix_algo = (char *)malloc(sizeof(char)*5);

        for(int arg = 1; arg < argc; arg++){
            if (argv[arg] == "-n"){nbr = argv[arg+1];};
            if (argv[arg] == "-a"){choix_algo = argv[arg+1];};
            if (argv[arg] == "-help"){help++};
            if (argv[arg] == "-r"){showres++};
            if (argv[arg] == "-w"){logres++};
            if (argv[arg] == "-p"){showperf++};
            if (argv[arg] == "-l"){logperf++};
        }
        FILE * f = fopen(argv[1], "r");
        if (f == NULL) {
            fprintf(stderr, "Erreur: impossible d'ouvrir le fichier '%s'\n", argv[1]);
            return EXIT_FAILURE;
        }
        printf("Lecture du fichier '%s'...\n", argv[1]);
        FileReader(f, &occ, &infomem);
        fclose(f);
    }
    // A mettre dans la partie pour utiliser l'algo de liste chaine
    // FILE * f = fopen(argv[1], "r");
    // if (f == NULL) {
    //     fprintf(stderr, "Erreur: impossible d'ouvrir le fichier '%s'\n", argv[1]);
    //     return EXIT_FAILURE;
    // }
    // printf("Lecture du fichier '%s'...\n", argv[1]);
    // Liste lst = NULL;
    // Algo_lst_chaine(f, &lst, &infomem);
    // fclose(f);

    FreeHistogramme(&occ, &infomem);

    printf("\n=== Statistiques mémoire ===\n");
    printf("Mémoire allouée: %zu bytes\n", infomem.cumul_alloc);
    printf("Mémoire libérée: %zu bytes\n", infomem.cumul_desalloc);
    printf("Pic d'allocation: %zu bytes\n", infomem.max_alloc);

    
    return 0;
}
