#define _POSIX_C_SOURCE 200809L
#define STACK_SIZE 2048
#include "stack.h"
#include "linked_list.h"
#include "spreadsheet.h"
#include "cell.h"
#include "orderedset.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <regex.h>

// Helper for safer string copy
void safe_strcpy(char *dest, size_t dest_size, const char *src)
{
    if (!dest || dest_size == 0)
        return;
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
}


/* ----------------
   Create/Destroy
   ---------------- */
Spreadsheet *spreadsheet_create(int rows, int cols)
{
    // fprintf(stderr, "[DEBUG] Creating spreadsheet: %d rows, %d cols\n", rows, cols);
    Spreadsheet *sheet = (Spreadsheet *)malloc(sizeof(Spreadsheet));
    sheet->rows = rows;
    sheet->cols = cols;
    sheet->view_row = 0;
    sheet->view_col = 0;
    sheet->cells = (Cell **)malloc(rows * cols * (sizeof(Cell *)));
    if (sheet->cells == NULL)
    {
        free(sheet);
        fprintf(stderr, "Space exceeded\n");
    }

    // Initialize cells
    for (int r = 1; r <= rows; r++)
    {
        for (int c = 1; c <= cols; c++)
        {
            // char cell_name[64];
            // printf("%s",cell_name);
            // spreadsheet_get_cell_name( r, c, cell_name, sizeof(cell_name));
            // Cell *new_cell = cell_create(r, c);
            sheet->cells[cols * (r - 1) + (c - 1)] = cell_create(r, c);
            
            if (!sheet->cells[cols * (r - 1) + (c - 1)]) // Check if allocation failed
            {
                fprintf(stderr, "Memory allocation failed for cell (%d, %d)\n", r, c);

                // Free already allocated cells before exiting
                for (int rr = 1; rr <= r; rr++)
                {
                    for (int cc = 1; cc <= cols; cc++)
                    {
                        if (sheet->cells[cols * (rr - 1) + (cc - 1)])
                            cell_destroy(sheet->cells[cols * (rr - 1) + (cc - 1)]);
                    }
                }
                free(sheet->cells);
                free(sheet);
                return NULL;
            }

            // ordereddict_insert(sheet->cells, cell_name, new_cell);
        }
    }
    return sheet;
}

void destroySpreadsheet (Spreadsheet*sheet){
    int col = sheet->cols;
    for(int r=1;r<=sheet->rows;r++){
        for(int c=1;c<=sheet->cols;c++){
            cell_destroy(sheet->cells[col*(r-1)+(c-1)]);
        }
    }
    free(sheet->cells);
    free(sheet);
}
/* ----------------
   Column <-> Letter
   ---------------- */
char *spreadsheet_col_to_letter(int col, char *buffer, size_t size)
{
    // fprintf(stderr, "[DEBUG] Converting col to letter: %d\n", col);
    if (size == 0)
        return NULL;
    buffer[0] = '\0';
    char temp[64];
    int index = 0;

    while (col > 0)
    {
        col--;
        int remainder = col % 26;
        temp[index++] = (char)('A' + remainder);
        col /= 26;
    }
    temp[index] = '\0';

    // Reverse temp into buffer
    for (int i = 0; i < index; i++)
    {
        buffer[i] = temp[index - i - 1];
    }
    buffer[index] = '\0';
    return buffer;
}
// A goes to 1... 1 based indexing
int spreadsheet_letter_to_col(const char *letters)
{
    // fprintf(stderr, "[DEBUG] Converting letter to col: %s\n", letters);
    int result = 0;
    for (int i = 0; letters[i] != '\0'; i++)
    {
        result = result * 26 + (letters[i] - 'A' + 1);
    }
    return result;
}

int col_to_index(const char *col)
{
    int index = 0;
    while (*col)
    {
        index = index * 26 + (*col - 'A' + 1);
        col++;
    }
    return index - 1; // Convert to 0-based index
}

void index_to_col(int index, char *col)
{
    int i = 0;
    while (index >= 0)
    {
        col[i++] = 'A' + (index % 26);
        index = index / 26 - 1;
    }
    col[i] = '\0';

    // Reverse the string to get correct column name
    for (int j = 0; j < i / 2; j++)
    {
        char temp = col[j];
        col[j] = col[i - j - 1];
        col[i - j - 1] = temp;
    }
}
/* ----------------
   Cell Name Helpers
   ---------------- */
