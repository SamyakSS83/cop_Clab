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
static void safe_strcpy(char *dest, size_t dest_size, const char *src)
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
               // ordereddict_insert(sheet->cells, cell_name, new_cell);
           }
       }
       return sheet;
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
       while (cell_name[j] && isdigit((unsigned char)cell_name[j]))
           j++;
       if (cell_name[j] != '\0')
       {
           // fprintf(stderr, "[ERROR] Invalid cell name\n");
           *out_row = *out_col = -1;
           return 0;
       }
       *out_row = atoi(&cell_name[i]);
       if (*out_row > sheet->rows || *out_row < 0)
       {
           // fprintf(stderr, "[ERROR] Invalid row\n");
           *out_row = *out_col = -1;
           return 0;
       }
       // fprintf(stderr, "[DEBUG] Parsed cell name: %d %d\n", *out_row, *out_col);
       return 1;
   }
   
   /* ----------------
      Range (e.g. A1:B2)
      ---------------- */
   char **spreadsheet_parse_range(const Spreadsheet *sheet, const char *range_str, int *count)
   {
       // fprintf(stderr, "[DEBUG] Parsing range: %s\n", range_str);
       *count = 0;
       char *copy = strdup(range_str);
       char *colon = strchr(copy, ':');
       if (!colon)
       {
           free(copy);
           return NULL;
       }
       *colon = '\0';
       const char *start = copy;
       const char *end = colon + 1;
   
       int start_row, start_col, end_row, end_col;
       spreadsheet_parse_cell_name(sheet, start, &start_row, &start_col);
       spreadsheet_parse_cell_name(sheet, end, &end_row, &end_col);
       if (start_row > end_row || start_col > end_col)
       {
           free(copy);
           return NULL;
       }
       int total = (end_row - start_row + 1) * (end_col - start_col + 1);
       char **cells = (char **)malloc(sizeof(char *) * total);
   
       int idx = 0;
       for (int r = start_row; r <= end_row; r++)
       {
           for (int c = start_col; c <= end_col; c++)
           {
               char buf[64];
               spreadsheet_get_cell_name(r, c, buf, sizeof(buf));
               cells[idx] = strdup(buf);
               idx++;
           }
       }
       *count = total;
       free(copy);
       return cells;
   }
   
   int isNumeric(const char *str)
   {
       int i = 0;
   
       // Check if the input is empty
       if (str[0] == '\0')
           return 0;
   
       // Check each character
       for (; str[i] != '\0'; i++)
       {
           if (!isdigit(str[i]))
           {
               return 0; // Return false if a non-digit character is found
           }
       }
       return 1; // All characters are digits
   }
   