#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>
#include <stdint.h>

// Define the vector structure
typedef struct {
    char **data;   // Array of char* strings
    int16_t size;   // Current number of elements
    int16_t capacity; // Maximum capacity before resizing
} Vector;

// Function prototypes
void vector_init(Vector *vec);
void vector_push_back(Vector *vec, const char *str);
char *vector_get(Vector *vec, size_t index);
void vector_set(Vector *vec, size_t index, const char *str);
void vector_remove(Vector *vec, const char *str);
void vector_free(Vector *vec);
size_t vector_size(Vector *vec);

#endif // VECTOR_H
