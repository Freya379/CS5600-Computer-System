/**
 * Created by Freya Ma (YU MA) on 2/1/24.
 * CS 5600 PA1: The Unix Shell
 * built-in commands: cd, help, exit, and history
 *
 * run with the Cmake file, then type ./shell then the prompt> will appear
 * run the batch file with ./shell test_batch.txt

 **/

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define LSH_TOK_BUFSIZE 64
#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_DELIM " \t\r\n\a"

#define MAX_CMD 1024
#define BUFFSIZE 256
#define MAX_HISTORY 1024
#define MAX_COMMAND_LEN 256

/*
  Function Declarations for builtin shell commands:
 */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_history(char **args);
int commandNum = 0;
char history[MAX_HISTORY][MAX_COMMAND_LEN];  // store the history

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
        "cd",
        "help",
        "exit",
        "history"
};

int (*builtin_func[]) (char **) = {
        &lsh_cd,
        &lsh_help,
        &lsh_exit,
        &lsh_history
};

int num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

/*
  Function Declarations
 */
char **split_line(char *line);
char **split_commands(char *line, int *n_commands);
void execute_commands(char **commands, int n_commands);
void free_tokens(char **tokens);

/*
  Builtin function implementations.
*/

/**
 * @brief Executes the 'cd' built-in command to change the current directory.
 *
 * @param args An array of arguments where args[0] is "cd" and args[1] is the target directory.
 * @return Returns 0 if directory change is successful, 1 if an error occurs.
 */
int lsh_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
        return 1; // Return error code
    } else {
        if (chdir(args[1]) != 0) {
            perror("lsh");
            return 1; // Return error code
        }
    }
    return 0;
}

/**
 * @brief Displays help information about the built-in commands.
 *
 * @param args An array of arguments, not used in this function.
 * @return Always returns 0 to indicate successful execution.
 */
