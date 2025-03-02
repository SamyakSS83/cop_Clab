#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include "cell.h"
#include "orderedset.h"

// Define the structure for a node
typedef struct Node_l {
    Cell* data;          // Pointer to a Cell
    struct Node_l* next;   // Pointer to the next node
} Node_l;

// Function prototypes
Node_l* createNode_l(Cell* data);
void freeList(Node_l** head);
void appendNode(Node_l** head, Cell* data);
void deleteNode(Node_l** head, Cell* data);
void displayList(Node_l* head);
void insertFront(Node_l** head, Cell* data);

#endif // LINKED_LIST_H

