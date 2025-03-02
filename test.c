#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pty.h>
#include <utmp.h>
#include <fcntl.h>
#include <regex.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define READ_BUF_SIZE 256
#define CMD_BUF_SIZE 1024
#define PROMPT_REGEX "\\[[0-9]{1,2}\\.[0-9]\\] \\([^)]*\\) >"

int read_until_pattern(int fd, regex_t *pattern,  char **outbuf, int timeout_seconds) {
    size_t total_size = 0;
    size_t buffer_size = 4096;
    char *buffer = malloc(buffer_size);
    if (!buffer) {
        perror("malloc");
        return -1;
    }
    buffer[0] = '\0';

    fd_set readfds;
    struct timeval tv;
    time_t start = time(NULL);
    int found = 0;

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        int ret = select(fd + 1, &readfds, NULL, NULL, &tv);
        if (ret > 0 && FD_ISSET(fd, &readfds)) {
            char temp[READ_BUF_SIZE];
            int n = read(fd, temp, READ_BUF_SIZE - 1);
            if (n < 0) {
                perror("read");
                free(buffer);
                return -1;
            } else if (n == 0) {
                // End-of-file
                break;
            }
            temp[n] = '\0';
            
            size_t new_total = total_size + n;
            if (new_total + 1 > buffer_size) {
                buffer_size = new_total + 1024;
                char *newbuf = realloc(buffer, buffer_size);
                if (!newbuf) {
                    perror("realloc");
                    free(buffer);
                    return -1;
                }
                buffer = newbuf;
            }
            strcat(buffer, temp);
            total_size = new_total;
            // If pattern matches the accumulated output, break out.
            if (regexec(pattern, buffer, 0, NULL, 0) == 0) {
                found = 1;
                break;
            }
        }
        if (time(NULL) - start >= timeout_seconds) {
            break;
        }
    }
    *outbuf = buffer;
    return found ? 0 : 1; // 0: pattern found, 1: timeout/no match.
}

int main(int argcmy, char *argvmy[]) {

    char *myrows = argvmy[1];
    char *mycols = argvmy[2];
    char *myfile = argvmy[3];
    char *outfile = argvmy[4];

    printf("Rows: %s\n", myrows);
    printf("Cols: %s\n", mycols);
   
    FILE *outfp = fopen(outfile, "w");
    if (!outfp) {
        // perror("fopen output file");
        // fclose(logfp);
        return 1;
    }
    FILE *inputfp = fopen(myfile, "r");
    if (!inputfp) {
        perror("fopen input file");
        // fclose(logfp);
        fclose(outfp);
        return 1;
    }



    // Create a pseudo-terminal and spawn the external process.
    int master_fd;
    pid_t pid = forkpty(&master_fd, NULL, NULL, NULL);
    if (pid < 0) {
        // perror("forkpty");
        // fclose(logfp);
        fclose(outfp);
        fclose(inputfp);
        return 1;
    } else if (pid == 0) {
        // Child process: execute the external program ("./sheet 50 50")
        char *argv[] = {"./target/release/spreadsheet", myrows, mycols, NULL};
        execvp(argv[0], argv);
        perror("execvp");
        exit(1);
    } else {
        printf("Starting program...\n");

        // Wait for the initial prompt, expecting "[0.0] (ok) >"
        regex_t init_regex;
        if (regcomp(&init_regex, PROMPT_REGEX , REG_EXTENDED) != 0) {
            fprintf(stderr, "Failed to compile initial regex\n");
            // fclose(logfp);
            fclose(outfp);
            fclose(inputfp);
            return 1;
        }
        char *buffer = NULL;
        if (read_until_pattern(master_fd, &init_regex,  &buffer, 100) != 0) {
            printf("Error: Unexpected response while waiting for initial prompt.\n");
            if (buffer) {
                printf("Buffer: %s\n", buffer);
                free(buffer);
            }
            regfree(&init_regex);
            // fclose(logfp);
            fclose(outfp);
            fclose(inputfp);
            return 1;
        }
        free(buffer);
        regfree(&init_regex);

        // Read commands from input.txt line-by-line and process each.
        char line[CMD_BUF_SIZE];
        while (fgets(line, sizeof(line), inputfp) != NULL) {
            // Trim trailing newline
            size_t len = strlen(line);
            if (len > 0 && line[len-1] == '\n') {
                line[len-1] = '\0';
            }
            // Skip empty lines.
            if (strlen(line) == 0) continue;

            printf("\n=== Processing command: %s ===\n", line);
            // Prepare the command by appending a newline.
            char cmd_line[CMD_BUF_SIZE];
            snprintf(cmd_line, sizeof(cmd_line), "%s\n", line);

            if (write(master_fd, cmd_line, strlen(cmd_line)) < 0) {
                perror("write");
                break;
            }
            printf("Command sent: %s\n", line);

            // Expect a prompt like "[<number>.<number>] (ok) >" after sending the command.
            regex_t prompt_regex;
            if (regcomp(&prompt_regex,PROMPT_REGEX , REG_EXTENDED) != 0) {
                fprintf(stderr, "Failed to compile prompt regex\n");
                break;
            }
            char *cmd_output = NULL;
            if (read_until_pattern(master_fd, &prompt_regex,  &cmd_output, 10) != 0) {
                printf("Error: Response not received for command: %s\n", line);
                if (cmd_output) {
                    printf("Buffer: %s\n", cmd_output);
                    free(cmd_output);
                }
                regfree(&prompt_regex);
                break;
            }
            // Write the command and response output to output.txt.
            fprintf(outfp, "Command: %s\nOutput: %s\n\n", line, cmd_output);
            // printf("Output captured: %s\n", cmd_output);
            free(cmd_output);
            regfree(&prompt_regex);
            usleep(100000); // short delay between commands
        }

        // Send the quit command.
        printf("\nSending quit command...\n");
        write(master_fd, "q\n", 2);
        waitpid(pid, NULL, 0); // Optionally wait for the child process to exit.
    }

    
    fclose(outfp);
    fclose(inputfp);
    return 0;
}
