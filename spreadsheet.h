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
static void safe_strcpy(char *dest, size_t dest_size, const char *src);
Spreadsheet *spreadsheet_create(int rows, int cols);
// void spreadsheet_get_row_col(const Spreadsheet *,int *r,int *c,const char *cell_name,size_t cell_size);

// Spreadsheet* spreadsheet_create(int rows, int cols);

char* spreadsheet_col_to_letter(int col, char *buffer, size_t size);
int spreadsheet_letter_to_col(const char *letters);
int col_to_index(const char *col);
void index_to_col(int index, char *col);


char* spreadsheet_get_cell_name( int row, int col, char *buffer, size_t size);
int spreadsheet_parse_cell_name(const Spreadsheet *sheet, const char *cell_name, int *out_row, int *out_col);
char **spreadsheet_parse_range(const Spreadsheet *sheet, const char *range_str, int *count);
int isNumeric(const char *str);


#ifdef __cplusplus
}
#endif

#endif // SPREADSHEET_H