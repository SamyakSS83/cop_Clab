#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "spreadsheet.h"
#include "cell.h"
#include "orderedset.h"

// Helper function to check cell values
void assert_cell_value(Spreadsheet *sheet, const char *cell_name, int expected_value, int expected_error) {
    int row, col;
    spreadsheet_parse_cell_name(sheet, cell_name, &row, &col);
    Cell *cell = sheet->cells[sheet->cols * (row - 1) + (col - 1)];
    assert(cell != NULL);
    assert(cell->value == expected_value);
    assert(cell->error == expected_error);
    printf("✓ Cell %s has value %d and error status %d as expected\n", 
           cell_name, expected_value, expected_error);
}

// Helper to safely set a cell's formula
void set_cell(Spreadsheet *sheet, const char *cell_name, const char *formula) {
    char status[256];
    spreadsheet_set_cell_value(sheet, (char *)cell_name, formula, status, sizeof(status));
    // printf("Setting %s=%s: %s\n", cell_name, formula, status);
}

// Test spreadsheet creation and basic properties
void test_spreadsheet_create() {
    printf("\n====== Testing spreadsheet_create ======\n");
    Spreadsheet *sheet = spreadsheet_create(100, 100);
    assert(sheet != NULL);
    assert(sheet->rows == 100);
    assert(sheet->cols == 100);
    assert(sheet->cells != NULL);
    printf("✓ Spreadsheet created successfully with 100x100 dimensions\n");
    
    // Test cell initialization
    Cell *cell_a1 = sheet->cells[0]; // A1
    assert(cell_a1 != NULL);
    assert(cell_a1->row == 1);
    assert(cell_a1->col == 1);
    assert(cell_a1->value == 0);
    assert(cell_a1->formula == NULL);
    assert(cell_a1->error == 0);
    printf("✓ Cell A1 initialized correctly\n");
    
    // Test a cell in the middle
    Cell *cell_j10 = sheet->cells[9 * sheet->cols + 9]; // J10
    assert(cell_j10 != NULL);
    assert(cell_j10->row == 10);
    assert(cell_j10->col == 10);
    printf("✓ Cell J10 initialized correctly\n");
    
    // Test bottom-right cell
    Cell *cell_cv100 = sheet->cells[99 * sheet->cols + 99]; // CV100
    assert(cell_cv100 != NULL);
    assert(cell_cv100->row == 100);
    assert(cell_cv100->col == 100);
    printf("✓ Cell CV100 (bottom-right) initialized correctly\n");
    
    // Cleanup
    // TODO: Implement spreadsheet_destroy
     for (int r = 1; r <= 100; r++)
    {
        for (int c = 1; c <= 100; c++)
        {
            cell_destroy(sheet->cells[100*(r-1)+(c-1)]);

        }
    }

    free(sheet->cells);
    free(sheet);
}