char *spreadsheet_get_cell_name(int row, int col, char *buffer, size_t size)
{
    // if (!sheet || !buffer)
    //     return NULL;
    char col_letters[64];
    spreadsheet_col_to_letter(col, col_letters, sizeof(col_letters));
    snprintf(buffer, size, "%s%d", col_letters, row);
    return buffer;
}

int spreadsheet_parse_cell_name(const Spreadsheet *sheet, const char *cell_name, int *out_row, int *out_col)
{
    // fprintf(stderr, "[DEBUG] Parsing cell name: %s\n", cell_name);
    // We find the boundary between letters and digits
    int i = 0;

    while (isalpha((unsigned char)cell_name[i]))
        i++;
    if (i == 0)
    {
        // fprintf(stderr, "[ERROR] Invalid cell name\n %s\n", cell_name);
        *out_row = *out_col = -1;
        return 0;
    }
    char letters[64];
    strncpy(letters, cell_name, i);
    letters[i] = '\0';
    *out_col = spreadsheet_letter_to_col(letters);

    if (*out_col > sheet->cols || *out_col < 0)
    {
        // fprintf(stderr, "[ERROR] Invalid column\n");
        *out_row = *out_col = -1;
        return 0;
    }
    // Check valid row
    if (cell_name[i] == '\0')
    {
        // fprintf(stderr, "[ERROR] Invalid cell name\n");
        *out_row = *out_col = -1;
        return 0;
    }
    int j = i;
    if(cell_name[j]=='0'){
        *out_row = *out_col = -1;
        return 0;
    }
    while (cell_name[j] && isdigit((unsigned char)cell_name[j]))
        j++;
    if (cell_name[j] != '\0')
    {
        // fprintf(stderr, "[ERROR] Invalid cell name\n");
        *out_row = *out_col = -1;
        return 0;
    }
    
    *out_row = atoi(&cell_name[i]);
    if (*out_row > sheet->rows || *out_row <= 0)
    {
        // fprintf(stderr, "[ERROR] Invalid row\n");
        *out_row = *out_col = -1;
        return 0;
    }
    // fprintf(stderr, "[DEBUG] Parsed cell name: %d %d\n", *out_row, *out_col);
    return 1;
}

int isNumeric(const char *str) {
    if (*str == '\0') return 0;  // Empty string is not an integer

    char *endptr;
    strtol(str, &endptr, 10);  // Convert string to long

    return (*endptr == '\0');  // If endptr points to '\0', it's a valid integer
}

/* ----------------
   Expression & Function
   ---------------- */
