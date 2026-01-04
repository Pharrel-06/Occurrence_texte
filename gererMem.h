#ifndef GERERMEM_H
#define GERERMEM_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Structure pour la gestion de la mémoire
typedef struct {
    size_t cumul_alloc;    // champ obligatoire : cumul de l’espace mémoire alloué
    size_t cumul_desalloc; // champ obligatoire : cumul de l’espace mémoire désalloué
    size_t max_alloc;      // pic d'allocation (diff max des deux cumuls)
    
    time_t temps_debut;
    time_t temps_fin;
    time_t cumul_temps;
} InfoMem;

void *myMalloc(size_t size, InfoMem *infoMem);

void *myRealloc(void *ptr, size_t new_size, InfoMem *infoMem, size_t old_size);

void myFree(void *ptr, InfoMem *infoMem, size_t old_size);

#endif