#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "stack.h"
#include "cell.h"
#include "orderedset.h"

void print_cell(Cell *cell) {
    printf("Cell(%d, %d) ", cell->row, cell->col);
}

int main() {
    printf("=== Stack Test Suite ===\n\n");
    
    printf("Test 1: Stack initialization\n");
    Node* stack = NULL;
    assert(isEmpty(stack));
    printf("Stack is empty - PASS\n\n");
    
    printf("Test 2: Pushing elements onto the stack\n");
    Cell *cell1 = cell_create(1, 2);
    push(&stack, cell1);
    assert(!isEmpty(stack));
    printf("Element pushed, stack is not empty - PASS\n\n");
    
    Cell *cell2 = cell_create(3, 4);
    push(&stack, cell2);
    printf("Another element pushed - PASS\n\n");
    
    printf("Test 3: Peeking top element\n");
    Cell *topCell = peek(stack);
    assert(topCell->row == 3 && topCell->col == 4);
    printf("Peek returned correct element - PASS\n\n");
    
    printf("Test 4: Popping elements\n");
    Cell *poppedCell = pop(&stack);
    assert(poppedCell->row == 3 && poppedCell->col == 4);
    cell_destroy(poppedCell);
    printf("First pop returned correct element - PASS\n\n");
    
    poppedCell = pop(&stack);
    assert(poppedCell->row == 1 && poppedCell->col == 2);
    cell_destroy(poppedCell);
    printf("Second pop returned correct element - PASS\n\n");
    
    printf("Test 5: Stack should be empty now\n");
    assert(isEmpty(stack));
    printf("Stack is empty - PASS\n\n");
    
    printf("Test 6: Popping from an empty stack\n");
    assert(pop(&stack) == NULL);
    printf("Pop on empty stack returned NULL - PASS\n\n");
    
    printf("All tests completed.\n");
    return 0;
}
