#ifndef STACK_H
#define STACK_H

// #include "spreadsheet.h"
#include "cell.h"
// #include "orderedset.h"
// #include "ordereddict.h"

// Define the structure for a stack node
typedef struct Node {
    Cell* data;          // Pointer to a Cell
    struct Node* next;   // Pointer to the next node
} Node;

// Function prototypes for stack operations
Node* createNode(Cell* data);
void push(Node** top, Cell* data);
Cell* pop(Node** top);
Cell* peek(Node* top);
int isEmpty(Node* top);
void displayStack(Node* top);

#endif // STACK_H

//Usage of stack

// #include <stdio.h>
// #include <stdlib.h>
// #include "stack.h"

// int main() {
//     Node* stack = NULL;

//     // Create some Cell objects
//     Cell* cell1 = (Cell*)malloc(sizeof(Cell));
//     cell1->row = 1;
//     cell1->col = 2;

//     Cell* cell2 = (Cell*)malloc(sizeof(Cell));
//     cell2->row = 3;
//     cell2->col = 4;

//     Cell* cell3 = (Cell*)malloc(sizeof(Cell));
//     cell3->row = 5;
//     cell3->col = 6;

//     // Push Cells onto the stack
//     push(&stack, cell1);
//     push(&stack, cell2);
//     push(&stack, cell3);

//     // Display the stack
//     displayStack(stack);

//     // Peek at the top of the stack
//     Cell* topCell = peek(stack);
//     if (topCell) {
//         printf("Top of stack: Cell(%d, %d)\n", topCell->row, topCell->col);
//     }

//     // Pop elements from the stack
//     Cell* poppedCell = pop(&stack);
//     if (poppedCell) {
//         free(poppedCell); // Free the popped Cell object
//     }

//     displayStack(stack);

//     // Free remaining allocated memory
//     while (!isEmpty(stack)) {
//         Cell* temp = pop(&stack);
//         free(temp);
//     }

//     displayStack(stack);

//     return 0;
// }
