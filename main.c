#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cell.h"
#include "spreadsheet.h"
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // fprintf(stderr, "Welcome to the spreadsheet program\n");
    if(argc != 3) {
        // stderr is used for printing to console the error message : it does not buffer the output,immediate action
        fprintf(stderr, "Usage: %s <rows> <cols>\n", argv[0]);
        return 1;
    }
    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);
    if(rows < 1 || rows > 999 || cols < 1 || cols > 18278) {
        fprintf(stderr, "Error: Invalid dimensions\n");
        return 1;
    }
    double start_time = (double)time(NULL);
    // fprintf(stderr, "Before spreadsheet_create\n");
    Spreadsheet *sheet = spreadsheet_create(rows, cols);
    // fprintf(stderr, "After spreadsheet_create\n");
    double elapsed_time = 0.0;
    char status[64];
    strcpy(status, "ok");
    int show = 1;

    while(1) {
        // fprintf(stderr, "Displaying spreadsheet\n");
        if(show) {
            spreadsheet_display(sheet);
        }
        fflush(stdout);
        elapsed_time = (double)time(NULL) - start_time;
        printf("[%.1f] (%s) > ", elapsed_time, status);
        fflush(stdout);

        char command[50]; // if input is more that 256 show error
        if(!fgets(command, sizeof(command), stdin)) {
            // EOF or error
            break;
        }
        start_time = (double)time(NULL);
        // Strip newline
        command[strcspn(command, "\n")] = '\0';
        if(strlen(command) == 0) {
            strcpy(status, "invalid command");
            continue;
        }

        if(strcmp(command, "q") == 0) {
            break;
        } else if(strlen(command) == 1 &&
                  (command[0] == 'w' || command[0] == 'a' ||
                   command[0] == 's' || command[0] == 'd')) {
            // Move view
                // printf("$$%d",sheet->view_row);
            if(command[0] == 'w' && sheet->view_row > 0) {
                // printf("i am here");
                int new_view = sheet->view_row - 10;
                sheet->view_row = (new_view < 0) ? 0 : new_view;
            } else if(command[0] == 's' && sheet->view_row < sheet->rows - 19) {
                int max_row = (sheet->rows > 10) ? sheet->rows - 10 : 0;
                int new_view = sheet->view_row + 10;
                sheet->view_row = (new_view > max_row) ? max_row : new_view;
            } else if(command[0] == 'a' && sheet->view_col > 0) {
                int new_col = sheet->view_col - 10;
                sheet->view_col = (new_col < 0) ? 0 : new_col;
            } else if(command[0] == 'd' && sheet->view_col < sheet->cols - 19) {
                int max_col = (sheet->cols > 10) ? sheet->cols - 10 : 0;
                int new_col = sheet->view_col + 10;
                sheet->view_col = (new_col > max_col) ? max_col : new_col;
            }
            // elapsed_time = 0.0;
            strcpy(status, "ok");
        } else if(strcmp(command, "disable_output") == 0){
            show = 0;
            strcpy(status, "ok");
        } else if(strcmp(command, "enable_output") == 0){
            show = 1;
            strcpy(status, "ok");
        } else if(strncmp(command, "scroll_to", 9) == 0){
            // Parse scroll_to command like scroll_to A1
            const char *cell_name = command + 10;
            int row, col;
            if(!spreadsheet_parse_cell_name(sheet, cell_name, &row, &col)){
                strcpy(status, "invalid cell");
            } else {
                sheet->view_row = row - 1;
                sheet->view_col = col - 1;
                strcpy(status, "ok");
            }
        }
            else {
            // Parse cell assignment like A1=10
            char *equal_sign = strchr(command, '=');
            if(equal_sign) {
                *equal_sign = '\0';

                char *cell_name = command;
                char *formula = equal_sign + 1;
                if(!is_valid_command(sheet, &cell_name, &formula)) {

                    strcpy(status, "invalid command");
                } else {
                    // fprintf(stderr, "[DEBUG] Command: %s = %s\n", cell_name, formula);
                    spreadsheet_set_cell_value(sheet, cell_name, formula, status, sizeof(status));
                }
            } else {
                // elapsed_time = 0.0;
                strcpy(status, "invalid command");
            }
        }
    }
    destroySpreadsheet(sheet);
    return 0;
}
