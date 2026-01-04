#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algo_hist.h"
#include "algo_lst_chaine.h"
#include "gererMem.h"

void InitInfoMem(InfoMem *i){
    i->cumul_alloc = 0;
    i->cumul_desalloc = 0;
    i->max_alloc = 0;

    i->temps_debut = 0;
    i->temps_fin = 0;
    i->cumul_temps = 0;
}

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

void showPerfs(InfoMem i) {
    fprintf(stdout, "\n=== Statistiques mémoire ===\n");
    fprintf(stdout, "Temps d'execution: %ld\n", i.cumul_temps);
    fprintf(stdout, "Mémoire allouée: %zu\n", i.cumul_alloc);
    fprintf(stdout, "Mémoire libérée: %zu\n", i.cumul_desalloc);
    fprintf(stdout, "Pic d'allocation: %zu\n", i.max_alloc);
}

int main(int argc, const char * argv[]) {
    histogramme occ;
    InfoMem infomem;
    InitInfoMem(&infomem);
    InitHist(&occ, &infomem);

    if (argc < 2) {
        fprintf(stdout, "Lecture depuis l'entree standard...\n");
        FileReader(stdin, &occ, &infomem);
    } else {
        int nbr = 10, help = 0, showres = 0, logres = 0, showperf = 0, logperf = 0;
        char * choix_algo = (char *)malloc(sizeof(char)*5);

        for(int arg = 1; arg < argc; arg++){
            if (strcmp(argv[arg], "-n") == 0 && arg+1 < argc) {nbr = atoi(argv[arg+1]);}
            if (strcmp(argv[arg], "-a") == 0 && arg+1 < argc) {choix_algo = (char *)argv[arg+1];}
            if (strcmp(argv[arg], "-help") == 0){help++;}
            if (strcmp(argv[arg], "-r") == 0){showres++;}
            if (strcmp(argv[arg], "-w") == 0){logres++;}
            if (strcmp(argv[arg], "-p") == 0){showperf++;}
            if (strcmp(argv[arg], "-l") == 0){logperf++;}
        }

        if (help != 0){usage((char *)argv[0]);}

        for(int arg = 1; arg < argc; arg++){
                if (strcmp(argv[arg], "-n") == 0 || strcmp(argv[arg], "-a") == 0) {
                    arg++; // skip la valeur qui casse tout
                    continue;
                }
                if (argv[arg][0] == '-') {continue;}

                FILE * f = fopen(argv[arg], "r");
                if (f == NULL) {
                    fprintf(stderr, "Erreur: impossible d'ouvrir le fichier '%s'\n", argv[arg]); return EXIT_FAILURE;
                }

                if (strcmp(choix_algo, "algo1") == 0){
                    fprintf(stdout, "Lecture du fichier '%s' avec algorithme par histogramme non-trier...\n", argv[arg]);
                    FileReader(f, &occ, &infomem);
                    fclose(f);
                    if (showres != 0){TopNmot(&occ, nbr, &infomem);}
                    if (logres != 0) {continue;}
                    if (showperf != 0) {showPerfs(infomem);}
                    if (logperf != 0) {continue;}
                    FreeHistogramme(&occ, &infomem);
                }else if (strcmp(choix_algo, "algo2") == 0) {
                    fprintf(stdout, "Lecture du fichier '%s' avec algorithme par histogramme trier...\n", argv[arg]);
                    FileReader(f, &occ, &infomem);
                    fclose(f);
                    if (showres != 0){TopNmot(&occ, nbr, &infomem);}
                    if (logres != 0) {continue;}
                    if (showperf != 0) {showPerfs(infomem);}
                    if (logperf != 0) {continue;}
                    FreeHistogramme(&occ, &infomem);
                }else if (strcmp(choix_algo, "algo3") == 0) {
                    fprintf(stdout, "Lecture du fichier '%s' avec algorithme par listes chainees...\n", argv[arg]);
                    Liste lst = NULL;
                    Algo_lst_chaine(f, &lst, &infomem);
                    fclose(f);
                }else{fprintf(stderr,"Mauvais algorithme entree... KABOOM\n"); return EXIT_FAILURE;}
        }
    }

    // printf("\n=== Statistiques mémoire ===\n");
    // printf("Mémoire allouée: %zu bytes\n", infomem.cumul_alloc);
    // printf("Mémoire libérée: %zu bytes\n", infomem.cumul_desalloc);
    // printf("Pic d'allocation: %zu bytes\n", infomem.max_alloc);

    
    return 0;
}
