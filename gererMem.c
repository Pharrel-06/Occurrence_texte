#include <stdio.h>
#include <stdlib.h>

typedef struct {
    size_t cumul_alloc;    // champ obligatoire : cumul de l’espace mémoire alloué
    size_t cumul_desalloc; // champ obligatoire : cumul de l’espace mémoire
                           // désalloué
    size_t max_alloc;      // pic d'allocation (diff max des deux cumuls)

    // d’autres champs qui sembleraient utiles
    // ...
} InfoMem;

void *myMalloc(size_t size, InfoMem *infoMem) {
    void* new_malloc = (void*) malloc(size);
    if (!new_malloc) {
        return NULL;
    }
    infoMem->cumul_alloc += size;
    infoMem->max_alloc += size;
    return new_malloc;
}

void *myRealloc(void *ptr, size_t new_size, InfoMem *infoMem, size_t old_size) {
    void* new_realloc = (void*) realloc(ptr, new_size);
    if (!new_realloc) {
        return NULL;
    }
    infoMem->cumul_desalloc += old_size;
    infoMem->cumul_alloc += new_size;
    infoMem->max_alloc += new_size;
    return new_realloc;
}

void myFree(void *ptr, InfoMem *infoMem, size_t old_size) {
    free(ptr);
    infoMem->cumul_desalloc += old_size;
}