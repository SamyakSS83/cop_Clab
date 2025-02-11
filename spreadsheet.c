#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#define STACK_SIZE 2048
#include "stack.h"
#include "linked_list.h"
#include "spreadsheet.h"
#include "cell.h"
#include "orderedset.h"
#include "ordereddict.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <regex.h>

int spreadsheet_evaluate_function(Spreadsheet *sheet, const char *func, const char *args, Cell *cell)
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
            Cell *op = ordereddict_get(sheet->cells, args);
            if (!op)
            {
                // no formula ever assigned to this cell . it must have zero value iff it's a valid cell
                // to be checked for a valid cell logic is inside extract already
                val = 0;
            }
            else
                val = sign*op->value;
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
        sleep((unsigned int)val);

        return val;
    }

    // Evaluate range
    int count = 0;
    char **cells = spreadsheet_parse_range(sheet, args, &count);
    if (!cells || count == 0)
    {
        if (cells)
            free(cells);
        return 0;
    }
    int *values = (int *)malloc(sizeof(int) * count);
    for (int i = 0; i < count; i++)
    {
        Cell *c = ordereddict_get(sheet->cells, cells[i]);
        if (c && c->error)
        {
            cell->error = 1;
            free(cells);
            free(values);
            return 0;
        }
        values[i] = c ? c->value : 0;
        free(cells[i]);
    }
    free(cells);

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
        double mean = 0;
        for (int i = 0; i < count; i++)
            mean += values[i];
        mean /= count;
        double variance = 0;
        for (int i = 0; i < count; i++)
        {
            double diff = values[i] - mean;
            variance += diff * diff;
        }
        variance /= (count - 1);
        free(values);
        cell->error = 0;
        return (int)(sqrt(variance));
    }

    free(values);
    // fprintf(stderr, "[ERROR] Unknown function: %s\n", func);
    return 0;
}

int spreadsheet_evaluate_expression(Spreadsheet *sheet, const char *expr, Cell *cell)
{
    // if (!cell)
    //     fprintf(stderr, "cell is wrong\n\n");
    // fprintf(stderr, "%s\n", expr);
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
        return spreadsheet_evaluate_function(sheet, func, args, cell);
    }
    regfree(&funcRegex);

    // Check if the expr is a cell reference (e.g. A1)
    regex_t cellRegex;
    regcomp(&cellRegex, "^[A-Za-z]+[0-9]+$", REG_EXTENDED);
    if (regexec(&cellRegex, expr, 0, NULL, 0) == 0)
    {
        // It's a cell reference
        Cell *c = ordereddict_get(sheet->cells, expr);
        // if (!c)
        //     fprintf(stderr, "cell not in dictionary here");
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
        Cell *c_ = ordereddict_get(sheet->cells, cell_name_);
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
        Cell *c = ordereddict_get(sheet->cells, cell_name_);
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