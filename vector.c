#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"

// Initial capacity for the vector
#define INITIAL_CAPACITY 2

// Function to initialize the vector
void vector_init(Vector *vec) {
    vec->size = 0;
    vec->capacity = INITIAL_CAPACITY;
    vec->data = (char **)malloc(vec->capacity * sizeof(char *));
    if (!vec->data) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);

    }
}

// Function to resize the vector when full
static void vector_resize(Vector *vec, size_t new_capacity) {
    vec->capacity = new_capacity;
    vec->data = (char **)realloc(vec->data, vec->capacity * sizeof(char *));
    if (!vec->data) {
        perror("Failed to reallocate memory");
        exit(EXIT_FAILURE);
    }
}

// Function to add a string to the end of the vector
void vector_push_back(Vector *vec, const char *str) {
    if (vec->size == vec->capacity) {
        vector_resize(vec, vec->capacity * 2);
    }
    vec->data[vec->size] = strdup(str);
    vec->size++;
}

// Function to get a string at a specific index
char *vector_get(Vector *vec, size_t index) {
    if (index >= vec->size) {
        fprintf(stderr, "Index out of bounds\n");
        return NULL;
    }
    return vec->data[index];
}

// Function to set a string at a specific index
void vector_set(Vector *vec, size_t index, const char *str) {
    if (index >= vec->size) {
        fprintf(stderr, "Index out of bounds\n");
        return;
    }
    free(vec->data[index]); // Free existing memory
    vec->data[index] = strdup(str);
}

// Function to remove a string at a specific index
void vector_remove(Vector *vec, const char *str) {
    size_t index = vec->size; // Set to an invalid index initially

    // Find the index of the string to remove
    for (size_t i = 0; i < vec->size; i++) {
        if (strcmp(vec->data[i], str) == 0) {
            index = i;
            break;
        }
    }

    // If not found, return
    if (index == vec->size) {
        fprintf(stderr, "String not found in vector\n");
        return;
    }

    free(vec->data[index]); // Free the memory of the removed string

    // Shift remaining elements to the left
    for (size_t i = index; i < vec->size - 1; i++) {
        vec->data[i] = vec->data[i + 1];
    }

    vec->size--;

    // Resize down if the size is 1/2th of capacity
    if (vec->size > 0 && vec->size == vec->capacity / 2) {
        vector_resize(vec, vec->capacity / 2);
    }
}


// Function to get the current size of the vector
size_t vector_size(Vector *vec) {
    return vec->size;
}

// Function to free the memory allocated by the vector
void vector_free(Vector *vec) {
    for (size_t i = 0; i < vec->size; i++) {
        free(vec->data[i]);
    }
    free(vec->data);
    vec->size = 0;
    vec->capacity = 0;
}
