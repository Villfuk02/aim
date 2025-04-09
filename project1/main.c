#include <stdio.h>
#include <malloc.h>
#include <assert.h>


void *xmemcpy(void* dest, const void *src, size_t n){
    for (size_t i = 0; i < n; ++i) {
        ((char*)dest)[i] = ((char*)src)[i];
    }
    return dest;
}

int main(void) {
    char txt[] ="Hello, World!\n";
    char *a = malloc(sizeof(txt));
    assert(a != NULL);
    xmemcpy(txt, a, sizeof(txt));

    free(a);
    return 0;
}

