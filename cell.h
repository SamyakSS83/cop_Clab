// cell.h
#ifndef CELL_H
#define CELL_H

#include "orderedset.h"

typedef struct Cell {
    int row;
    int col;
    int value;
    int error;
    char *formula;
    OrderedSet *dependencies;
    OrderedSet *dependents;
} Cell;

// Function to create a cell
Cell* cell_create(int row, int col);

// Function to destroy a cell
void cell_destroy(Cell *cell);

#endif // CELL_H