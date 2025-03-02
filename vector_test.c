#include "vector.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

void print_element(const char *str) {
    printf("%s ", str);
}

// Helper function to assert vector size
void assert_size(Vector *vec, size_t expected) {
    size_t result = vector_size(vec);
    printf("Size: %zu (Expected: %zu) - %s\n", result, expected, result == expected ? "PASS" : "FAIL");
    assert(result == expected);
}

// Helper function to assert vector element at index
void assert_element(Vector *vec, size_t index, const char *expected) {
    char *result = vector_get(vec, index);
    int pass = (expected == NULL && result == NULL) || 
               (result != NULL && expected != NULL && strcmp(result, expected) == 0);
    
    printf("Element at %zu: %s (Expected: %s) - %s\n", 
           index, 
           result ? result : "NULL", 
           expected ? expected : "NULL", 
           pass ? "PASS" : "FAIL");
    
    assert(pass);
}

int main() {
    printf("=== Vector Test Suite ===\n\n");
    
    printf("Test 1: Initialization\n");
    Vector vec;
    vector_init(&vec);
    assert_size(&vec, 0);
    printf("\n");
    
    printf("Test 2: Basic Insertion\n");
    vector_push_back(&vec, "A1");
    vector_push_back(&vec, "B1");
    vector_push_back(&vec, "A5");
    assert_size(&vec, 3);
    assert_element(&vec, 0, "A1");
    assert_element(&vec, 1, "B1");
    assert_element(&vec, 2, "A5");
    printf("\n");
    
    printf("Test 3: Out-of-bounds Access\n");
    assert_element(&vec, 3, NULL);
    printf("\n");
    
    printf("Test 4: Updating Elements\n");
    vector_set(&vec, 1, "VVV879");
    assert_element(&vec, 1, "VVV879");
    printf("\n");
    
    printf("Test 5: Removing Elements\n");
    vector_remove(&vec, "A1");
    assert_size(&vec, 2);
    assert_element(&vec, 0, "VVV879");
    assert_element(&vec, 1, "A5");
    printf("\n");
    
    printf("Test 6: Removing Non-existent Element\n");
    vector_remove(&vec, "X9");
    assert_size(&vec, 2);
    printf("\n");
    
    printf("Test 7: Clearing Vector\n");
    vector_free(&vec);
    assert_size(&vec, 0);
    printf("\n");
    
    printf("Test 8: Insert After Clear\n");
    vector_init(&vec);
    vector_push_back(&vec, "Z26");
    assert_size(&vec, 1);
    assert_element(&vec, 0, "Z26");
    printf("\n");
    
    printf("All tests completed.\n");
    vector_free(&vec);
    
    return 0;
}
