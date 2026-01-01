#ifndef GERERMEM_H
#define GERERMEM_H

#include <stdio.h>
#include <stdlib.h>


// Structure pour la gestion de la mémoire
typedef struct {
    size_t cumul_alloc;    // champ obligatoire : cumul de l’espace mémoire alloué
    size_t cumul_desalloc; // champ obligatoire : cumul de l’espace mémoire
                           // désalloué
    size_t max_alloc;      // pic d'allocation (diff max des deux cumuls)
} InfoMem;

void *myMalloc(size_t size, InfoMem *infoMem);

void *myRealloc(void *ptr, size_t new_size, InfoMem *infoMem, size_t old_size);

void myFree(void *ptr, InfoMem *infoMem, size_t old_size);

#endif