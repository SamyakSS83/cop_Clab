# Makefile

CC = gcc
CFLAGS = -Wall -Wextra -g -O3

OBJ = main.o spreadsheet.o orderedset.o vector.o stack.o linked_list.o cell.o 

all: spreadsheet


test: orderedset_test spreadsheet_test stack_test linked_list_test tester scroll_test vector_test cell_test
	@echo "Running tests"
	@echo "Orderedset test"
	@echo "----------------------------------------------------------------------------------------------------------"
	./orderedset_test
	@echo "Vector test"
	@echo "----------------------------------------------------------------------------------------------------------"
	./vector_test
	@echo "Cell test"
	@echo "----------------------------------------------------------------------------------------------------------"
	./cell_test
	@echo "Stack test"
	@echo "----------------------------------------------------------------------------------------------------------"
	./stack_test
	@echo "Linked list test"
	@echo "----------------------------------------------------------------------------------------------------------"
	./linked_list_test
	@echo "Spreadsheet test"
	@echo "----------------------------------------------------------------------------------------------------------"
	./spreadsheet_test
	@echo "Scroll test"
	@echo "----------------------------------------------------------------------------------------------------------"
	./scroll_test
	@echo "Interactive test"
	@echo "----------------------------------------------------------------------------------------------------------"
	@./test 100 100 input_arbit.txt output_arbit.txt
	@echo "----------------------------------------------------------------------------------------------------------"
	@./test 100 100 input_checks_err.txt output_checks_err.txt
	@echo "----------------------------------------------------------------------------------------------------------"
	@./test 100 100 input_cycle.txt output_cycle.txt
	@echo "----------------------------------------------------------------------------------------------------------"
	@./test 100 100 input_unrec_cmds.txt output_unrec_cmds.txt
	@echo "----------------------------------------------------------------------------------------------------------"
	@./test 100 100 input_sleep.txt output_sleep.txt
	@echo "----------------------------------------------------------------------------------------------------------"
	@echo "----------------------------------------------------------------------------------------------------------"
	@./test 100 100 input_cycle1.txt output_cycle1.txt
	@echo "----------------------------------------------------------------------------------------------------------"
	@echo "----------------------------------------------------------------------------------------------------------"
	@./test 100 100 input_out_of_range.txt output_out_of_range.txt
	@echo "----------------------------------------------------------------------------------------------------------"
	@echo "----------------------------------------------------------------------------------------------------------"
	@./test 100 100 input_scroll_test.txt output_scroll_test.txt
	@echo "----------------------------------------------------------------------------------------------------------"
	@echo "----------------------------------------------------------------------------------------------------------"
	@./target/release/spreadsheet 100 100 < input100x100.txt > output100x100.txt
	@echo "----------------------------------------------------------------------------------------------------------"
	@echo "All tests passed"

report: report.tex
	@pdflatex report.tex 
	@echo "Report generated as report.pdf"



spreadsheet: $(OBJ)
	$(CC) $(CFLAGS) -o spreadsheet $(OBJ) -lm
	mkdir -p target/release
	mv spreadsheet target/release

main.o: main.c spreadsheet.h
	$(CC) $(CFLAGS) -c main.c

spreadsheet.o: spreadsheet.c spreadsheet.h orderedset.h vector.h stack.h linked_list.h
	$(CC) $(CFLAGS) -c spreadsheet.c

orderedset.o: orderedset.c orderedset.h
	$(CC) $(CFLAGS) -c orderedset.c

vector.o: vector.c vector.h
	$(CC) $(CFLAGS) -c vector.c


stack.o: stack.c stack.h cell.h
	$(CC) $(CFLAGS) -c stack.c

# vector.h vector.c vector.o

linked_list.o: linked_list.h cell.h
	$(CC) $(CFLAGS) -c linked_list.c

cell.o: cell.c cell.h orderedset.h vector.h
	$(CC) $(CFLAGS) -c cell.c

orderedset_test: orderedset_test.o orderedset.o
	$(CC) $(CFLAGS) -o orderedset_test orderedset_test.o orderedset.o

orderedset_test.o: orderedset_test.c orderedset.h
	$(CC) $(CFLAGS) -c orderedset_test.c

cell_test: cell_test.o cell.o orderedset.o vector.o
	$(CC) $(CFLAGS) -o cell_test cell_test.o cell.o orderedset.o vector.o

cell_test.o: cell_test.c cell.h orderedset.h vector.h
	$(CC) $(CFLAGS) -c cell_test.c

stack_test: stack_test.o stack.o cell.o orderedset.o vector.o
	$(CC) $(CFLAGS) -o stack_test stack_test.o stack.o cell.o orderedset.o vector.o

stack_test.o: stack_test.c stack.h
	$(CC) $(CFLAGS) -c stack_test.c

linked_list_test: linked_list_test.o linked_list.o cell.o orderedset.o vector.o
	$(CC) $(CFLAGS) -o linked_list_test linked_list_test.o linked_list.o cell.o orderedset.o vector.o

linked_list_test.o: linked_list_test.c linked_list.h
	$(CC) $(CFLAGS) -c linked_list_test.c

spreadsheet_test: spreadsheet_test.o spreadsheet.o orderedset.o stack.o linked_list.o cell.o vector.o
	$(CC) $(CFLAGS) -o spreadsheet_test spreadsheet_test.o spreadsheet.o orderedset.o vector.o stack.o linked_list.o cell.o -lm 

spreadsheet_test.o: spreadsheet_test.c spreadsheet.h
	$(CC) $(CFLAGS) -c spreadsheet_test.c 

tester: test.c spreadsheet
	$(CC) $(CFLAGS) -o test test.c

scroll_test: scroll_test.o vector.o stack.o linked_list.o cell.o spreadsheet.o orderedset.o
	$(CC) $(CFLAGS) -o scroll_test scroll_test.o spreadsheet.o orderedset.o vector.o stack.o linked_list.o cell.o -lm

scroll_test.o: scroll_test.c 
	$(CC) $(CFLAGS) -c scroll_test.c

vector_test: vector_test.c vector.o
	$(CC) $(CFLAGS) -o vector_test vector.c vector_test.c




clean:
	rm -rf *.o spreadsheet orderedset_test target test orderedset_test cell_test stack_test linked_list_test spreadsheet_test tester scroll_test vector_test vector
	rm -f *.aux *.log *.out *.toc *.bbl *.blg *.lof *.lot *.pdf

.PHONY: report, clean, test
