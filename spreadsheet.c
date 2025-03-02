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

int find_depends(const char *formula, Spreadsheet *sheet, int *r1, int *r2, int *c1, int *c2, int *range_bool)
{
    // fprintf(stderr, "find_depends start");
    *range_bool = 0;
    const char *ranges[] = {"MIN", "MAX", "AVG", "SUM", "STDEV"};
    // start with this or not :
    int i;
    for (i = 0; i < 5; i++)
    {
        if (strncmp(formula, ranges[i], strlen(ranges[i])) == 0)
        {
            *range_bool = 1;
            char *brack_sign = strchr(formula, '(');
            const char *new_formula = brack_sign + 1;
            int temp = strlen(new_formula);
            // printf("%s",new_formula);
            char only_range[100];
            strncpy(only_range, new_formula, temp - 1);
            only_range[temp - 1] = '\0';
            // printf("%s", only_range);
            int col1, row1, col2, row2;
            char col_start[10], col_end[10];
            if (sscanf(formula, "%[A-Z]%d:%[A-Z]%d", col_start, &row1, col_end, &row2) == 4)
            {
                col1 = col_to_index(col_start);
                col2 = col_to_index(col_end);
                row1--; // Convert to 0-based index
                row2--;

                if (col2 < col1)
                {
                    // fprintf(stderr, "Invalid formula format.\n");
                    return -1;
                }
                else if (col1 == col2 && row2 < row1)
                {
                    // fprintf(stderr, "Invalid formula format.\n");
                    return -1;
                }
                *r1 = row1;
                *r2 = row2;
                *c1 = col1;
                *c2 = col2;

                // printf("Extracting cells from range.. %s%d:%s%d\n", col_start, row1 + 1, col_end, row2 + 1);
                // printf("%d %d %d %d\n",visible_row_start,visible_col_start,visible_col_end,visible_row_end);

                // Iterate through the v*isible range
            }
        }
    }
    // it is not of the range form then :
    if (i >= 5)
    {
        // if (extract_cells(formula, new_depends) == -1)
        // {
        //     return -1;
        // }
        *r1 = -1;
        *r2 = -1;
        *c1 = -1;
        *c2 = -1;

        if (formula && strlen(formula) > 0)
        {
            // We'll do a simple regex to find references
            // this will only extract the cell references because only they are the ones to be sent to new_depends, it does nothing with the constant operands
            regex_t refRegex;
            regcomp(&refRegex, "([A-Za-z]+[0-9]+)", REG_EXTENDED);
            const char *p = formula;
            regmatch_t m;
            int cnt = 0;
            while (regexec(&refRegex, p, 1, &m, 0) == 0)
            {
                char ref[64];
                int len = m.rm_eo - m.rm_so;
                strncpy(ref, p + m.rm_so, len);
                ref[len] = '\0';
                p += m.rm_eo;
                // printf("%s\n", ref);
                // fprintf(stderr, "%s this is going to new_depends\n", ref);
                if (cnt == 0)
                {
                    spreadsheet_parse_cell_name(sheet, ref, r1, c1);
                }
                if (cnt == 1)
                {
                    spreadsheet_parse_cell_name(sheet, ref, r2, c2);
                }
                // orderedset_insert(new_depends, ref);
                cnt++;

                /* vanshika ---> do be done... check for out of boundaries cells and return -1 in that case*/
            }
            regfree(&refRegex);
        }
    }
    // new
    // fprintf(stderr, "find_depends end");
    // extract_visible_cells("MAX(A1:C5)", 0, 0, 10, 10);
    // extract_visible_cells("MAX(D3:G7)", 2, 3, 10, 10);
    return 0;
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

int rec_find_cycle_using_stack(Spreadsheet *sheet, int r1, int r2, int c1, int c2, int range_bool, OrderedSet *visited, Node **top)
{
    // orderedset_print(new_depends);
    while (!isEmpty(*top))
    {
        Cell *my_node = pop(top);
        // get the cell name from my_node->col and my_node->row
        char temp_col[10];
        index_to_col((my_node->col) - 1, temp_col);
        char temp_row[10];
        sprintf(temp_row, "%d", my_node->row);
        char *start_key = strcat(temp_col, temp_row);
        // fprintf(stderr, "key to be checked is %s", start_key);
        orderedset_insert(visited, start_key);
        // if (orderedset_contains(new_depends, start_key))
        if ((range_bool == 1 && (my_node->row >= r1 && my_node->row <= r2 && my_node->col >= c1 && my_node->col <= c2)) || (range_bool == 0 && ((my_node->row == r1 && my_node->col == c1) || (my_node->row == r2 && my_node->col == c2))))
            return 1;
        else
        {
            // Cell *my_node = ordereddict_get(dict, start_key);
            // now my set is my_node->dependents
            OrderedSet *my_set = my_node->dependents;
            // orderedset_print(my_set);
            // if (my_set->root == NULL)
            //     fprintf(stderr, "[DEBUG]16\n");
            char **keys = NULL;
            int size = 0;
            v_orderedset_collect_keys(my_set, &keys, &size);
            // fprintf(stderr, "[DEBUG]17\n");

            for (int i = 0; i < size; i++)
            {
                // If the key has not been visited, recurse to find a cycle
                if (!orderedset_contains(visited, keys[i]))
                {
                    // Cell *neighbour_node = ordereddict_get(dict, keys[i]);
                    int r_, c_;
                    spreadsheet_parse_cell_name(sheet, keys[i], &r_, &c_);
                    Cell *neighbour_node = sheet->cells[sheet->cols * (r_ - 1) + (c_ - 1)];
                    if (!neighbour_node)
                    {
                        // fprintf(stderr,"i think it should not be the case this should be in dict\n");
                        // if this changes nothing else changes so just check this cell alone
                        if (orderedset_contains(visited, keys[i]))
                            for(int x=0;x<size;x++) free(keys[x]);
                            if(keys) free(keys);
                            
                            return 1;
                    }
                    push(top, neighbour_node);
                }
            }
            for(int x=0;x<size;x++) free(keys[x]);
            if(keys) free(keys);
            // Rethink about this
            // Cell * c = ordereddict_get(sheet->cells);
        }
    }
    return 0;
}

void v_inorder_traversal_helper(OrderedSetNode *node, char **array, int *index)
{
    if (node == NULL)
    {
        return;
    }

    // Traverse the left subtree
    v_inorder_traversal_helper(node->left, array, index);

    // Visit the current node and add the key to the array
    array[*index] = strdup(node->key); // Assuming strdup is available for deep copying
    (*index)++;

    // Traverse the right subtree
    v_inorder_traversal_helper(node->right, array, index);
}

// Public function to collect all keys in an array
int count_nodes(OrderedSetNode *node)
{
    if (node == NULL)
        return 0;
    return count_nodes(node->left) + count_nodes(node->right) + 1;
}
void v_orderedset_collect_keys(OrderedSet *set, char ***array, int *size)
{

    // fprintf(stderr, "[DEBUG]18\n");
    // orderedset_print(set);
    if (set == NULL || set->root == NULL)
    {
        *array = NULL;
        *size = 0;
        return;
    }

    // First, find the number of nodes in the set
    *size = 0;

    *size = count_nodes(set->root);

    // Allocate memory for the array to store the keys;
    *array = malloc(sizeof(char *) * (*size));
    int index = 0;
    // Perform inorder traversal and collect the keys in the array
    v_inorder_traversal_helper(set->root, *array, &index);
}

int first_step_find_cycle(Spreadsheet *sheet, char *cell_name, int r1, int r2, int c1, int c2, int range_bool)
{

    // Cell *cell = ordereddict_get(sheet->cells, cell_name);
    int r_, c_;
    spreadsheet_parse_cell_name(sheet, cell_name, &r_, &c_);
    // Cell *cell = sheet->cells[sheet->cols * (r_ - 1) + (c_ - 1)];
    // new
    // fprintf(stderr, "lord vanshika\n");
    // Cell * start = cell_name;
    // fprintf(stderr, "[DEBUG]11\n");
    OrderedSet *visited = orderedset_create();
    // orderedset_print(new_depends);
    // Cell *node_of_start = ordereddict_get(sheet->cells, start);
    Cell *node_of_start = sheet->cells[sheet->cols * (r_ - 1) + (c_ - 1)];
    if (!node_of_start){
        orderedset_destroy(visited);
        return 0;
    }
        
         // start was never in rhs of any formula before there can't be cyclic dependency
    Node *top = createNode(node_of_start);
    // int xx = isEmpty(top);
    // if (!xx)
    //     fprintf(stderr, "created stack successfully\n");
    if (rec_find_cycle_using_stack(sheet, r1, r2, c1, c2, range_bool, visited, &top))
    {
        // fprintf(stderr, "correct");
        destroyStack(&top);
        orderedset_destroy(visited);
        return 1;
    }
    // fprintf(stderr, "[DEBUG]13\n");
    destroyStack(&top);
    orderedset_destroy(visited);
    return 0;
}

void remove_old_dependents(Spreadsheet *sheet, const char *cell_name)
{
    // Cell *cell = ordereddict_get(sheet->cells, cell_name);
    int r_, c_;
    spreadsheet_parse_cell_name(sheet, cell_name, &r_, &c_);
    Cell *cell = sheet->cells[sheet->cols * (r_ - 1) + (c_ - 1)];

    // OrderedSet *old_depends = orderedset_create();
    char *formula = cell->formula;
    if (formula == NULL)
    {
        return;
    }
    // fprintf(stderr, "[DEBUG]19\n");
    char *cpy_formula = malloc(strlen(formula) + 1);
    strcpy(cpy_formula, formula);

    // ###
    const char *ranges[] = {"MIN", "MAX", "AVG", "SUM", "STDEV"};
    // start with this or not :
    int i;
    for (i = 0; i < 5; i++)
    {
        // fprintf(stderr,"entering loop %d time\n",i);
        if (strncmp(cpy_formula, ranges[i], strlen(ranges[i])) == 0)
        {
            char *brack_sign = strchr(cpy_formula, '(');
            const char *new_formula = brack_sign + 1;
            int temp = strlen(new_formula);
            // printf("%s",new_formula);
            char only_range[100];
            strncpy(only_range, new_formula, temp - 1);
            only_range[temp - 1] = '\0';
            // printf("%s", only_range);

            int col1, row1, col2, row2;
            char col_start[10], col_end[10];

            // Parse range format like "A1:B3"
            if (sscanf(cpy_formula, "%[A-Z]%d:%[A-Z]%d", col_start, &row1, col_end, &row2) == 4)
            {
                col1 = col_to_index(col_start);
                col2 = col_to_index(col_end);
                row1--; // Convert to 0-based index
                row2--;
                clock_t start = clock();
                for (int r = row1; r <= row2; r++)
                {
                    for (int c = col1; c <= col2; c++)
                    {
                        char col_name[10];
                        index_to_col(c, col_name);
                        // printf("%s%d ", col_name, r + 1);
                        char result[100];
                        sprintf(result, "%s%d", col_name, r + 1);

                        // char *ref = result;
                        // printf("ref -> %s\n", ref);
                        // Also add cell_name to ref->dependents
                        // Cell *dep_cell = ordereddict_get(sheet->cells, ref);
                        // int r_, c_;
                        // spreadsheet_parse_cell_name(sheet, ref, &r_, &c_);
                        Cell *dep_cell = sheet->cells[sheet->cols * (r) + (c)];
                        if (!dep_cell)
                        {
                            // char * not_needed[64];
                            // int r, c;
                            // spreadsheet_parse_cell_name(sheet, ref, &r, &c);
                            // dep_cell = cell_create(r, c);
                            // ordereddict_insert(sheet->cells, ref, dep_cell);
                            fprintf(stderr, "cell not found\n");
                        }
                        orderedset_remove(dep_cell->dependents, cell_name);

                        // orderedset_insert(new_depends, result);
                    }
                    // printf("\n");
                }
                clock_t end = clock();
                double time_taken = (double)(end - start) / CLOCKS_PER_SEC;
                // printf("Time taken by loop in extract_visible_cell: %f\n", time_taken);
            }
            // else
            // {
            //     // fprintf(stderr, "Invalid formula format.\n");
            //     return -1;
            // }
            // return 1;

            // if (extract_visible_cells(only_range, new_depends) == -1)
            // {
            //     return -1;
            // }
            break;
        }
    }
    // it is not of the range form then :
    if (i >= 5)
    {
        // fprintf(stderr,"entering here i>=5 \n");
        int r1, r2, c1, c2, range_bool;
        find_depends(cpy_formula, sheet, &r1, &r2, &c1, &c2, &range_bool);
        // v_orderedset_collect_keys(new_depends, &key, &size);

        if (r1 > 0)
        {
            Cell *dep_cell = sheet->cells[sheet->cols * (r1 -1 ) + (c1 -1)];
            orderedset_remove(dep_cell->dependents, cell_name);
        }
        if (r2 > 0)
        {
            Cell *dep_cell = sheet->cells[sheet->cols * (r2 -1) + (c2 -1)];
            orderedset_remove(dep_cell->dependents, cell_name);
        }

        // printf("ref -> %s\n", ref);
        // Also add cell_name to ref->dependents
    }
    // ###
    free(cpy_formula);
    // Free old dependencies
    // orderedset_destroy(old_depends);
}

int v_spreadsheet_update_dependencies(Spreadsheet *sheet, const char *cell_name, const char *formula)
{
    // fprintf(stderr, "[DEBUG] Updating dependencies for %s\n", cell_name);
    // Cell *cell = ordereddict_get(sheet->cells, cell_name);
    int r_, c_;
    spreadsheet_parse_cell_name(sheet, cell_name, &r_, &c_);
    // Cell *cell = sheet->cells[sheet->cols * (r_ - 1) + (c_ - 1)];
    // Remove old dependencies
    remove_old_dependents(sheet, cell_name);

    // fprintf(stderr, "[DEBUG]14\n");

    const char *ranges[] = {"MIN", "MAX", "AVG", "SUM", "STDEV"};
    // start with this or not :
    int i;
    for (i = 0; i < 5; i++)
    {
        if (strncmp(formula, ranges[i], strlen(ranges[i])) == 0)
        {
            // fprintf(stderr, "%s\n", ranges[i]);
            char *brack_sign = strchr(formula, '(');
            const char *new_formula = brack_sign + 1;
            int temp = strlen(new_formula);
            // printf("%s",new_formula);
            char only_range[100];
            strncpy(only_range, new_formula, temp - 1);
            only_range[temp - 1] = '\0';
            // printf("%s", only_range);

            int col1, row1, col2, row2;
            char col_start[10], col_end[10];

            // Parse range format like "A1:B3"
            if (sscanf(only_range, "%[A-Z]%d:%[A-Z]%d", col_start, &row1, col_end, &row2) == 4)
            {
                col1 = col_to_index(col_start);
                col2 = col_to_index(col_end);
                row1--; // Convert to 0-based index
                row2--;

                if (col2 < col1)
                {
                    // fprintf(stderr, "Invalid formula format.\n");
                    return -1;
                }
                else if (col1 == col2 && row2 < row1)
                {
                    // fprintf(stderr, "Invalid formula format.\n");
                    return -1;
                }

                // printf("Extracting cells from range.. %s%d:%s%d\n", col_start, row1 + 1, col_end, row2 + 1);
                // printf("%d %d %d %d\n",visible_row_start,visible_col_start,visible_col_end,visible_row_end);

                // Iterate through the visible range
                clock_t start = clock();
                // fprintf(stderr, "range inside update depend..func %d %d \n %d %d\n", row1, col1, row2, col2);
                for (int r = row1; r <= row2; r++)
                {
                    for (int c = col1; c <= col2; c++)
                    {
                        // printf("ref -> %s\n", ref);
                        // Also add cell_name to ref->dependents
                        // Cell *dep_cell = ordereddict_get(sheet->cells, ref);
                        // int r_, c_;
                        // spreadsheet_parse_cell_name(sheet, ref, &r_, &c_);
                        Cell *dep_cell = sheet->cells[sheet->cols * (r) + (c)];
                        if (!dep_cell)
                        {
                            // char * not_needed[64];
                            // int r, c;
                            // spreadsheet_parse_cell_name(sheet, ref, &r, &c);
                            // dep_cell = cell_create(r, c);
                            // ordereddict_insert(sheet->cells, ref, dep_cell);
                            fprintf(stderr, "cell not found\n");
                        }
                        // fprintf(stderr, "%d %d %s\n", dep_cell->row, dep_cell->col, cell_name);
                        orderedset_insert(dep_cell->dependents, cell_name);

                        // orderedset_insert(new_depends, result);
                    }
                    // printf("\n");
                }
                clock_t end = clock();
                double time_taken = (double)(end - start) / CLOCKS_PER_SEC;
                // printf("Time taken by loop in extract_visible_cell: %f\n", time_taken);
            }
            // else
            // {
            //     // fprintf(stderr, "Invalid formula format.\n");
            //     return -1;
            // }
            // return 1;

            // if (extract_visible_cells(only_range, new_depends) == -1)
            // {
            //     return -1;
            // }
            // break;
        }
    }
    // it is not of the range form then :
    if (i >= 5)
    {

        int r1, r2, c1, c2, range_bool;
        find_depends(formula, sheet, &r1, &r2, &c1, &c2, &range_bool);
        // fprintf(stderr,"formula format in find_depends %s\n",formula);
        // fprintf(stderr, "range inside update depend..func %d %d \n %d %d\n", r1, c1, r2, c2);
        // v_orderedset_collect_keys(new_depends, &key, &size);

        if (r1 > 0)
        {
            Cell *dep_cell = sheet->cells[sheet->cols * (r1-1) + (c1-1)];
            orderedset_insert(dep_cell->dependents, cell_name);
        }
        if (r2 > 0)
        {
            Cell *dep_cell = sheet->cells[sheet->cols * (r2-1) + (c2-1)];
            orderedset_insert(dep_cell->dependents, cell_name);
        }

        // printf("ref -> %s\n", ref);
        // Also add cell_name to ref->dependents
    }
    // extract_visible_cells("MAX(A1:C5)", 0, 0, 10, 10);
    // extract_visible_cells("MAX(D3:G7)", 2, 3, 10, 10);
    return 0;

    // fprintf(stderr, "[DEBUG]15\n");

    // regfree(&refRegex);
}
Node_l *topo_sort(Spreadsheet *sheet, Cell *starting)
{
    // fprintf(stderr,"@ %d %d\n",starting->row,starting->col);
    Node_l *head = NULL;
    Node *st_top = createNode(starting);
    OrderedSet *visited = orderedset_create();
    char temp_col[10];
    index_to_col((starting->col) - 1, temp_col);
    char temp_row[10];
    sprintf(temp_row, "%d", starting->row);
    // char * start_key = strcat(temp_col,temp_row);
    // fprintf(stderr,"key to be checked is %s",start_key);
    // orderedset_insert(visited, start_key);
    // orderedset_insert(visited,starting);
    while (!isEmpty(st_top))
    {
        Cell *now = peek(st_top);
        // else {
        // push_withInt(&st_top,now.data,-1);
        // fprintf(stderr," ## %d %d \n",now->row,now->col);
        OrderedSet *my_set = now->dependents;
        // orderedset_print(my_set);
        // if (my_set->root == NULL)
        //     fprintf(stderr, "[DEBUG]16\n");
        char **keys = NULL;
        int size = 0;
        v_orderedset_collect_keys(my_set, &keys, &size);
        // int flag = 1;
        for (int i = 0; i < (size); i++)
        {
            char *ref = keys[i];
            // printf("ref -> %s\n", ref);
            // Also add cell_name to ref->dependents
            // Cell *dep_cell = ordereddict_get(sheet->cells, ref);
            int r_, c_;
            spreadsheet_parse_cell_name(sheet, ref, &r_, &c_);
            Cell *dep_cell = sheet->cells[sheet->cols * (r_ - 1) + (c_ - 1)];
            if (!dep_cell)
            {
                // char * not_needed[64];
                // int r,c;
                // spreadsheet_parse_cell_name(sheet,ref,&r,&c);
                // dep_cell = cell_create(r, c);
                // ordereddict_insert(sheet->cells, ref, dep_cell);
                fprintf(stderr, "cell not found\n");
            }
            if (!orderedset_contains(visited, ref))
            {
                push(&st_top, dep_cell);
                // orderedset_insert(visited,ref);
            }
        }
        if (now == peek(st_top))
        {
            pop(&st_top);
            insertFront(&head, now);
            char cell_name[64];
            spreadsheet_get_cell_name(now->row, now->col, cell_name, sizeof(cell_name));
            orderedset_insert(visited, cell_name);
        }
        // }
    for(int x=0;x<size;x++) free(keys[x]);
    if(keys) free(keys);
    }
    
    destroyStack(&st_top);
    orderedset_destroy(visited);
    return head;
}

void spreadsheet_set_cell_value(Spreadsheet *sheet, char *cell_name, const char *formula,
                                char *status_out, size_t status_size)
{
    // fprintf(stderr, "[DEBUG] Setting cell value: %s=*%s*\n", cell_name, formula);
    if (!sheet || !cell_name || !formula || *cell_name == '\0' || (*formula == '\0'))
    {
        // printf("went inside");
        safe_strcpy(status_out, status_size, "invalid args");
        return;
    }
    // fprintf(stderr, "[DEBUG]1\n");
    // printf("heyy");
    // Cell *cell = ordereddict_get(sheet->cells, cell_name);
    int r_, c_;
    spreadsheet_parse_cell_name(sheet, cell_name, &r_, &c_);
    Cell *cell = sheet->cells[sheet->cols * (r_ - 1) + (c_ - 1)];
    // no need to check out of bounds that is checked by is_valid_command
    // fprintf(stderr, "[DEBUG]4\n");
    // OrderedSet *new_depends = orderedset_create();
    char *cpy_formula = malloc(strlen(formula) + 1);
    strcpy(cpy_formula, formula);
    clock_t start_time = clock();
    int r1, r2, c1, c2;
    int range_bool;
    if (find_depends(cpy_formula, sheet, &r1, &r2, &c1, &c2, &range_bool) == -1)
    {
        free(cpy_formula);
        safe_strcpy(status_out, status_size, "invalid command");
        return;
    }
    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    // printf("Time taken by find_depends: %.6f seconds\n", elapsed_time);

    // fprintf(stderr, "[DEBUG]5\n");
    // orderedset_print(new_depends);
    start_time = clock();
    if (first_step_find_cycle(sheet, cell_name, r1, r2, c1, c2, range_bool))
    {
        // printf("Cycle Detected\n");
         free(cpy_formula);
        safe_strcpy(status_out, status_size, "Cycle Detected");
        return;
    }
    end_time = clock();
    elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    // printf("Time taken by first_step_find_cycle: %.6f seconds\n", elapsed_time);
    // fprintf(stderr, "[DEBUG]6\n");
    start_time = clock();
    v_spreadsheet_update_dependencies(sheet, cell_name, cpy_formula);
    end_time = clock();
    elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    // printf("Time taken by v_spreadsheet_update_dependencies: %.6f seconds\n", elapsed_time);
    // orderedset_print(cell->dependents);
    // fprintf(stderr, "[DEBUG]7\n");

    free(cell->formula);
    cell->formula = strdup(formula);
    // fprintf(stderr, "hereeee\n");
    Node_l *head = topo_sort(sheet, cell);
    // const char *v = "A1";
    // Cell *c1 = ordereddict_get(sheet->cells, v);
    // if (c1)
    //     orderedset_print(c1->dependents);
    // if (c1)
    //     fprintf(stderr, "A1 is inside dict\n");

    Node_l *curr = head;
    while (curr != NULL)
    {
        // fprintf(stderr, "cell row %d cell col %d\n", curr->data->row, curr->data->col);
        int x = spreadsheet_evaluate_expression(sheet, curr->data->formula, curr->data);
        curr->data->value = x;
        // fprintf(stderr, "%d\n\n\n", x);
        curr = curr->next;
    }
     free(cpy_formula);
    freeList(&head);
    safe_strcpy(status_out, status_size, "ok");
}
/* ----------------
   Display
   ---------------- */

void spreadsheet_display(Spreadsheet *sheet)
{
    if (!sheet)
        return;
    // fprintf(stderr, "[DEBUG] Displaying spreadsheet\n");
    int end_row = (sheet->view_row + 10 < sheet->rows) ? (sheet->view_row + 10) : sheet->rows;
    int end_col = (sheet->view_col + 10 < sheet->cols) ? (sheet->view_col + 10) : sheet->cols;

    // Print col headers
    printf("\t\t");
    for (int col = sheet->view_col + 1; col <= end_col; col++)
    {
        char buf[64];
        spreadsheet_col_to_letter(col, buf, sizeof(buf));
        printf("%s\t\t", buf);
    }
    printf("\n");

    // Print rows
    for (int row = sheet->view_row + 1; row <= end_row; row++)
    {
        printf("%d\t\t", row);
        for (int col = sheet->view_col + 1; col <= end_col; col++)
        {
            char cell_name[64];
            spreadsheet_get_cell_name(row, col, cell_name, sizeof(cell_name));
            // Cell *cell = ordereddict_get(sheet->cells, cell_name);
            Cell *cell = sheet->cells[sheet->cols * (row - 1) + (col - 1)];
            if (cell)
            {
                if (cell->error)
                {
                    printf("ERR\t\t");
                }
                else
                {
                    printf("%-16d", cell->value);
                }
            }
            else
            {
                printf("0\t\t");
            }
        }
        printf("\n");
    }
}

int is_valid_command(Spreadsheet *sheet, char **cell_name, char **formula)
{
    if (!sheet || !cell_name || !formula || !*cell_name || !*formula || **cell_name == '\0' || **formula == '\0')
    {
        return 0;
    }
    // Check if valid cell name
    int r, c;
    if (!spreadsheet_parse_cell_name(sheet, *cell_name, &r, &c))
    {
        return 0;
    }
    // Check if valid formula
    if (!*formula || strlen(*formula) == 0)
        return 0;
    // fprintf(stderr, "[DEBUG] Evaluating expression: %s\n", expr);

    // Check for function call pattern: FUNC(...)

    regex_t funcRegex;
    regcomp(&funcRegex, "^([A-Za-z]+)\\((.*)\\)$", REG_EXTENDED);
    regmatch_t matches[3];
    if (regexec(&funcRegex, *formula, 3, matches, 0) == 0)
    {
        char func[64], args0[256];
        char *args = args0;
        strncpy(func, *formula + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
        func[matches[1].rm_eo - matches[1].rm_so] = '\0';
        strncpy(args, *formula + matches[2].rm_so, matches[2].rm_eo - matches[2].rm_so);
        args[matches[2].rm_eo - matches[2].rm_so] = '\0';
        regfree(&funcRegex);
        if (strcasecmp(func, "SLEEP") == 0)
        {
            if(args[0]=='\0'){
                return 0;
            }
            // Check if args is a valid integer
            char *endPtr = NULL;
            long val = strtol(args, &endPtr, 10);
            if (endPtr && *endPtr == '\0')
            {
                return 1;
            }
            if (args[0] == '-' || args[0] == '+')
            {
                args++;
            }
            int r;
            int c;
            if (spreadsheet_parse_cell_name(sheet, args, &r, &c))
            {
                return 1;
            }
            return 0;
        }
        else
        {

            char *copy = strdup(args);
            char *colon = strchr(copy, ':');
            if (!colon)
            {
                free(copy);
                return 0;
            }
            *colon = '\0';
            const char *start = copy;
            const char *end = colon + 1;

            int start_row, start_col, end_row, end_col;
            if (!spreadsheet_parse_cell_name(sheet, start, &start_row, &start_col))
            {
                free(copy);
                return 0;
            }
            if (!spreadsheet_parse_cell_name(sheet, end, &end_row, &end_col))
            {
                free(copy);
                return 0;
            }
            if (start_row > end_row || start_col > end_col)
            {
                free(copy);
                return 0;
            }
            if (strcasecmp(func, "MIN") != 0 && strcasecmp(func, "MAX") != 0 && strcasecmp(func, "SUM") != 0 && strcasecmp(func, "AVG") != 0 && strcasecmp(func, "STDEV") != 0)
            {
                free(copy);
                return 0;
            }
            free(copy);
        }
        return 1;
    }
    regfree(&funcRegex);
    // Check if the expr is a cell reference (e.g. A1)
    regex_t cellRegex;
    regcomp(&cellRegex, "^[A-Za-z]+[0-9]+$", REG_EXTENDED);
    if (regexec(&cellRegex, *formula, 0, NULL, 0) == 0)
    {
        // It's a cell reference
        regfree(&cellRegex);
        int r, c;
        if (spreadsheet_parse_cell_name(sheet, *formula, &r, &c))
        {
            return 1;
        }
        return 0;
    }
    regfree(&cellRegex);

    // Check for arithmetic
    // We'll do a naive approach: we find + - * / and split
    char *expr = *formula;
    int temp = strlen(expr);
    int i = 0;
    char flag = 0;
    if (expr[i] == '+' || expr[i] == '-')
    {
        i++;
        flag = 1;
    }
    if (expr[i] >= 48 && expr[i] <= 57)
    {
        int j = i;
        while (j < temp && isdigit(expr[j]))
        {
            j++;
        }
        i = j;
        // fprintf(stderr, "1 is an int\n");
    }
    else if (flag == 0 && expr[i] >= 65 && expr[i] <= 90)
    {
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
        int r, c;
        if (!spreadsheet_parse_cell_name(sheet, cell_name_, &r, &c))
        {
            return 0;
        }
        i = k;
    }
    else
    {
        return 0;
    }
    // fprintf(stderr, "i = %d, temp = %d", i, temp);
    char operation;
    if (i < temp)
    {
        // fprintf(stderr, "I came here\n");
        operation = expr[i];
        if (operation != '+' && operation != '-' && operation != '*' && operation != '/')
        {
            return 0;
        }
    }
    else
    {
        // fprintf(stderr, "valid command A1=1\n");
        return 1;
    }
    i++;
    // fprintf(stderr, "%d", i);
    flag = 0;
    if (expr[i] == '+' || expr[i] == '-')
    {
        i++;
        flag = 1;
    }
    if (expr[i] >= 48 && expr[i] <= 57)
    {
        int j = i;
        while (j < temp && isdigit(expr[j]))
        {
            j++;
        }
        i = j;
    }
    else if (flag == 0 && expr[i] >= 65 && expr[i] <= 90)
    {
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
        int r, c;
        if (!spreadsheet_parse_cell_name(sheet, cell_name_, &r, &c))
        {
            return 0;
        }
        i = k;
    }
    else
    {
        return 0;
    }
    if (i == temp)
    {
        return 1;
    }
    else
    {
        return 0;
    }

    return 0;
}
