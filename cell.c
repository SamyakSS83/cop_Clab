// cell.c
#include "cell.h"
#include <stdlib.h>
#include <string.h>

Cell* cell_create(int row, int col) {
    Cell *cell = malloc(sizeof(Cell));
    cell->row = row;
    cell->col = col;
    cell->value = 0;
    cell->error = 0;
    cell->formula = NULL;
    cell->dependencies = orderedset_create();
    cell->dependents = orderedset_create();
    return cell;
}

void cell_destroy(Cell *cell) {
    if (cell == NULL)
        return;
    if (cell->formula != NULL)
        free(cell->formula);
    orderedset_destroy(cell->dependencies);
    orderedset_destroy(cell->dependents);
    free(cell);
}