// Test column/letter conversion functions
void test_column_letter_conversion() {
    printf("\n====== Testing column/letter conversion ======\n");
    
    // Column to letter tests
    char buffer[64];
    assert(strcmp(spreadsheet_col_to_letter(1, buffer, sizeof(buffer)), "A") == 0);
    printf("✓ Column 1 converts to 'A'\n");
    
    assert(strcmp(spreadsheet_col_to_letter(26, buffer, sizeof(buffer)), "Z") == 0);
    printf("✓ Column 26 converts to 'Z'\n");
    
    assert(strcmp(spreadsheet_col_to_letter(27, buffer, sizeof(buffer)), "AA") == 0);
    printf("✓ Column 27 converts to 'AA'\n");
    
    assert(strcmp(spreadsheet_col_to_letter(52, buffer, sizeof(buffer)), "AZ") == 0);
    printf("✓ Column 52 converts to 'AZ'\n");
    
    assert(strcmp(spreadsheet_col_to_letter(100, buffer, sizeof(buffer)), "CV") == 0);
    printf("✓ Column 100 converts to 'CV'\n");

    assert(strcmp(spreadsheet_col_to_letter(703, buffer, sizeof(buffer)), "AAA") == 0);
    printf("✓ Column 703 converts to 'AAA'\n");

    assert(strcmp(spreadsheet_col_to_letter(1404, buffer, sizeof(buffer)), "BAZ") == 0);
    printf("✓ Column 1404 converts to 'BAZ'\n");

    assert(strcmp(spreadsheet_col_to_letter(703, buffer, sizeof(buffer)), "AAA") == 0);
    printf("✓ Column 703 converts to 'AAA'\n");
    
    assert(strcmp(spreadsheet_col_to_letter(704, buffer, sizeof(buffer)), "AAB") == 0);
    printf("✓ Column 704 converts to 'AAB'\n");
    
    assert(strcmp(spreadsheet_col_to_letter(728, buffer, sizeof(buffer)), "AAZ") == 0);
    printf("✓ Column 728 converts to 'AAZ'\n");
    
    assert(strcmp(spreadsheet_col_to_letter(729, buffer, sizeof(buffer)), "ABA") == 0);
    printf("✓ Column 729 converts to 'ABA'\n");
    
    assert(strcmp(spreadsheet_col_to_letter(18278, buffer, sizeof(buffer)), "ZZZ") == 0);
    printf("✓ Column 18278 converts to 'ZZZ'\n");
    
    
    // Letter to column tests
    assert(spreadsheet_letter_to_col("A") == 1);
    printf("✓ 'A' converts to column 1\n");
    
    assert(spreadsheet_letter_to_col("Z") == 26);
    printf("✓ 'Z' converts to column 26\n");
    
    assert(spreadsheet_letter_to_col("AA") == 27);
    printf("✓ 'AA' converts to column 27\n");
    
    assert(spreadsheet_letter_to_col("AZ") == 52);
    printf("✓ 'AZ' converts to column 52\n");
    
    assert(spreadsheet_letter_to_col("CV") == 100);
    printf("✓ 'CV' converts to column 100\n");
    
    assert(spreadsheet_letter_to_col("AAA") == 703);
    printf("✓ 'AAA' converts to column 703\n");

    assert(spreadsheet_letter_to_col("BAZ") == 1404);
    printf("✓ 'BAZ' converts to column 1404\n");
    
    assert(spreadsheet_letter_to_col("AAB") == 704);
    printf("✓ 'AAB' converts to column 704\n");
    
    assert(spreadsheet_letter_to_col("AAZ") == 728);
    printf("✓ 'AAZ' converts to column 728\n");
    
    assert(spreadsheet_letter_to_col("ABA") == 729);
    printf("✓ 'ABA' converts to column 729\n");

    assert(spreadsheet_letter_to_col("ZZZ") == 18278);
    printf("✓ 'ZZZ' converts to column 18278\n");
    
    // Test round-trip conversion
    for (int col = 1; col <= 100; col++) {
        spreadsheet_col_to_letter(col, buffer, sizeof(buffer));
        int back = spreadsheet_letter_to_col(buffer);
        assert(col == back);
    }
    printf("✓ Round-trip conversion successful for columns 1-100\n");
}

// Test cell name helper functions
void test_cell_name_helpers() {
    printf("\n====== Testing cell name helpers ======\n");
    Spreadsheet *sheet = spreadsheet_create(100, 100);
    
    // Test cell name generation
    char buffer[64];
    
    assert(strcmp(spreadsheet_get_cell_name(1, 1, buffer, sizeof(buffer)), "A1") == 0);
    printf("✓ Cell (1,1) generates name 'A1'\n");
    
    assert(strcmp(spreadsheet_get_cell_name(10, 26, buffer, sizeof(buffer)), "Z10") == 0);
    printf("✓ Cell (10,26) generates name 'Z10'\n");
    
    assert(strcmp(spreadsheet_get_cell_name(100, 100, buffer, sizeof(buffer)), "CV100") == 0);
    printf("✓ Cell (100,100) generates name 'CV100'\n");
    
    // Test cell name parsing
    int row, col;
    
    assert(spreadsheet_parse_cell_name(sheet, "A1", &row, &col) == 1);
    assert(row == 1 && col == 1);
    printf("✓ 'A1' parses to row=1, col=1\n");
    
    assert(spreadsheet_parse_cell_name(sheet, "Z10", &row, &col) == 1);
    assert(row == 10 && col == 26);
    printf("✓ 'Z10' parses to row=10, col=26\n");
    
    assert(spreadsheet_parse_cell_name(sheet, "CV100", &row, &col) == 1);
    assert(row == 100 && col == 100);
    printf("✓ 'CV100' parses to row=100, col=100\n");
    
    // // Test invalid cell names
    // assert(spreadsheet_parse_cell_name(sheet, "A0", &row, &col) == 0);
    // printf("✓ 'A0' correctly identified as invalid\n");
    
    assert(spreadsheet_parse_cell_name(sheet, "A101", &row, &col) == 0);
    printf("✓ 'A101' correctly identified as invalid (row out of bounds)\n");
    
    assert(spreadsheet_parse_cell_name(sheet, "CW1", &row, &col) == 0);
    printf("✓ 'CW1' correctly identified as invalid (column out of bounds)\n");
    
    assert(spreadsheet_parse_cell_name(sheet, "1A", &row, &col) == 0);
    printf("✓ '1A' correctly identified as invalid format\n");
    
    assert(spreadsheet_parse_cell_name(sheet, "A1B", &row, &col) == 0);
    printf("✓ 'A1B' correctly identified as invalid format\n");
    
    assert(spreadsheet_parse_cell_name(sheet, "", &row, &col) == 0);
    printf("✓ Empty string correctly identified as invalid\n");
    
    // Cleanup
     for (int r = 1; r <= 100; r++)
    {
        for (int c = 1; c <= 100; c++)
        {
            cell_destroy(sheet->cells[100*(r-1)+(c-1)]);

        }
    }

    free(sheet->cells);
    free(sheet);
}

