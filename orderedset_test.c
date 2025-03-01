#include "orderedset.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void print_key(const char *key) {
    printf("%s ", key);
}

// Helper function to test if an element exists in the set
void assert_contains(OrderedSet *set, const char *key, int expected) {
    int result = orderedset_contains(set, key);
    printf("Contains %s: %d (Expected: %d) - %s\n", 
           key, result, expected, 
           result == expected ? "PASS" : "FAIL");
    assert(result == expected);
}

// Helper to count elements during traversal
int element_count = 0;
void count_element(const char *key) {
    element_count++;
    print_key(key);
}

int main() {
    printf("=== OrderedSet Test Suite ===\n\n");
    
    printf("Test 1: Basic creation and insertion\n");
    OrderedSet *set = orderedset_create();
    assert(set != NULL);
    
    orderedset_insert(set, "A1");
    orderedset_insert(set, "B2");
    orderedset_insert(set, "C3");
    
    assert_contains(set, "A1", 1);
    assert_contains(set, "B2", 1);
    assert_contains(set, "C3", 1);
    assert_contains(set, "D4", 0);
    
    printf("In-order traversal: ");
    orderedset_inorder_traversal(set, print_key);
    printf("\n\n");
    
    printf("Test 2: Duplicate insertion\n");
    orderedset_insert(set, "A1");  // Duplicates should be ignored
    
    // Count should still be 3
    element_count = 0;
    printf("Elements after duplicate insertion: ");
    orderedset_inorder_traversal(set, count_element);
    printf("\nCount: %d (Expected: 3) - %s\n\n", 
           element_count, element_count == 3 ? "PASS" : "FAIL");
    assert(element_count == 3);
    
    printf("Test 3: Removal\n");
    orderedset_remove(set, "B2");
    assert_contains(set, "B2", 0);
    assert_contains(set, "A1", 1);
    assert_contains(set, "C3", 1);
    
    element_count = 0;
    printf("Elements after removal: ");
    orderedset_inorder_traversal(set, count_element);
    printf("\nCount: %d (Expected: 2) - %s\n\n", 
           element_count, element_count == 2 ? "PASS" : "FAIL");
    assert(element_count == 2);
    
    printf("Test 4: Removing non-existent element\n");
    orderedset_remove(set, "X9");  // Should not crash
    element_count = 0;
    printf("Elements after attempted removal of non-existent item: ");
    orderedset_inorder_traversal(set, count_element);
    printf("\nCount: %d (Expected: 2) - %s\n\n", 
           element_count, element_count == 2 ? "PASS" : "FAIL");
    assert(element_count == 2);
    
    printf("Test 5: Complex insertion and removal (AVL balancing)\n");
    // Insert more elements to test tree balancing
    orderedset_insert(set, "D4");
    orderedset_insert(set, "E5");
    orderedset_insert(set, "F6");
    orderedset_insert(set, "G7");
    
    element_count = 0;
    printf("Elements after complex insertion: ");
    orderedset_inorder_traversal(set, count_element);
    printf("\nCount: %d (Expected: 6) - %s\n\n", 
           element_count, element_count == 6 ? "PASS" : "FAIL");
    assert(element_count == 6);
    
    // Remove nodes to test different rebalancing cases
    orderedset_remove(set, "A1");
    orderedset_remove(set, "C3");
    
    element_count = 0;
    printf("Elements after complex removal: ");
    orderedset_inorder_traversal(set, count_element);
    printf("\nCount: %d (Expected: 4) - %s\n\n", 
           element_count, element_count == 4 ? "PASS" : "FAIL");
    assert(element_count == 4);
    
    printf("Test 6: Clear\n");
    orderedset_clear(set);
    element_count = 0;
    printf("Elements after clear: ");
    orderedset_inorder_traversal(set, count_element);
    printf("\nCount: %d (Expected: 0) - %s\n\n", 
           element_count, element_count == 0 ? "PASS" : "FAIL");
    assert(element_count == 0);
    assert_contains(set, "D4", 0);
    
    printf("Test 7: Insert after clear\n");
    orderedset_insert(set, "Z26");
    assert_contains(set, "Z26", 1);
    
    printf("Test 8: Edge cases - empty strings\n");
    orderedset_insert(set, "");
    assert_contains(set, "", 1);
    
    element_count = 0;
    printf("Final elements: ");
    orderedset_inorder_traversal(set, count_element);
    printf("\nCount: %d (Expected: 2) - %s\n\n", 
           element_count, element_count == 2 ? "PASS" : "FAIL");
    assert(element_count == 2);
    
    printf("All tests completed.\n");
    orderedset_destroy(set);
    
    return 0;
}