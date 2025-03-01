#include "linked_list.h"
#include "cell.h"
#include "orderedset.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    printf("=== Linked List Test Suite ===\n\n");

    printf("Test 1: Creating a node\n");
    Cell *cell1 = cell_create(1, 1);
    Node_l *node1 = createNode_l(cell1);
    int pass = (node1 != NULL && node1->data == cell1 && node1->next == NULL);
    printf("Node created with Cell(%d, %d) - %s\n\n", cell1->row, cell1->col, pass ? "PASS" : "FAIL");
    assert(pass);

    printf("Test 2: Appending nodes\n");
    Cell *cell2 = cell_create(2, 2);
    Cell *cell3 = cell_create(3, 3);
    Node_l *head = NULL;
    appendNode(&head, cell1);
    appendNode(&head, cell2);
    appendNode(&head, cell3);
    pass = (head != NULL && head->data == cell1 && head->next->data == cell2 && head->next->next->data == cell3);
    printf("Nodes appended correctly - %s\n\n", pass ? "PASS" : "FAIL");
    assert(pass);

    printf("Test 3: Deleting a node\n");
    deleteNode(&head, cell2);
    pass = (head->data == cell1 && head->next->data == cell3);
    printf("Node with Cell(%d, %d) deleted - %s\n\n", cell2->row, cell2->col, pass ? "PASS" : "FAIL");
    assert(pass);

    printf("Test 4: Inserting at front\n");
    Cell *cell4 = cell_create(4, 4);
    insertFront(&head, cell4);
    pass = (head->data == cell4 && head->next->data == cell1);
    printf("Node inserted at front with Cell(%d, %d) - %s\n\n", cell4->row, cell4->col, pass ? "PASS" : "FAIL");
    assert(pass);

    printf("Test 5: Freeing the list\n");
    freeList(&head);
    pass = (head == NULL);
    printf("Linked list freed successfully - %s\n\n", pass ? "PASS" : "FAIL");
    assert(pass);

    printf("All tests completed.\n");
    return 0;
}