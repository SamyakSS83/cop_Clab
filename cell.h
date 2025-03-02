// cell.h
#ifndef CELL_H
#define CELL_H

#include "orderedset.h"
#include <stdint.h>


typedef struct Cell {
    int16_t row;
    int16_t col;
    char error;
    int value;
    char *formula;
    OrderedSet *dependents;
} Cell;


// Function to create a cell
Cell* cell_create(int row, int col);

// Function to destroy a cell
void cell_destroy(Cell *cell);

#endif // CELL_H
