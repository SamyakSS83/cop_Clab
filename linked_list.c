#include <stdio.h>
#include <stdlib.h>
#include "linked_list.h"

// Function to create a new Node_l
Node_l* createNode_l(Cell* data) {
    Node_l* newNode_l = (Node_l*)malloc(sizeof(Node_l));
    if (!newNode_l) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    newNode_l->data = data;
    newNode_l->next = NULL;
    return newNode_l;
}

// Function to free the entire linked list
void freeList(Node_l** head) {
    Node_l* current = *head;
    Node_l* nextNode;

    while (current != NULL) {
        nextNode = current->next; // Store reference to next node
        free(current);            // Free current node
        current = nextNode;        // Move to next node
    }

    *head = NULL; // Set head to NULL to avoid dangling pointers
}


// Function to add a Node_l at the end of the list
void appendNode(Node_l** head, Cell* data) {
    Node_l* newNode_l = createNode_l(data);
    if (*head == NULL) {
        *head = newNode_l;
        return;
    }
    Node_l* temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newNode_l;
}

// Function to delete a Node_l by value
void deleteNode(Node_l** head, Cell* data) {
    if (*head == NULL) {
        // printf("List is empty.\n");
        return;
    }
    Node_l* temp = *head;
    Node_l* prev = NULL;

    // If the Node_l to be deleted is the head
    if (temp != NULL && temp->data->row == data->row && temp->data->col == data->col) {
        *head = temp->next;
        free(temp);
        // printf("Node_l with Cell(%d, %d) deleted.\n", data->row, data->col);
        return;
    }

    // Search for the Node_l to be deleted
    while (temp != NULL && (temp->data->row != data->row || temp->data->col != data->col)) {
        prev = temp;
        temp = temp->next;
    }

    // If the value is not in the list
    if (temp == NULL) {
        // printf("Node_l with Cell(%d, %d) not found.\n", data->row, data->col);
        return;
    }

    // Unlink the Node_l from the list
    prev->next = temp->next;
    free(temp);
    // printf("Node_l with Cell(%d, %d) deleted.\n", data->row, data->col);
}

// Function to display the list
void displayList(Node_l* head) {
    if (head == NULL) {
        // printf("List is empty.\n");
        return;
    }
    Node_l* temp = head;
    printf("Linked list: ");
    while (temp != NULL) {
        // printf("Cell(%d, %d) -> ", temp->data->row, temp->data->col);
        temp = temp->next;
    }
    // printf("NULL\n");
}


// Function to insert a Node_l at the front of the list
void insertFront(Node_l** head, Cell* data) {
    Node_l* newNode_l = createNode_l(data);
    newNode_l->next = *head;  // Point the new Node_l to the current head
    *head = newNode_l;        // Update head to the new Node_l
}