int spreadsheet_evaluate_function(Spreadsheet *sheet, const char *func, const char *args, Cell *cell,const char*expr)
{
    // fprintf(stderr, "[DEBUG] Evaluating function: %s(%s)\n", func, args);
    // SLEEP(value)
    if (strcasecmp(func, "SLEEP") == 0)
    {
        int val;
        // case 1 , args is not cell reference
        if (isNumeric(args))
        {
            val = atoi(args);
            cell->error = 0;
        }

        else
        {
            int sign = 1;
            if (args[0] == '-')
            {
                sign = -1;
                args++;
            }
            else if (args[0] == '+')
            {
                args++;
            }
            // Cell *op = ordereddict_get(sheet->cells, args);
            int r_, c_;
            spreadsheet_parse_cell_name(sheet, args, &r_, &c_);
            Cell *op = sheet->cells[sheet->cols * (r_ - 1) + (c_ - 1)];
            if (!op)
            {
                fprintf(stderr, "cell not found\n");

                // no formula ever assigned to this cell . it must have zero value iff it's a valid cell
                // to be checked for a valid cell logic is inside extract already
                val = 0;
            }
            else
                val = sign * op->value;
            if (op->error)
            {
                cell->error = 1;
                return val;
            }
            else
            {
                cell->error = 0;
            }
        }
        // int val = spreadsheet_evaluate_expression(sheet, args);
        if(val>0){
            sleep((unsigned int)val);
        }

        return val;
    }

    // Evaluate range
    int count = 0;
    int r1,r2,c1,c2,range_bool;
    find_depends(expr,sheet,&r1,&r2,&c1,&c2,&range_bool);
    count = (r2-r1+1)*(c2-c1+1);
    // if (count == 0)
    // {
    //     if (cells)
    //         free(cells);
    //     return 0;
    // }
    int *values = (int *)malloc(sizeof(int) * count);
    // fprintf(stderr,"find_depends in which index %d %d %d %d \n",r1,r2,c1,c2);
    int k = 0;
    for (int i = r1; i <= r2; i++)
    {
        for(int j=c1;j<=c2;j++){
            // Cell *c = ordereddict_get(sheet->cells, cells[i]);
            // int r_, c_;
            // spreadsheet_parse_cell_name(sheet, cells[i], &r_, &c_);

            Cell *c = sheet->cells[sheet->cols * (i-1 ) + (j-1)];
            if (c && c->error)
            {
                cell->error = 1;
                // free(cells);
                free(values);
                return 0;
            }
            values[k] = c ? c->value : 0;
            k++;
            // fprintf(stderr,"values[i] is %d]\n",values[i]);

            // free(cells[i]);
        }
    }
    // free(cells);

    if (strcasecmp(func, "MIN") == 0)
    {
        int minv = values[0];
        for (int i = 1; i < count; i++)
            if (values[i] < minv)
                minv = values[i];
        free(values);
        cell->error = 0;
        return minv;
    }
    else if (strcasecmp(func, "MAX") == 0)
    {
        int maxv = values[0];
        for (int i = 1; i < count; i++)
            if (values[i] > maxv)
                maxv = values[i];
        free(values);
        cell->error = 0;
        return maxv;
    }
    else if (strcasecmp(func, "SUM") == 0)
    {
        int sumv = 0;
        for (int i = 0; i < count; i++)
            sumv += values[i];
        free(values);
        cell->error = 0;
        // fprintf(stderr,"returns what sum %d\n",sumv);
        return sumv;
    }
    else if (strcasecmp(func, "AVG") == 0)
    {
        int sumv = 0;
        for (int i = 0; i < count; i++)
            sumv += values[i];
        free(values);
        cell->error = 0;
        return (count == 0) ? 0 : (sumv / count);
    }
    else if (strcasecmp(func, "STDEV") == 0)
    {
        if (count < 2)
        {
            free(values);
            return 0;
        }
        int mean = 0;
        for (int i = 0; i < count; i++)
            mean += values[i];
        mean /= count;
        double variance = 0;
        for (int i = 0; i < count; i++)
        {
            double diff = values[i] - mean;
            variance += diff * diff;
        }
        variance /= (count);
        free(values);
        cell->error = 0;
        return (int)round(sqrt(variance));
    }

    free(values);
    // fprintf(stderr, "[ERROR] Unknown function: %s\n", func);
    return 0;
}

