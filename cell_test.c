#include "cell.h"
#include "orderedset.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Function to print a key
void print_key(const char *key) {
    printf("%s ", key);
}

// Helper function to count elements during traversal
int dependent_count = 0;
void count_dependent(const char *key) {
    dependent_count++;
    print_key(key);
}

// Helper function to safely add a dependent
void add_dependent(Cell *cell, const char *dependent_key) {
    if (cell && cell->dependents && dependent_key) {
        orderedset_insert(cell->dependents, dependent_key);
    }
}

int main() {
    printf("=== Cell Test Suite ===\n\n");
    
    printf("Test 1: Basic cell creation\n");
    Cell *cell = cell_create(1, 1);
    assert(cell != NULL);
    assert(cell->row == 1);
    assert(cell->col == 1);
    assert(cell->value == 0);
    assert(cell->error == 0);
    assert(cell->formula == NULL);
    assert(cell->dependents != NULL);
    printf("Cell created at position (%d,%d) - PASS\n\n", cell->row, cell->col);
    
    printf("Test 2: Cell value modification\n");
    cell->value = 100;
    assert(cell->value == 100);
    printf("Cell value set to %d - PASS\n\n", cell->value);
    
    printf("Test 3: Cell formula assignment\n");
    char *formula = "=B1+C2";
    cell->formula = malloc(strlen(formula)+1);
    strcpy(cell->formula, formula);
    assert(cell->formula != NULL);
    assert(strcmp(cell->formula, "=B1+C2") == 0);
    printf("Cell formula set to \"%s\" - PASS\n\n", cell->formula);
    
    printf("Test 4: Error flag\n");
    cell->error = 1;
    assert(cell->error == 1);
    printf("Cell error flag set - PASS\n\n");
    
    printf("Test 5: Managing dependents\n");
    add_dependent(cell, "B1");
    add_dependent(cell, "C2");
    add_dependent(cell, "D3");
    
    assert(orderedset_contains(cell->dependents, "B1") == 1);
    assert(orderedset_contains(cell->dependents, "C2") == 1);
    assert(orderedset_contains(cell->dependents, "D3") == 1);
    assert(orderedset_contains(cell->dependents, "E4") == 0);
    
    dependent_count = 0;
    printf("Dependents of cell: ");
    orderedset_inorder_traversal(cell->dependents, count_dependent);
    printf("\nDependent count: %d (Expected: 3) - %s\n\n", 
           dependent_count, dependent_count == 3 ? "PASS" : "FAIL");
    assert(dependent_count == 3);
    
    printf("Test 6: Removing dependents\n");
    orderedset_remove(cell->dependents, "C2");
    assert(orderedset_contains(cell->dependents, "C2") == 0);
    dependent_count = 0;
    printf("Dependents after removal: ");
    orderedset_inorder_traversal(cell->dependents, count_dependent);
    printf("\nDependent count: %d (Expected: 2) - %s\n\n", 
           dependent_count, dependent_count == 2 ? "PASS" : "FAIL");
    assert(dependent_count == 2);
    
    printf("Test 7: Creating multiple cells\n");
    Cell *cell2 = cell_create(2, 3);
    assert(cell2 != NULL);
    printf("Cell 2 created at position (%d,%d)\n", cell2->row, cell2->col);
    assert(cell2->row == 2 && cell2->col == 3);
    
    add_dependent(cell2, "A1");
    add_dependent(cell2, "X10");
    
    // Verify each cell has its own dependencies
    assert(orderedset_contains(cell->dependents, "B1") == 1);
    assert(orderedset_contains(cell2->dependents, "B1") == 0);
    assert(orderedset_contains(cell->dependents, "A1") == 0);
    assert(orderedset_contains(cell2->dependents, "A1") == 1);
    printf("Each cell maintains its own dependencies - PASS\n\n");
    
    printf("Test 8: Memory management\n");
    size_t cell_size = sizeof(Cell);
    printf("Size of Cell struct: %zu bytes\n", cell_size);
    
    cell_destroy(cell);
    cell_destroy(cell2);
    printf("Cells destroyed successfully\n\n");
    
    printf("All tests completed.\n");
    return 0;
}