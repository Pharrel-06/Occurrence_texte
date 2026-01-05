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
    (i.cumul_temps != 0) ? fprintf(stdout, "Temps d'execution: %ld secondes\n", i.cumul_temps) : fprintf(stdout, "Temps d'execution instantanee\n");
    fprintf(stdout, "Mémoire allouée: %zu bytes (%.2f Mo)\n", i.cumul_alloc, i.cumul_alloc / (1024.0 * 1024.0));
    fprintf(stdout, "Mémoire libérée: %zu bytes (%.2f Mo)\n", i.cumul_desalloc, i.cumul_desalloc / (1024.0 * 1024.0));
    fprintf(stdout, "Pic d'allocation: %zu bytes (%.2f Mo)\n", i.max_alloc, i.max_alloc / (1024.0 * 1024.0));
}

int main(int argc, const char *argv[]) {
    InfoMem infomem;
    InitInfoMem(&infomem);

    if (argc < 2) {
        histogramme occ;
        InitHist(&occ, &infomem);
        fprintf(stdout, "Lecture depuis l'entree standard...\n");
        FileReader(stdin, &occ, &infomem);
        FreeHistogramme(&occ, &infomem);
        return 0;
    }

    int nbr = 10, help = 0, showres = 0, logres = 0, showperf = 0, logperf = 0;
    char *choix_algo = NULL;
    char *FileRes = NULL;
    char *FilePerf = NULL;

    FILE *fres = NULL;
    FILE *fperf = NULL;

    for (int arg = 1; arg < argc; arg++) {
        if (strcmp(argv[arg], "-n") == 0 && arg + 1 < argc) nbr = atoi(argv[++arg]);
        else if (strcmp(argv[arg], "-a") == 0 && arg + 1 < argc) choix_algo = (char *)argv[++arg];
        else if (strcmp(argv[arg], "-w") == 0 && arg + 1 < argc) { FileRes = (char *)argv[++arg]; logres = 1; }
        else if (strcmp(argv[arg], "-l") == 0 && arg + 1 < argc) { FilePerf = (char *)argv[++arg]; logperf = 1; }
        else if (strcmp(argv[arg], "-help") == 0) help = 1;
        else if (strcmp(argv[arg], "-r") == 0) showres = 1;
        else if (strcmp(argv[arg], "-p") == 0) showperf = 1;
    }

    if (help || choix_algo == NULL) {
        usage((char *)argv[0]);
        return EXIT_FAILURE;
    }

    if (logres) {
        fres = fopen(FileRes, "w");
        if (!fres) {perror("Erreur fichier resultats"); return EXIT_FAILURE;}
    }

    if (logperf) {
        fperf = fopen(FilePerf, "w");
        if (!fperf) {perror("Erreur fichier performances"); return EXIT_FAILURE;}
    }

    for (int arg = 1; arg < argc; arg++) {
        if (argv[arg][0] == '-') {
            if (strcmp(argv[arg], "-n") == 0 ||
                strcmp(argv[arg], "-a") == 0 ||
                strcmp(argv[arg], "-w") == 0 ||
                strcmp(argv[arg], "-l") == 0)
                arg++;
            continue;
        }

        FILE *f = fopen(argv[arg], "r");
        if (!f) {
            fprintf(stderr, "Erreur: impossible d'ouvrir '%s'\n", argv[arg]);
            continue;
        }

        if (strcmp(choix_algo, "algo1") == 0) {
            histogramme occ;
            InitHist(&occ, &infomem);
            FileReader(f, &occ, &infomem);
            if (showres) {TopNmot(&occ, nbr, &infomem);}
            if (logres) {Ecrit_resultats_hist(fres, occ, nbr);}
            FreeHistogramme(&occ, &infomem);
            if (showperf) {showPerfs(infomem);}
            if (logperf) {Ecrit_performances_hist(fperf, &infomem, Compte_mot_hist(occ));}

        } else if (strcmp(choix_algo, "algo2") == 0) {
            histogramme occ;
            InitHist(&occ, &infomem);
            FileReader(f, &occ, &infomem);
            TrieHistogramme(&occ, &infomem);
            if (showres) {AfficherHistogramme(occ, nbr);}
            if (logres) {Ecrit_resultats_hist(fres, occ, nbr);}
            FreeHistogramme(&occ, &infomem);
            if (showperf) {showPerfs(infomem);}
            if (logperf) {Ecrit_performances_hist(fperf, &infomem, Compte_mot_hist(occ));}

        } else if (strcmp(choix_algo, "algo3") == 0) {
            Liste lst = NULL;
            Algo_lst_chaine(f, &lst, &infomem);
            if (showres) {Affiche_n_liste_chaine(&lst, nbr);}
            if (logres) {Ecrit_resultats(fres, &lst, nbr);}
            Free_liste(&lst, &infomem);
            if (showperf) {showPerfs(infomem);}
            if (logperf) {Ecrit_performances(fperf, &infomem, Compte_mot(&lst));}

        } else {
            fprintf(stderr, "Algorithme inconnu\n");
            fclose(f);
            break;
        }

        fclose(f);
    }

    if (fres) fclose(fres);
    if (fperf) fclose(fperf);

    return 0;
}