int spreadsheet_evaluate_expression(Spreadsheet *sheet, const char *expr, Cell *cell)
{
    if (!expr || strlen(expr) == 0)
        return 0;
    // fprintf(stderr, "[DEBUG] Evaluating expression: %s\n", expr);

    // Check for function call pattern: FUNC(...)
    regex_t funcRegex;
    regcomp(&funcRegex, "^([A-Za-z]+)\\((.*)\\)$", REG_EXTENDED);
    regmatch_t matches[3];
    if (regexec(&funcRegex, expr, 3, matches, 0) == 0)
    {
        char func[64], args[256];
        strncpy(func, expr + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
        func[matches[1].rm_eo - matches[1].rm_so] = '\0';
        strncpy(args, expr + matches[2].rm_so, matches[2].rm_eo - matches[2].rm_so);
        args[matches[2].rm_eo - matches[2].rm_so] = '\0';
        regfree(&funcRegex);
        return spreadsheet_evaluate_function(sheet, func, args, cell,expr);
    }
    regfree(&funcRegex);

    // Check if the expr is a cell reference (e.g. A1)
    regex_t cellRegex;
    regcomp(&cellRegex, "^[A-Za-z]+[0-9]+$", REG_EXTENDED);
    if (regexec(&cellRegex, expr, 0, NULL, 0) == 0)
    {
        // It's a cell reference
        // Cell *c = ordereddict_get(sheet->cells, expr);
        int r_, c_;
        spreadsheet_parse_cell_name(sheet, expr, &r_, &c_);
        Cell *c = sheet->cells[sheet->cols * (r_ - 1) + (c_ - 1)];
        if (!c)
            fprintf(stderr, "cell not found\n");
        cell->error = c->error;
        regfree(&cellRegex);
        return c ? c->value : 0;
    }
    regfree(&cellRegex);

    // Check for arithmetic
    // We'll do a naive approach: we find + - * / and split

    int temp = strlen(expr);
    int i = 0;
    int num1 = 0;
    int sign1 = 1;
    if (expr[i] == '+')
    {
        i++;
    }
    else if (expr[i] == '-')
    {
        i++;
        sign1 = -1;
    }
    if (expr[i] >= 48 && expr[i] <= 57)
    {
        int j = i;
        while (j < temp && isdigit(expr[j]))
        {
            num1 = num1 * 10 + (expr[j] - '0');
            j++;
        }
        i = j;
    }
    else if (expr[i] >= 65 && expr[i] <= 90)
    {
        // fprintf(stderr, "goes here1\n");
        int j = i;
        while (j < temp && expr[j] >= 65 && expr[j] <= 90)
        {
            j++;
        }
        int k = j;
        int row = 0;
        while (k < temp && isdigit(expr[k]))
        {
            row = row * 10 + expr[k] - '0';
            k++;
        }
        int temp_l = k - i;
        char cell_name_[10];
        strncpy(cell_name_, expr + i, temp_l);
        cell_name_[temp_l] = '\0';
        i = k;
        // fprintf(stderr, "i=%d", i);
        // Cell *c_ = ordereddict_get(sheet->cells, cell_name_);
        int r, c;
        spreadsheet_parse_cell_name(sheet, cell_name_, &r, &c);
        Cell *c_ = sheet->cells[sheet->cols * (r - 1) + (c - 1)];
        // if (!c_)
        //     fprintf(stderr, "c_ not in dict\n");
        if (c_->error)
        {
            cell->error = 1;
            return 0;
        }
        num1 = c_->value;
        // fprintf(stderr, "reaches till here\n");
    }
    num1 *= sign1;
    char operation;
    if (i < temp)
    {
        operation = expr[i];
    }
    else
    {
        cell->error = 0;
        return num1;
    }
    i++;
    int num2 = 0;
    int sign2 = 1;
    if (expr[i] == '+')
    {
        i++;
    }
    else if (expr[i] == '-')
    {
        sign2 = -1;
        i++;
    }
    if (expr[i] >= 48 && expr[i] <= 57)
    {
        int j = i;
        while (j < temp && isdigit(expr[j]))
        {
            num2 = num2 * 10 + (expr[j] - '0');
            j++;
        }
        i = j;
    }
    else if (expr[i] >= 65 && expr[i] <= 90)
    {
        // fprintf(stderr, "should go here next\n");
        int j = i;
        while (j < temp && expr[j] >= 65 && expr[j] <= 90)
        {
            j++;
        }
        int k = j;
        int row = 0;
        while (k < temp && isdigit(expr[k]))
        {
            row = row * 10 + expr[k] - '0';
            k++;
        }
        int temp_l = k - i;
        char cell_name_[10];
        strncpy(cell_name_, expr + i, temp_l);
        cell_name_[temp_l] = '\0';
        i = k;
        // Cell *c = ordereddict_get(sheet->cells, cell_name_);
        int r_, c_;
        spreadsheet_parse_cell_name(sheet, cell_name_, &r_, &c_);
        Cell *c = sheet->cells[sheet->cols * (r_ - 1) + (c_ - 1)];
        // if (!c)
        //     fprintf(stderr, "c not in dict\n");
        if (c->error)
        {
            cell->error = 1;
            return 0;
        }
        num2 = c->value;
    }
    num2 *= sign2;
    cell->error = 0;
    if (operation == '+')
    {
        return num1 + num2;
    }
    else if (operation == '-')
    {
        return num1 - num2;
    }
    else if (operation == '*')
    {
        return num1 * num2;
    }
    else if (operation == '/')
    {
        if (num2 == 0)
        {
            cell->error = 1;
            return 0;
        }
        return num1 / num2;
    }
    else
    {
        cell->error = 1;
        return -1;
    }
}

