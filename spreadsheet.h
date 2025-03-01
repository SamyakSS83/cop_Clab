#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include <stddef.h>
#include "stack.h"
#include "linked_list.h"

typedef struct Spreadsheet {
    int rows;
    int cols;
    Cell **cells;
    int view_row;
    int view_col;
} Spreadsheet;

#ifdef __cplusplus
extern "C" {
#endif
void safe_strcpy(char *dest, size_t dest_size, const char *src);
Spreadsheet *spreadsheet_create(int rows, int cols);
void destroySpreadsheet (Spreadsheet*sheet);

char* spreadsheet_col_to_letter(int col, char *buffer, size_t size);
int spreadsheet_letter_to_col(const char *letters);
int col_to_index(const char *col);
void index_to_col(int index, char *col);
int find_depends(const char *formula, Spreadsheet *sheet, int *r1, int *r2, int *c1, int *c2, int *range_bool);

char* spreadsheet_get_cell_name( int row, int col, char *buffer, size_t size);
int spreadsheet_parse_cell_name(const Spreadsheet *sheet, const char *cell_name, int *out_row, int *out_col);
int isNumeric(const char *str);

int spreadsheet_evaluate_function(Spreadsheet *sheet, const char *func, const char *args, Cell* cell,const char*expr);
int spreadsheet_evaluate_expression(Spreadsheet *sheet, const char *expr, Cell* cell);
void v_inorder_traversal_helper(OrderedSetNode *node, char **array, int *index);
int count_nodes(OrderedSetNode *node);
void v_orderedset_collect_keys(OrderedSet *set, char ***array, int *size);
int rec_find_cycle_using_stack(Spreadsheet*sheet, int r1,int r2 ,int c1,int c2,int range_bool, OrderedSet *visited, Node **top);
int first_step_find_cycle(Spreadsheet *sheet, char *cell_name, int r1,int r2 ,int c1,int c2,int range_bool);

void spreadsheet_set_cell_value(Spreadsheet *sheet, char *cell_name, const char *formula, char *status_out, size_t status_size);
void spreadsheet_display(Spreadsheet *sheet);

int is_valid_command(Spreadsheet *sheet, char **cell_name, char **formula);
#ifdef __cplusplus
}
#endif

#endif // SPREADSHEET_H
