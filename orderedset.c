// orderedset.c

#define _POSIX_C_SOURCE 200809L
#include "orderedset.h"

#include <stdlib.h>
#include <string.h> // Added for strdup
#include <stdio.h>
#include <ctype.h>
#include <strings.h>

// Helper functions
static int max_orderedset(int a, int b) {
    return (a > b) ? a : b;
}

static int height_node(OrderedSetNode *node) {
    if (node == NULL)
        return 0;
    return node->height;
}

static OrderedSetNode* right_rotate(OrderedSetNode *y) {
    OrderedSetNode *x = y->left;
    OrderedSetNode *T2 = x->right;
    
    // Perform rotation
    x->right = y;
    y->left = T2;
    
    // Update heights
    y->height = max_orderedset(height_node(y->left), height_node(y->right)) + 1;
    x->height = max_orderedset(height_node(x->left), height_node(x->right)) + 1;
    
    // Return new root
    return x;
}

static OrderedSetNode* left_rotate(OrderedSetNode *x) {
    OrderedSetNode *y = x->right;
    OrderedSetNode *T2 = y->left;
    
    // Perform rotation
    y->left = x;
    x->right = T2;
    
    // Update heights
    x->height = max_orderedset(height_node(x->left), height_node(x->right)) + 1;
    y->height = max_orderedset(height_node(y->left), height_node(y->right)) + 1;
    
    // Return new root
    return y;
}

static int get_balance(OrderedSetNode *node) {
    if (node == NULL)
        return 0;
    return height_node(node->left) - height_node(node->right);
}

static OrderedSetNode* orderedset_insert_node(OrderedSetNode* node, const char *key) {
    if (node == NULL) {
        OrderedSetNode* new_node = malloc(sizeof(OrderedSetNode));
        new_node->key = strdup(key);
        new_node->left = new_node->right = NULL;
        new_node->height = 1;
        return new_node;
    }
    
    int cmp = strcmp(key, node->key);
    if (cmp < 0)
        node->left = orderedset_insert_node(node->left, key);
    else if (cmp > 0)
        node->right = orderedset_insert_node(node->right, key);
    else // Duplicate keys not allowed
        return node;
    
    // Update height
    node->height = max_orderedset(height_node(node->left), height_node(node->right)) + 1;
    
    // Get balance factor
    int balance = get_balance(node);
    
    // Balance the tree
    // Left Left
    if (balance > 1 && strcmp(key, node->left->key) < 0)
        return right_rotate(node);
    
    // Right Right
    if (balance < -1 && strcmp(key, node->right->key) > 0)
        return left_rotate(node);
    
    // Left Right
    if (balance > 1 && strcmp(key, node->left->key) > 0) {
        node->left = left_rotate(node->left);
        return right_rotate(node);
    }
    
    // Right Left
    if (balance < -1 && strcmp(key, node->right->key) < 0) {
        node->right = right_rotate(node->right);
        return left_rotate(node);
    }
    
    return node;
}

OrderedSet* orderedset_create() {
    OrderedSet *set = malloc(sizeof(OrderedSet));
    set->root = NULL;
    return set;
}

void orderedset_insert(OrderedSet *set, const char *key) {
    set->root = orderedset_insert_node(set->root, key);
}

static int orderedset_contains_node(OrderedSetNode *node, const char *key) {
    if (node == NULL)
        return 0;
    int cmp = strcmp(key, node->key);
    if (cmp < 0)
        return orderedset_contains_node(node->left, key);
    else if (cmp > 0)
        return orderedset_contains_node(node->right, key);
    else
        return 1;
}

int orderedset_contains(OrderedSet *set, const char *key) {
    return orderedset_contains_node(set->root, key);
}

static void orderedset_destroy_node(OrderedSetNode *node) {
    if (node == NULL)
        return;
    orderedset_destroy_node(node->left);
    orderedset_destroy_node(node->right);
    free(node->key);
    free(node);
}

void orderedset_destroy(OrderedSet *set) {
    if (set == NULL)
        return;
    orderedset_destroy_node(set->root);
    free(set);
}

static void orderedset_inorder_traversal_helper(OrderedSetNode *node, void (*func)(const char*)) {
    if (node == NULL)
        return;
    orderedset_inorder_traversal_helper(node->left, func);
    func(node->key);
    orderedset_inorder_traversal_helper(node->right, func);
}

void orderedset_inorder_traversal(OrderedSet *set, void (*func)(const char*)) {
    orderedset_inorder_traversal_helper(set->root, func);
}

// void orderedset_print(OrderedSet *set) {
//     orderedset_inorder_traversal(set, puts);
// }


// Add the wrapper function
void print_node(const char *str) {
    puts(str);
}

void orderedset_print(OrderedSet *set) {
    // Use the wrapper function instead of puts
    orderedset_inorder_traversal(set, print_node);
}



// Helper function to find the minimum value node in a subtree
static OrderedSetNode* find_min(OrderedSetNode *node) {
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

// Helper function to remove a node from the set
static OrderedSetNode* remove_node(OrderedSetNode *root, const char *key) {
    if (root == NULL) return NULL;

    int cmp = strcmp(key, root->key);
    if (cmp < 0) {
        root->left = remove_node(root->left, key);
    } else if (cmp > 0) {
        root->right = remove_node(root->right, key);
    } else {
        // Node with only one child or no child
        if (root->left == NULL) {
            OrderedSetNode *temp = root->right;
            free(root->key);
            free(root);
            return temp;
        } else if (root->right == NULL) {
            OrderedSetNode *temp = root->left;
            free(root->key);
            free(root);
            return temp;
        }

        // Node with two children: Get the inorder successor (smallest in the right subtree)
        OrderedSetNode *temp = find_min(root->right);

        // Copy the inorder successor's content to this node
        free(root->key);
        root->key = strdup(temp->key);

        // Delete the inorder successor
        root->right = remove_node(root->right, temp->key);
    }

    root->height = max_orderedset(height_node(root->left), height_node(root->right)) + 1;
    int balance = get_balance(root);

    // Balance the tree
    if (balance > 1 && get_balance(root->left) >= 0)
        return right_rotate(root);
    if (balance > 1 && get_balance(root->left) < 0) {
        root->left = left_rotate(root->left);
        return right_rotate(root);
    }
    if (balance < -1 && get_balance(root->right) <= 0)
        return left_rotate(root);
    if (balance < -1 && get_balance(root->right) > 0) {
        root->right = right_rotate(root->right);
        return left_rotate(root);
    }
    
    return root;
}

void orderedset_remove(OrderedSet *set, const char *key) {
    set->root = remove_node(set->root, key);
}


void clear_node(OrderedSetNode *node) {
        if (node == NULL) return;
        clear_node(node->left);
        clear_node(node->right);
        free(node->key);
        free(node);
    }

void orderedset_clear(OrderedSet *set) {
    
    clear_node(set->root);
    set->root = NULL;

}
