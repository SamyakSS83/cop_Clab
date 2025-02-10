// ordereddict.c
#define _POSIX_C_SOURCE 200809L
#include "ordereddict.h"
#include <stdlib.h>
#include <string.h> // Added for strdup
#include <stdio.h>
#include "cell.h"

// Helper functions
static int max_ordereddict(int a, int b) {
    return (a > b) ? a : b;
}

static int height_node_dict(OrderedDictNode *node) {
    if (node == NULL)
        return 0;
    return node->height;
}

static OrderedDictNode* right_rotate_dict(OrderedDictNode *y) {
    OrderedDictNode *x = y->left;
    OrderedDictNode *T2 = x->right;
    
    // Perform rotation
    x->right = y;
    y->left = T2;
    
    // Update heights
    y->height = max_ordereddict(height_node_dict(y->left), height_node_dict(y->right)) + 1;
    x->height = max_ordereddict(height_node_dict(x->left), height_node_dict(x->right)) + 1;
    
    // Return new root
    return x;
}

static OrderedDictNode* left_rotate_dict(OrderedDictNode *x) {
    OrderedDictNode *y = x->right;
    OrderedDictNode *T2 = y->left;
    
    // Perform rotation
    y->left = x;
    x->right = T2;
    
    // Update heights
    x->height = max_ordereddict(height_node_dict(x->left), height_node_dict(x->right)) + 1;
    y->height = max_ordereddict(height_node_dict(y->left), height_node_dict(y->right)) + 1;
    
    // Return new root
    return y;
}

static int get_balance_dict(OrderedDictNode *node) {
    if (node == NULL)
        return 0;
    return height_node_dict(node->left) - height_node_dict(node->right);
}

static OrderedDictNode* ordereddict_insert_node(OrderedDictNode* node, const char *key, struct Cell *value) {
    if (node == NULL) {
        OrderedDictNode* new_node = malloc(sizeof(OrderedDictNode));
        new_node->key = strdup(key);
        new_node->value = value;
        new_node->left = new_node->right = NULL;
        new_node->height = 1;
        return new_node;
    }
    
    int cmp = strcmp(key, node->key);
    if (cmp < 0)
        node->left = ordereddict_insert_node(node->left, key, value);
    else if (cmp > 0)
        node->right = ordereddict_insert_node(node->right, key, value);
    else { // Duplicate key, update value
        node->value = value;
        return node;
    }
    
    // Update height
    node->height = max_ordereddict(height_node_dict(node->left), height_node_dict(node->right)) + 1;
    
    // Get balance
    int balance = get_balance_dict(node);
    
    // Balance the tree
    // Left Left
    if (balance > 1 && strcmp(key, node->left->key) < 0)
        return right_rotate_dict(node);
    
    // Right Right
    if (balance < -1 && strcmp(key, node->right->key) > 0)
        return left_rotate_dict(node);
    
    // Left Right
    if (balance > 1 && strcmp(key, node->left->key) > 0) {
        node->left = left_rotate_dict(node->left);
        return right_rotate_dict(node);
    }
    
    // Right Left
    if (balance < -1 && strcmp(key, node->right->key) < 0) {
        node->right = right_rotate_dict(node->right);
        return left_rotate_dict(node);
    }
    
    return node;
}

OrderedDict* ordereddict_create() {
    OrderedDict *dict = malloc(sizeof(OrderedDict));
    dict->root = NULL;
    return dict;
}

void ordereddict_insert(OrderedDict *dict, const char *key, struct Cell *value) {
    dict->root = ordereddict_insert_node(dict->root, key, value);
}

static struct Cell* ordereddict_get_node(OrderedDictNode *node, const char *key) {
    if (node == NULL)
        return NULL;
    int cmp = strcmp(key, node->key);
    if (cmp < 0)
        return ordereddict_get_node(node->left, key);
    else if (cmp > 0)
        return ordereddict_get_node(node->right, key);
    else
        return node->value;
}

struct Cell* ordereddict_get(OrderedDict *dict, const char *key) {
    return ordereddict_get_node(dict->root, key);
}

static void ordereddict_destroy_node(OrderedDictNode *node) {
    if (node == NULL)
        return;
    ordereddict_destroy_node(node->left);
    ordereddict_destroy_node(node->right);
    free(node->key);
    // Assume Cell structs are managed elsewhere
    free(node);
}

void ordereddict_destroy(OrderedDict *dict) {
    if (dict == NULL)
        return;
    ordereddict_destroy_node(dict->root);
    free(dict);
}

static void ordereddict_inorder_traversal_helper(OrderedDictNode *node, void (*func)(const char*, struct Cell*)) {
    if (node == NULL)
        return;
    ordereddict_inorder_traversal_helper(node->left, func);
    func(node->key, node->value);
    ordereddict_inorder_traversal_helper(node->right, func);
}

void ordereddict_inorder_traversal(OrderedDict *dict, void (*func)(const char*, struct Cell*)) {
    ordereddict_inorder_traversal_helper(dict->root, func);
}