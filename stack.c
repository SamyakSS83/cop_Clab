#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

// Function to create a new node
Node* createNode(Cell* data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// Function to push an element onto the stack
void push(Node** top, Cell* data) {
    Node* newNode = createNode(data);
    newNode->next = *top;
    *top = newNode;
    // printf("Pushed Cell(%d, %d) onto the stack.\n", data->row, data->col);
}

// Function to pop an element from the stack
Cell* pop(Node** top) {
    if (*top == NULL) {
        // printf("Stack underflow. Nothing to pop.\n");
        return NULL;
    }
    Node* temp = *top;
    Cell* poppedData = temp->data;
    *top = (*top)->next;
    free(temp);
    // printf("Popped Cell(%d, %d) from the stack.\n", poppedData->row, poppedData->col);
    return poppedData;
}

// Function to peek at the top element of the stack
Cell* peek(Node* top) {
    if (top == NULL) {
        // printf("Stack is empty.\n");
        return NULL;
    }
    return top->data;
}

// Function to check if the stack is empty
int isEmpty(Node* top) {
    return top == NULL;
}

// Function to display the stack
void displayStack(Node* top) {
    if (top == NULL) {
        // printf("Stack is empty.\n");
        return;
    }
    Node* temp = top;
    // printf("Stack (top to bottom): ");
    while (temp != NULL) {
        // printf("Cell(%d, %d) -> ", temp->data->row, temp->data->col);
        temp = temp->next;
    }
    // printf("NULL\n");
}
