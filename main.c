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
    fprintf(stdout, "  %s [-n int] [-a algo1|algo2|algo3|…] [—help] [-r] [-w fileRes] [-p] [-l filePerf] [-curse] [fichier] [...]\n\n", fichier);
    fprintf(stdout, "DESCRIPTION:\n");
    fprintf(stdout, "  Affiche le nombre et les mots dans les fichiers fournis.\n");
    fprintf(stdout, "  Si aucun fichier n'est fourni, lit depuis l'entree standard.\n\n");
    fprintf(stdout, "OPTIONS:\n");
    fprintf(stdout, "  -n       choix du nombre de mots a afficher\n");
    fprintf(stdout, "  -a       choix de l'algorithme\n");
    fprintf(stdout, "  -help    affiche le synopsis\n");
    fprintf(stdout, "  -r       affiche le resultat\n");
    fprintf(stdout, "  -w       ecris le resultat dans un fichier\n");
    fprintf(stdout, "  -p       affiche les performances de l'algorithme\n");
    fprintf(stdout, "  -l       ecris les performances dans un fichier\n");
    fprintf(stdout, "  -curse   permet l'ajout de mots interdits qui ne seront pas pris en compte dans les comptages d'occurences\n");
}

void showPerfs(InfoMem i) {
    fprintf(stdout, "\n=== Statistiques mémoire ===\n");
    (i.cumul_temps != 0) ? fprintf(stdout, "Temps d'execution: %ld secondes\n", i.cumul_temps) : fprintf(stdout, "Temps d'execution instantanee\n");
    fprintf(stdout, "Mémoire allouée: %zu bytes (%.2f Mo)\n", i.cumul_alloc, i.cumul_alloc / (1024.0 * 1024.0));
    fprintf(stdout, "Mémoire libérée: %zu bytes (%.2f Mo)\n", i.cumul_desalloc, i.cumul_desalloc / (1024.0 * 1024.0));
    fprintf(stdout, "Pic d'allocation: %zu bytes (%.2f Mo)\n", i.max_alloc, i.max_alloc / (1024.0 * 1024.0));
}

Curse *AjoutCurse(Curse *lst, const char *mot, InfoMem *infomem) {
    size_t len = strlen(mot) + 1;
    Curse *n = malloc(sizeof(Curse));

    myMalloc(sizeof(Curse), infomem);
    n->mot = malloc(len);

    myMalloc(len, infomem);
    memcpy(n->mot, mot, len);
    n->suiv = lst;

    return n;
}

void FreeCurse(Curse *lst, InfoMem *infomem) {
    while (lst) {
        Curse *tmp = lst;
        lst = lst->suiv;

        myFree(tmp->mot, infomem, strlen(tmp->mot) + 1);

        myFree(tmp, infomem, sizeof(Curse));
    }
}


int main(int argc, const char *argv[]) {
    InfoMem infomem;
    InitInfoMem(&infomem);

    Curse *liste_curse = NULL;

    if (argc < 2) {
        histogramme occ;
        InitHist(&occ, &infomem);
        fprintf(stdout, "Lecture depuis l'entree standard...\n");
        FileReader(stdin, &occ, &infomem, liste_curse);
        FreeHistogramme(&occ, &infomem);
        if (liste_curse) {FreeCurse(liste_curse, &infomem);}
    return 0;
}


    int nbr = 10, help = 0, showres = 0, logres = 0, showperf = 0, logperf = 0, use_curse = 0;
    char *choix_algo = NULL;
    char *FileRes = NULL;
    char *FilePerf = NULL;

    FILE *fres = NULL;
    FILE *fperf = NULL;

    for (int arg = 1; arg < argc; arg++) {
        if (strcmp(argv[arg], "-n") == 0 && arg + 1 < argc) {nbr = atoi(argv[++arg]);}
        else if (strcmp(argv[arg], "-a") == 0 && arg + 1 < argc) {choix_algo = (char *)argv[++arg];}
        else if (strcmp(argv[arg], "-w") == 0 && arg + 1 < argc) { FileRes = (char *)argv[++arg]; logres = 1; }
        else if (strcmp(argv[arg], "-l") == 0 && arg + 1 < argc) { FilePerf = (char *)argv[++arg]; logperf = 1; }
        else if (strcmp(argv[arg], "-help") == 0) {help = 1;}
        else if (strcmp(argv[arg], "-r") == 0) {showres = 1;}
        else if (strcmp(argv[arg], "-p") == 0) {showperf = 1;}
        else if (strcmp(argv[arg], "-curse") == 0) {use_curse = 1;}
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

    if (use_curse) {
        char buffer[256];
        fprintf(stdout, "Entrez les mots interdits (Ctrl+D pour finir) :\n");
        while (fgets(buffer, sizeof(buffer), stdin)) {
            buffer[strcspn(buffer, "\n")] = 0;
            if (strlen(buffer) > 0) {
                liste_curse = AjoutCurse(liste_curse, buffer, &infomem);
            }
        }
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
            FileReader(f, &occ, &infomem, liste_curse);
            int nb_mot = Compte_mot_hist(occ);
            if (showres) {TopNmot(&occ, nbr, &infomem);}
            if (logres) {TrieHistogramme(&occ, &infomem); Ecrit_resultats_hist(fres, &occ, nbr, "algo1");}
            FreeHistogramme(&occ, &infomem);
            if (liste_curse) {FreeCurse(liste_curse, &infomem);}
            if (showperf) {showPerfs(infomem);}
            if (logperf) {Ecrit_perf_hist(fperf, &infomem, nb_mot, "algo1");}

        } else if (strcmp(choix_algo, "algo2") == 0) {
            histogramme occ;
            InitHist(&occ, &infomem);
            FileReader(f, &occ, &infomem, liste_curse);
            TrieHistogramme(&occ, &infomem);
            int nb_mot = Compte_mot_hist(occ);
            if (showres) {AfficherHistogramme(occ, nbr);}
            if (logres) {Ecrit_resultats_hist(fres, &occ, nbr, "algo2");}
            FreeHistogramme(&occ, &infomem);
            if (liste_curse) {FreeCurse(liste_curse, &infomem);}
            if (showperf) {showPerfs(infomem);}
            if (logperf) {Ecrit_perf_hist(fperf, &infomem, nb_mot, "algo2");;}

        } else if (strcmp(choix_algo, "algo3") == 0) {
            Liste lst = NULL;
            Algo_lst_chaine(f, &lst, &infomem, liste_curse);
            int nb_mot = Compte_mot(&lst);
            if (showres) {Affiche_n_liste_chaine(&lst, nbr);}
            if (logres) {Ecrit_resultats_lst(fres, &lst, nbr, "algo3");}
            Free_liste(&lst, &infomem);
            if (liste_curse) {FreeCurse(liste_curse, &infomem);}
            if (showperf) {showPerfs(infomem);}
            if (logperf) {Ecrit_performances_lst(fperf, &infomem, nb_mot, "algo3");}

        } else {
            fprintf(stderr, "Algorithme inconnu\n");
            fclose(f);
            break;
        }

        fclose(f);
    }
    if (fres) {fclose(fres);}
    if (fperf) {fclose(fperf);}

    return 0;
}