// Test basic cell value setting and arithmetic
void test_basic_arithmetic() {
    printf("\n====== Testing basic arithmetic ======\n");
    Spreadsheet *sheet = spreadsheet_create(100, 100);
    char status[256];
    
    // Test setting literal values
    set_cell(sheet, "A1", "5");
    assert_cell_value(sheet, "A1", 5, 0);
    
    set_cell(sheet, "A2", "10");
    assert_cell_value(sheet, "A2", 10, 0);
    
    // Test basic arithmetic
    set_cell(sheet, "B1", "A1+A2");
    assert_cell_value(sheet, "B1", 15, 0);
    
    set_cell(sheet, "B2", "A2-A1");
    assert_cell_value(sheet, "B2", 5, 0);
    
    set_cell(sheet, "B3", "A1*A2");
    assert_cell_value(sheet, "B3", 50, 0);
    
    set_cell(sheet, "B4", "A2/A1");
    assert_cell_value(sheet, "B4", 2, 0);
    
    // Test more complex expressions
    set_cell(sheet, "C1", "B1+B2");
    assert_cell_value(sheet, "C1", 20, 0);
    
    // Test error conditions
    set_cell(sheet, "C2", "A1/0");
    assert_cell_value(sheet, "C2", 0, 1); // Division by zero should set error flag
    
    // Cleanup
     for (int r = 1; r <= 100; r++)
    {
        for (int c = 1; c <= 100; c++)
        {
            cell_destroy(sheet->cells[100*(r-1)+(c-1)]);

        }
    }

    free(sheet->cells);
    free(sheet);
}

// Test cell references and dependencies
void test_cell_dependencies() {
    printf("\n====== Testing cell dependencies ======\n");
    Spreadsheet *sheet = spreadsheet_create(100, 100);
    
    // Set up some values
    set_cell(sheet, "A1", "5");
    set_cell(sheet, "A2", "10");
    set_cell(sheet, "B1", "A1+A2");
    
    // Test dependency propagation
    assert_cell_value(sheet, "B1", 15, 0);
    set_cell(sheet, "A1", "20");
    assert_cell_value(sheet, "B1", 30, 0);
    
    // Test more complex dependencies
    set_cell(sheet, "C1", "B1*2");
    assert_cell_value(sheet, "C1", 60, 0);
    
    set_cell(sheet, "A2", "5");
    assert_cell_value(sheet, "B1", 25, 0);
    assert_cell_value(sheet, "C1", 50, 0);
    
    // Verify that dependents are correctly tracked
    Cell *cell_a1 = sheet->cells[0]; // A1
    assert(cell_a1->dependents != NULL);
    assert(orderedset_contains(cell_a1->dependents, "B1"));
    
    // Cleanup
     for (int r = 1; r <= 100; r++)
    {
        for (int c = 1; c <= 100; c++)
        {
            cell_destroy(sheet->cells[100*(r-1)+(c-1)]);

        }
    }

    free(sheet->cells);
    free(sheet);
}

