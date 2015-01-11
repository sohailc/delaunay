#ifndef _ARRAY_H
#define _ARRAY_H

#define ARRAY_BUFFER_SIZE 10

typedef struct array
{
    int *data;
    int len;
    int *next;
} array;

void addToArray(array *a, int n);
void deleteIndexFromArray(array *a, int idxToDelete);
void deleteValueFromArray(array *a, int valueToDelete);
void printArray(array a);

#endif
