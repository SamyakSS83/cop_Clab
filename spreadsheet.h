#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include "ordereddict.h"
#include <stddef.h>

typedef struct Spreadsheet {
    int rows;
    int cols;
    OrderedDict *cells;
    int view_row;
    int view_col;
} Spreadsheet;

#ifdef __cplusplus
extern "C" {
#endif
int spreadsheet_parse_cell_name(const Spreadsheet *sheet, const char *cell_name, int *out_row, int *out_col);
char** spreadsheet_parse_range(const Spreadsheet *sheet, const char *range_str, int *count);
int spreadsheet_evaluate_function(Spreadsheet *sheet, const char *func, const char *args, Cell* cell);
int spreadsheet_evaluate_expression(Spreadsheet *sheet, const char *expr, Cell* cell);

#ifdef __cplusplus
}
#endif

#endif // SPREADSHEET_H