// Test cycle detection
void test_cycle_detection() {
    printf("\n====== Testing cycle detection ======\n");
    Spreadsheet *sheet = spreadsheet_create(100, 100);
    char status[256];
    
    // Setup initial values
    set_cell(sheet, "A1", "5");
    set_cell(sheet, "B1", "A1+10");
    
    // Try to create a direct cycle
    spreadsheet_set_cell_value(sheet, (char *)"A1", "B1+5", status, sizeof(status));
    printf("Attempting direct cycle A1=B1+5: %s\n", status);
    assert(strcmp(status, "Cycle Detected") == 0);
    
    // Ensure original value is preserved
    assert_cell_value(sheet, "A1", 5, 0);
    
    // Try to create a longer cycle
    set_cell(sheet, "C1", "B1*2");
    set_cell(sheet, "D1", "C1+1");
    
    spreadsheet_set_cell_value(sheet, (char *)"A1", "D1+2", status, sizeof(status));
    printf("Attempting longer cycle A1=D1+2: %s\n", status);
    assert(strcmp(status, "Cycle Detected") == 0);
    
    // Test indirect cycles
    set_cell(sheet, "E1", "5");
    set_cell(sheet, "F1", "E1*3");
    set_cell(sheet, "G1", "F1+H1");
    
    spreadsheet_set_cell_value(sheet, (char *)"H1", "G1/2", status, sizeof(status));
    printf("Attempting indirect cycle H1=G1/2: %s\n", status);
    assert(strcmp(status, "Cycle Detected") == 0);
    
    // Cleanup
     for (int r = 1; r <= 100; r++)
    {
        for (int c = 1; c <= 100; c++)
        {
            cell_destroy(sheet->cells[100*(r-1)+(c-1)]);

        }
    }

    free(sheet->cells);
    free(sheet);
}

// Test range functions (MIN, MAX, AVG, SUM, STDEV)
void test_range_functions() {
    printf("\n====== Testing range functions ======\n");
    Spreadsheet *sheet = spreadsheet_create(100, 100);
    
    // Setup test data
    set_cell(sheet, "A1", "10");
    set_cell(sheet, "A2", "20");
    set_cell(sheet, "A3", "30");
    set_cell(sheet, "A4", "40");
    set_cell(sheet, "A5", "50");
    
    // Test SUM
    set_cell(sheet, "B1", "SUM(A1:A5)");
    assert_cell_value(sheet, "B1", 150, 0);
    printf("✓ SUM(A1:A5) = 150\n");
    
    // Test AVG
    set_cell(sheet, "B2", "AVG(A1:A5)");
    assert_cell_value(sheet, "B2", 30, 0);
    printf("✓ AVG(A1:A5) = 30\n");
    
    // Test MIN
    set_cell(sheet, "B3", "MIN(A1:A5)");
    assert_cell_value(sheet, "B3", 10, 0);
    printf("✓ MIN(A1:A5) = 10\n");
    
    // Test MAX
    set_cell(sheet, "B4", "MAX(A1:A5)");
    assert_cell_value(sheet, "B4", 50, 0);
    printf("✓ MAX(A1:A5) = 50\n");
    
    // Test STDEV
    set_cell(sheet, "B5", "STDEV(A1:A5)");
    // STDEV is sqrt(((10-30)² + (20-30)² + (30-30)² + (40-30)² + (50-30)²)/5) = sqrt(200) ≈ 14
    assert_cell_value(sheet, "B5", 14, 0);
    printf("✓ STDEV(A1:A5) ≈ 14\n");
    
    // Test range dependency
    set_cell(sheet, "A1", "100");
    assert_cell_value(sheet, "B1", 240, 0); // SUM now 240
    assert_cell_value(sheet, "B2", 48, 0);  // AVG now 48
    
    // Test rectangular ranges
    set_cell(sheet, "B6", "15");
    set_cell(sheet, "B7", "25");
    set_cell(sheet, "C1", "SUM(A1:B7)"); // This includes A1-A5, B6-B7
    assert_cell_value(sheet, "C1", 716, 0); 
    
    // Test edge cases
    set_cell(sheet, "C2", "MIN(A1:A1)");
    assert_cell_value(sheet, "C2", 100, 0);
    
    set_cell(sheet, "C3", "SUM(Z1:Z5)"); // Empty range
    assert_cell_value(sheet, "C3", 0, 0);
    
    // Invalid range format
    set_cell(sheet, "C4", "SUM(A5:A1)"); // Inverted range
    char status[256];
    spreadsheet_set_cell_value(sheet, (char *)"C5", "SUM(A5:A1)", status, sizeof(status));
    printf("Attempting invalid range SUM(A5:A1): %s\n", status);
    
    // Cleanup
     for (int r = 1; r <= 100; r++)
    {
        for (int c = 1; c <= 100; c++)
        {
            cell_destroy(sheet->cells[100*(r-1)+(c-1)]);

        }
    }

    free(sheet->cells);
    free(sheet);
}

