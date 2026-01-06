#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gererMem.h"

int EstInterdit(Curse *lst, const char *mot) {
    while (lst) {
        if (strcmp(lst->mot, mot) == 0)
            return 1;
        lst = lst->suiv;
    }
    return 0;
}

void *myMalloc(size_t size, InfoMem *infoMem) {
    void* new_malloc = (void*) malloc(size);
    if (!new_malloc) {
        return NULL;
    }
    infoMem->cumul_alloc += size;
    // Si la taille acctuelle de mémoire alloué est plus grande que le max, on le modifie
    size_t new_mem = infoMem->cumul_alloc - infoMem->cumul_desalloc;
    if (new_mem > infoMem->max_alloc) {
        infoMem->max_alloc = new_mem;
    }
    return new_malloc;
}

void *myRealloc(void *ptr, size_t new_size, InfoMem *infoMem, size_t old_size) {
    void* new_realloc = (void*) realloc(ptr, new_size);
    if (!new_realloc) {
        return NULL;
    }
    // Si realloc ne fais pas de déplacement de données, les pointeur sont les même
    if (ptr == new_realloc) {
        // Si on agrandi l'espace 
        if (new_size > old_size) {
            infoMem->cumul_alloc += new_size - old_size;
            // Si la taille acctuelle de mémoire alloué est plus grande que le max, on le modifie
            size_t new_mem = infoMem->cumul_alloc - infoMem->cumul_desalloc;
            if (new_mem > infoMem->max_alloc) {
                infoMem->max_alloc = new_mem;
            }
        }
        // Si on rétréci l'espace
        else {
            infoMem->cumul_desalloc += old_size - new_size;
        }
    }
    // Si realloc déplace les données
    else { 
        infoMem->cumul_desalloc += old_size; 
        infoMem->cumul_alloc += new_size;
        // Si la taille acctuelle de mémoire alloué est plus grande que le max, on le modifie
        size_t new_mem = infoMem->cumul_alloc - infoMem->cumul_desalloc; 
        if (new_mem > infoMem->max_alloc) { 
            infoMem->max_alloc = new_mem; 
        } 
    }
    return new_realloc;
}

void myFree(void *ptr, InfoMem *infoMem, size_t old_size) {
    free(ptr);
    infoMem->cumul_desalloc += old_size;
}
