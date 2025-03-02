// cell.h
#ifndef CELL_H
#define CELL_H

#include "orderedset.h"
#include <stdint.h>
#include "vector.h"


typedef struct Cell {
    int16_t row;
    int16_t col;
    char error;
    char container;
    char dependents_initialised;
    int value;
    char *formula;
    union Dependents{
        OrderedSet *dependents_set;
        Vector *dependents_vector;
    } dependents;
} Cell;


// Function to create a cell
Cell* cell_create(int row, int col);

// Function to destroy a cell
void cell_destroy(Cell *cell);

void cell_dep_insert(Cell *cell, const char *key);

void cell_dep_remove(Cell *cell, const char *key);

#endif // CELL_H