// Test the SLEEP function
void test_sleep_function() {
    printf("\n====== Testing SLEEP function ======\n");
    Spreadsheet *sheet = spreadsheet_create(100, 100);
    
    // Test with literal
    printf("Testing SLEEP(1) - should pause for 1 second...\n");
    set_cell(sheet, "A1", "SLEEP(1)");
    assert_cell_value(sheet, "A1", 1, 0);
    
    // Test with cell reference
    set_cell(sheet, "B1", "2");
    printf("Testing SLEEP(B1) - should pause for 2 seconds...\n");
    set_cell(sheet, "A2", "SLEEP(B1)");
    assert_cell_value(sheet, "A2", 2, 0);
    
    // Test with zero
    set_cell(sheet, "A3", "SLEEP(0)");
    assert_cell_value(sheet, "A3", 0, 0);
    
    // Cleanup
     for (int r = 1; r <= 100; r++)
    {
        for (int c = 1; c <= 100; c++)
        {
            cell_destroy(sheet->cells[100*(r-1)+(c-1)]);

        }
    }

    free(sheet->cells);
    free(sheet);
}

// Test edge cases and error handling
void test_edge_cases() {
    printf("\n====== Testing edge cases ======\n");
    Spreadsheet *sheet = spreadsheet_create(100, 100);
    char status[256];
    
    // Test reference to non-existent cell
    set_cell(sheet, "A1", "Z99");
    assert_cell_value(sheet, "A1", 0, 0); // Non-existent cells have value 0
    
    // Test division by zero in various ways
    set_cell(sheet, "B1", "0");
    set_cell(sheet, "A2", "10/0");
    assert_cell_value(sheet, "A2", 0, 1); // Error flag should be set
    
    set_cell(sheet, "A3", "10/B1");
    assert_cell_value(sheet, "A3", 0, 1);
    
    // Test propagation of errors
    set_cell(sheet, "A4", "A3+5");
    assert_cell_value(sheet, "A4", 0, 1); // Error propagates
    
    // Test references to cells with errors
    set_cell(sheet, "A5", "MIN(A2:A4)");
    assert_cell_value(sheet, "A5", 0, 1);
    
    // Cleanup
     for (int r = 1; r <= 100; r++)
    {
        for (int c = 1; c <= 100; c++)
        {
            cell_destroy(sheet->cells[100*(r-1)+(c-1)]);

        }
    }

    free(sheet->cells);
    free(sheet);
}

// Test commands validation
void test_command_validation() {
    printf("\n====== Testing command validation ======\n");
    Spreadsheet *sheet = spreadsheet_create(100, 100);
    
    // Test valid commands
    char *cell = (char *)"A1";
    char *formula = (char *)"10";
    assert(is_valid_command(sheet, &cell, &formula) == 1);
    printf("✓ 'A1=10' is valid\n");
    
    formula = (char *)"B1+C1";
    assert(is_valid_command(sheet, &cell, &formula) == 1);
    printf("✓ 'A1=B1+C1' is valid\n");
    
    formula = (char *)"SUM(A1:Z10)";
    assert(is_valid_command(sheet, &cell, &formula) == 1);
    printf("✓ 'A1=SUM(A1:Z10)' is valid\n");
    
    formula = (char *)"SLEEP(5)";
    assert(is_valid_command(sheet, &cell, &formula) == 1);
    printf("✓ 'A1=SLEEP(5)' is valid\n");
    
    // Test invalid commands
    // cell = (char *)"A0"; // Invalid row
    // formula = (char *)"10";
    // assert(is_valid_command(sheet, &cell, &formula) == 0);
    // printf("✓ 'A0=10' is invalid\n");
    
    //TODO: Fix this
    cell = (char *)"A1";
    formula = (char *)"B1++C1"; // Invalid arithmetic
    assert(is_valid_command(sheet, &cell, &formula) == 0);
    printf("✓ 'A1=B1++C1' is invalid\n");
    
    formula = (char *)"NOFUNCTION(A1)"; // Invalid function
    assert(is_valid_command(sheet, &cell, &formula) == 0);
    printf("✓ 'A1=NOFUNCTION(A1)' is invalid\n");
    
    formula = (char *)"SUM(Z10:A1)"; // Inverted range
    assert(is_valid_command(sheet, &cell, &formula) == 0);
    printf("✓ 'A1=SUM(Z10:A1)' is invalid\n");
    
    // Cleanup
     for (int r = 1; r <= 100; r++)
    {
        for (int c = 1; c <= 100; c++)
        {
            cell_destroy(sheet->cells[100*(r-1)+(c-1)]);

        }
    }

    free(sheet->cells);
    free(sheet);
}

// Run all tests
int main() {
    printf("Starting spreadsheet unit tests\n");
    
    test_spreadsheet_create();
    test_column_letter_conversion();
    test_cell_name_helpers();
    test_basic_arithmetic();
    test_cell_dependencies();
    test_cycle_detection();
    test_range_functions();
    test_sleep_function();
    test_edge_cases();
    test_command_validation();
    
    printf("\nAll tests completed successfully!\n");
    return 0;
}
