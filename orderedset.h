// orderedset.h
#ifndef ORDEREDSET_H
#define ORDEREDSET_H

typedef struct OrderedSetNode {
    char *key;
    struct OrderedSetNode *left;
    struct OrderedSetNode *right;
    int height;
} OrderedSetNode;

typedef struct OrderedSet {
    OrderedSetNode *root;
} OrderedSet;

OrderedSet* orderedset_create();
void orderedset_insert(OrderedSet *set, const char *key);
void orderedset_remove(OrderedSet *set, const char *key);
int orderedset_contains(OrderedSet *set, const char *key);
void orderedset_destroy(OrderedSet *set);
void orderedset_inorder_traversal(OrderedSet *set, void (*func)(const char*));
void orderedset_print(OrderedSet *set);
void orderedset_clear(OrderedSet *set);


#endif // ORDEREDSET_H