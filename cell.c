// cell.c
#include "cell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"

Cell* cell_create(int row, int col) {
    Cell *cell = malloc(sizeof(Cell));
    cell->row = row;
    cell->col = col;
    cell->value = 0;
    cell->error = 0;
    cell->formula = NULL;
    cell->container = 0;
    cell->dependents_initialised = 0;
    return cell;
}



void cell_destroy(Cell *cell) {
    if (cell == NULL)
        return;
    if (cell->formula != NULL)
        free(cell->formula);
    if(cell->container == 0 && cell->dependents_initialised == 1){
        // vector
        vector_free(cell->dependents.dependents_vector);
        free(cell->dependents.dependents_vector);
    }
    else{
        // orderedset
        orderedset_destroy(cell->dependents.dependents_set);
    }
    
    free(cell);
}

void cell_dep_insert(Cell *cell, const char *key){
    // fprintf(stderr, "inserting %s\n", key);
    if(cell->container == 0){
        // vector
        // fprintf(stderr, "inserting into vector1\n");
        if(cell->dependents_initialised == 0){
            cell->dependents_initialised = 1;
            cell->dependents.dependents_vector = malloc(sizeof(Vector));
            vector_init(cell->dependents.dependents_vector);
        }
        if(cell->dependents.dependents_vector->size>7){
            // fprintf(stderr, "converting to orderedset\n");
            // cell->container = 1;
            // cell->dependents.dependents_set = orderedset_create();
            OrderedSet* new_set = orderedset_create();
            for(int i=0;i<cell->dependents.dependents_vector->size;i++){
                orderedset_insert(new_set, cell->dependents.dependents_vector->data[i]);
            }
            orderedset_insert(new_set, key);
            if(cell->dependents_initialised == 1){
                vector_free(cell->dependents.dependents_vector);
                free(cell->dependents.dependents_vector);
            }
            
            cell->container = 1;
            cell->dependents.dependents_set = new_set;
        }
        else{
            // fprintf(stderr, "inserting into vector2\n");
            vector_push_back(cell->dependents.dependents_vector, key);
        }
        // fprintf(stderr, "row->%d, col-> %d, %d, %d\n", cell->row, cell->col, cell->dependents.dependents_vector->size, cell->dependents.dependents_vector->capacity);
    }
    else{
        // orderedset
        orderedset_insert(cell->dependents.dependents_set, key);
    }
}

void cell_dep_remove(Cell *cell, const char *key){
    if(cell->container == 0){
        // vector
        vector_remove(cell->dependents.dependents_vector, key);
    }
    else{
        // orderedset
        orderedset_remove(cell->dependents.dependents_set, key);
        //optional:convert set to vector when shrinking
    }
}