int lsh_help(char **args) {
    int i;
    printf("HA? It is mine LSH, FREYAYAYYAY\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (int i = 0; i < num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 0;
}

/**
 * @brief Exits the shell when the 'exit' built-in command is called.
 *
 * @param args An array of arguments, not used in this function.
 * @return Always returns 0 to indicate that the shell should terminate.
 */
int lsh_exit(char **args) {
    exit(0);
    return 0; // never reached
}

/**
 * @brief Displays the history of commands executed in the shell up to a specified number.
 *
 * If an argument is provided, it displays the last 'n' commands from the history,
 * where 'n' is the value of the argument. If 'n' is greater than the total number
 * of commands in the history, or if no argument is provided, it displays all the commands.
 *
 * @param args An array of arguments where args[1], if present, specifies the number of
 *             history entries to display.
 * @return Always returns 0 to indicate successful execution.
 */
int lsh_history(char **args) {
    int n = commandNum;

    if (args[1] != NULL) {
        n = atoi(args[1]);
        if (n < 0 || n > commandNum) {
            n = commandNum;
        }
    }

    printf("History (last %d commands):\n", n);
    for (int i = commandNum - n; i < commandNum; i++) {
        printf("%d: %s\n", i + 1, history[i]);
    }

    return 0;
}

/**
 * @brief Checks if a command is a built-in command.
 *
 * @param cmd The command string to check.
 * @return 1 if the command is a built-in command, 0 otherwise.
 */
int is_builtin_command(char *cmd) {
    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(cmd, builtin_str[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Executes the command entered by the user.
 *
 * This function determines if the command is a built-in shell command or an external program.
 * For built-in commands, it calls the appropriate function. For external commands,
 * it forks a child process and uses execvp to run the command. It handles the 'exit'
 * command by terminating the shell.
 *
 * @param args An array of arguments where args[0] is the command to execute.
 * @return Returns 1 if the shell should continue executing, 0 if it should exit, and
 *         the exit status of the executed command if an external command was executed.
 */
int execute(char **args) {
    if (args[0] == NULL || strcmp(args[0], "") == 0) {
        // empty
        return 1;
    }

    if (is_builtin_command(args[0])) {
        for (int i = 0; i < num_builtins(); i++) {
            if (strcmp(args[0], builtin_str[i]) == 0) {
                return (*builtin_func[i])(args);
            }
        }
    } else {
        // deal with "exit" command
        if (strcmp(args[0], "exit") == 0) {
            return lsh_exit(args);
        }

        // this is an external command
        pid_t pid, wpid;
        int status;

        pid = fork();
        if (pid == 0) {
            // child process
            if (execvp(args[0], args) == -1) {
                perror("lsh");
                exit(EXIT_FAILURE);
            }
        } else if (pid < 0) {
            perror("lsh");
            exit(EXIT_FAILURE);
        } else {
            // parent process
            do {
                wpid = waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));

            if (WIFEXITED(status)) {
                return WEXITSTATUS(status);
            } else {
                return 1;
            }
        }
    }

    return 1;
}
/**
 * @brief Splits a line of input into tokens.
 *
 * This function takes a line of input from the user and splits it into individual
 * tokens based on whitespace and other delimiters. It dynamically allocates memory
 * for the tokens and returns an array of pointers to the tokens.
 *
 * @param line The line of input to split.
 * @return Returns an array of pointers to the tokens, with the last element set to NULL.
 */
char **split_line(char *line) {
    int bufsize = LSH_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token, *escaped_token;

    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_TOK_DELIM);
    while (token != NULL) {
        // Handle escaped characters
        escaped_token = malloc(strlen(token) + 1);
        strcpy(escaped_token, token);

        tokens[position] = escaped_token;
        position++;

        if (position >= bufsize) {
            bufsize += LSH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

/**
 * @brief Splits a line of input into commands separated by semicolons.
 *
 * This function takes a line of input from the user and splits it into individual
 * commands based on semicolons. It dynamically allocates memory for the commands
 * and returns an array of pointers to the commands.
 *
 * @param line The line of input to split.
 * @param n_commands A pointer to an integer to store the number of commands.
 * @return Returns an array of pointers to the commands, with the last element set to NULL.
 */
char **split_commands(char *line, int *n_commands) {
    int bufsize = LSH_TOK_BUFSIZE, position = 0;
    char **commands = malloc(bufsize * sizeof(char*));
    char *command;

    if (!commands) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    command = strtok(line, ";");
    while (command != NULL) {
        commands[position] = malloc(strlen(command) + 1);
        strcpy(commands[position], command);
        position++;

        if (position >= bufsize) {
            bufsize += LSH_TOK_BUFSIZE;
            commands = realloc(commands, bufsize * sizeof(char*));
            if (!commands) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        command = strtok(NULL, ";");
    }
    commands[position] = NULL;
    *n_commands = position;

    return commands;
}

/**
 * @brief Executes a list of commands separated by semicolons.
 *
 * This function takes an array of commands and executes them one by one.
 * It handles empty commands and frees the memory allocated for the tokens
 * after executing each command.
 *
 * @param commands An array of pointers to the commands.
 * @param n_commands The number of commands in the array.
 */
void execute_commands(char **commands, int n_commands) {
    for (int i = 0; i < n_commands; i++) {
        if (strcmp(commands[i], "") == 0) continue;

        char **args = split_line(commands[i]);
        if (args[0] == NULL) {
            free_tokens(args);
            continue;
        }

        execute(args);
        free_tokens(args);
    }
}

/**
 * @brief Frees the memory allocated for an array of tokens.
 *
 * This function takes an array of pointers to tokens and frees the memory
 * allocated for each token, as well as the memory allocated for the array itself.
 *
 * @param tokens An array of pointers to the tokens.
 */
void free_tokens(char **tokens) {
    for (int i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);
}

/**
 * @brief The main loop of the shell.
 *
 * This function reads input from the user or a file, splits the input into commands,
 * executes the commands, and handles the history of commands.
 *
 * @param stream The input stream (stdin or a file).
 * @param isInteractive A flag indicating whether the shell is running in interactive mode or not.
 */
void loop(FILE *stream, int isInteractive) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int n_commands;

    do {
        if (isInteractive) {
            printf("prompt> ");
            fflush(stdout);
        }

        read = getline(&line, &len, stream);
        if (read == -1) break; // EOF

        if (line == NULL) {
            fprintf(stderr, "lsh: getline error\n");
            break;
        }

        char **commands = split_commands(line, &n_commands);

        if (commandNum < MAX_HISTORY) {
            strncpy(history[commandNum], line, MAX_COMMAND_LEN - 1);
            history[commandNum][MAX_COMMAND_LEN - 1] = '\0';
            commandNum++;
        }

        execute_commands(commands, n_commands);
        free(commands);
    } while (isInteractive || read != -1);

    free(line);
}

/**
 * @brief Main entry point.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return status code
 */
//int main(int argc, char **argv) {
//    FILE *inputStream = stdin;
//    int isInteractive = 1;
//
//    if (argc == 2) {
//        inputStream = fopen(argv[1], "r");
//        if (!inputStream) {
//            fprintf(stderr, "lsh: cannot open file %s\n", argv[1]);
//            exit(EXIT_FAILURE);
//        }
//        isInteractive = 0; // close the interactive model
//    }
//
//    loop(inputStream, isInteractive);
//
//    if (inputStream != stdin) {
//        fclose(inputStream);
//    }
//
//    return EXIT_SUCCESS;
//}
int main(int argc, char **argv) {
    if (argc == 2) { // batch mode
        FILE *batch_file = fopen(argv[1], "r");
        if (batch_file == NULL) {
            fprintf(stderr, "lsh: cannot open file %s\n", argv[1]);
            return 1;
        }
        loop(batch_file, 0); // 0 means non-interactive mode
        fclose(batch_file);
    } else if (argc == 1) { // interactive mode
        loop(stdin, 1); // 1 means interactive mode
    } else {
        fprintf(stderr, "Usage: %s [batch_file]\n", argv[0]);
        return 1;
    }
    return 0;
}