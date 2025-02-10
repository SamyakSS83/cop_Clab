// ordereddict.h
#ifndef ORDEREDDICT_H
#define ORDEREDDICT_H

#include "cell.h"

typedef struct OrderedDictNode {
    char *key;
    struct Cell *value;
    struct OrderedDictNode *left;
    struct OrderedDictNode *right;
    int height;
} OrderedDictNode;

typedef struct OrderedDict {
    OrderedDictNode *root;
} OrderedDict;

OrderedDict* ordereddict_create();
void ordereddict_insert(OrderedDict *dict, const char *key, struct Cell *value);
struct Cell* ordereddict_get(OrderedDict *dict, const char *key);
void ordereddict_destroy(OrderedDict *dict);
void ordereddict_inorder_traversal(OrderedDict *dict, void (*func)(const char*, struct Cell*));

#endif // ORDEREDDICT_H