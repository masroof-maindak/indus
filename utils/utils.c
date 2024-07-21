#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* copy_string(char* str) {
    char* copy = malloc(strlen(str) + 1);
    if (copy == NULL){
        perror("copy malloc() error");
        return NULL;
    }
    strcpy(copy, str);
    return copy;
}