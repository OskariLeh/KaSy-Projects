#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Calculates the length of a null-terminated array of strings
// Returns the number of elements in the array
int arrayLength(char **array){
    int i = 0;

    while(array[i] != NULL) {
        i++;
    }
    return i;
}

// Frees the memory allocated for a null-terminated array of strings
void freeArray(char ***array) {
    int elements = arrayLength((*array));
    for (int i = 0; i < elements; i++) {
        free((*array)[i]);
    }
    free((*array));
    (*array) = NULL;
    return;
}

// Adds a new argument to the end of a null-terminated array of strings
// If the array is NULL, it allocates memory for the first element
// If the array is not NULL, it reallocates memory to add the new element
void addArgument(char ***array, char *arg) {
    // This post helped with creating a dynamic array https://stackoverflow.com/questions/5935933/dynamically-create-an-array-of-strings-with-malloc

    if ((*array) == NULL) {
        (*array) = malloc(sizeof(char*) * 2);
        (*array)[0] = malloc(sizeof(char) * (strlen(arg) + 1));
        if (((*array) == NULL) || ((*array)[0] == NULL)) {
            fprintf(stderr, "malloc failed");
            exit(1);
        }
        strcpy((*array)[0], arg);
        (*array)[1] = NULL;
    } else {
        int elements = arrayLength((*array));

        // realloc the array to add a new element
        (*array) = realloc((*array), sizeof(char*) * (elements + 2));
        (*array)[elements] = malloc(sizeof(char) * (strlen(arg) + 1));

        if (((*array) == NULL) || ((*array)[elements] == NULL)) {
            fprintf(stderr, "realloc failed");
            exit(1);
        }

        strcpy((*array)[elements], arg);
        (*array)[elements + 1] = NULL;
    }

    return;
}

void executeCommand(char **argv, char *path) {
    char *commandPath = NULL;

    commandPath = malloc(sizeof(char) * (strlen(path) + strlen(argv[0]) + 2)); // +2 for '/' and '\0' 
    if (commandPath == NULL) {
        fprintf(stderr, "malloc failed");
        _exit(1);
    }

    // Construct the command path                                                                                                                                                                              
    strcpy(commandPath, path);
    strcat(commandPath, "/");
    strcat(commandPath, argv[0]);
    // Check if the command is executable
    if(access(commandPath, X_OK) == 0) {
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            // https://www.youtube.com/watch?v=5fnVr-zH-SE thsis video helped me with redirection
            if (arrayLength(argv) > 1) {
                if (strcmp(argv[arrayLength(argv)-2], ">") == 0) {
                    // Redirect stdout to a file
                    FILE *file = fopen(argv[arrayLength(argv) - 1], "w");
                    if (file == NULL) {
                        fprintf(stderr, "error: cannot open file '%s'\n", argv[arrayLength(argv)-1]);
                        _exit(1);
                    }
                    dup2(fileno(file), STDOUT_FILENO);
                    fclose(file);
    
                    free(argv[arrayLength(argv)-2]); // remove the redirection part from the argument list
                    free(argv[arrayLength(argv)-1]); // remove the file name from the argument list
                    argv[arrayLength(argv)-2] = NULL; // remove the redirection part from the argument list
                    argv[arrayLength(argv)-1] = NULL; // remove the file name from the argument list
    
                }
            }
            execv(commandPath, argv);
            fprintf(stderr, "error: execv failed");
            _exit(1);
        } else if (pid < 0) {
            // Fork failed
            fprintf(stderr, "error: fork failed");
            exit(1);
        } else {
            // Parent process
            wait(NULL);
            free(commandPath);
            commandPath = NULL;
        }
    } else {
        free(commandPath); // not found in this path check next path or the command does not exist
        commandPath = NULL;
    }
    return;
}

// Interactive mode function
// Reads commands from the user and executes them
// If the command is "exit", it breaks the loop and exits
void shell(char ***paths, FILE * stream) {
    char *command = NULL;
    char ***commandArray = NULL;
    char *token = NULL;
    char **argArray = NULL;
    size_t size = 0;
    __ssize_t line;

    while(1) {
        if (stream == stdin) {
            fprintf(stdout, "wish> ");
        }
        
        line = getline(&command, &size, stream); // Read a line from the input stream
        if (line == -1 ) {
            // EOF reached
            break;
        }
        if (line == 1) {
            // Empty command
            continue;
        }

        command[line - 1] = '\0'; // Remove the newline character

        // Tokenize the command
        token = strtok(command, " "); 
        while (token != NULL) {
            addArgument(&argArray, token);
            token = strtok(NULL, " ");
        }

        // Handle built-in commands
        //First check if the command is "exit"
        if (strcmp(argArray[0], "exit") == 0) {
            if (arrayLength(argArray) != 1) {
                fprintf(stderr, "error: exit does not take any arguments\n");
                freeArray(&argArray);
                continue;
            }
            freeArray(&argArray);
            break;
        }

        // Check for empty command
        // If the command is empty, we will continue to the next iteration
        if (argArray[0] == NULL) {
            freeArray(&argArray);
            continue;
        }

        // Check for "path" command
        // If the command is "path", we will set the paths to the new paths
        if (strcmp(argArray[0], "path") == 0) {
            freeArray(paths);
            (*paths) = NULL;

            (*paths) = malloc(sizeof(char*) * (arrayLength(argArray)));
            if ((*paths) == NULL) {
                fprintf(stderr, "malloc failed");
                exit(1);
            }

            for (int i = 1; i < arrayLength(argArray); i++) {
                (*paths)[i - 1] = malloc(sizeof(char) * (strlen(argArray[i]) + 1));
                if ((*paths)[i - 1] == NULL) {
                    fprintf(stderr, "malloc failed");
                    exit(1);
                }
                strcpy((*paths)[i - 1], argArray[i]);
                (*paths)[i] = NULL;
            }

            if (arrayLength(argArray) == 1) {
                (*paths)[0] = NULL;
            }
            freeArray(&argArray);
            continue;
        }

        // Check for "cd" command
        // If the command is "cd", we will change the directory
        if (strcmp(argArray[0], "cd") == 0) {
            if (arrayLength(argArray) != 2) {
                fprintf(stderr, "error: cd requires exactly one argument\n");
                freeArray(&argArray);
                continue;
            }
            if (chdir(argArray[1]) != 0) {
                fprintf(stderr, "error: cd failed\n");
            }
            freeArray(&argArray);
            continue;
        }
        
        // if not a built-in command, we will start finding the command in the paths
        // and execute it if it is found
        for (int i = 0; i < arrayLength((*paths)); i++){
            // allocating memory for the command array
            // Maximum number of commands is the number of arguments
            commandArray = malloc(sizeof(char**) * (arrayLength(argArray) + 1));
            commandArray[0] = malloc(sizeof(char*) * (arrayLength(argArray) + 1));
            if (commandArray == NULL || commandArray[0] == NULL) {
                fprintf(stderr, "malloc failed");
                exit(1);
            }
            commandArray[1] = NULL;

            int commandCount = 0;
            int argCount = 0;

            // Loop through the arguments and split them into commands
            // based on the "&" character
            for (int j = 0; j < arrayLength(argArray); j++) {
                if (strcmp(argArray[j], "&") == 0) {
                    commandCount++;
                    commandArray[commandCount] = malloc(sizeof(char*) * (arrayLength(argArray) + 1));
                    if (commandArray[commandCount] == NULL) {
                        fprintf(stderr, "malloc failed");
                        exit(1);
                    }
                    commandArray[commandCount + 1] = NULL;
                    argCount = 0;
                    continue;
                }

                commandArray[commandCount][argCount] = malloc(sizeof(char) * (strlen(argArray[j]) + 1));
                if (commandArray == NULL) {
                    fprintf(stderr, "malloc failed");
                    exit(1);
                }

                strcpy(commandArray[commandCount][argCount], argArray[j]);
                commandArray[commandCount][argCount + 1] = NULL;
                argCount++;
            }

            // Execute the commands in parallel
            // This post helped here: https://stackoverflow.com/questions/65202550/fork-4-children-in-a-loop
            for (int j = 0; j <= commandCount; j++){
                pid_t pid = fork();
                if (pid == 0) {
                    // Child process
                    executeCommand(commandArray[j], (*paths)[i]);
                    
                    _exit(0);
                }
                if (pid < 0) {
                    // Fork failed
                    fprintf(stderr, "error: fork failed");
                    _exit(1);
                }
            }
            // Parent process
            for (int j = 0; j <= commandCount; j++){
                wait(NULL);
            }
            for (int i = 0; i <= commandCount; i++){
                freeArray(&commandArray[i]);
            }
            free(commandArray);
            commandArray = NULL;
            
        }
        freeArray(&argArray);
        argArray = NULL;
    }
    free(command);
    return;
}

int main(int argc, char *argv[]) {
    char **paths = NULL;

    // Initialize paths to default value
    paths = malloc(sizeof(char*) * 2);
    if (paths == NULL) {
        fprintf(stderr, "malloc failed");
        exit(1);
    }
    paths[0] = malloc(sizeof(char) * 5);
    if (paths[0] == NULL) {
        fprintf(stderr, "malloc failed");
        exit(1);
    }
    strcpy(paths[0], "/bin");
    paths[1] = NULL;

    switch (argc) {
    case 1:
        shell(&paths, stdin);
        break;
    case 2:
        FILE *batchFile = fopen(argv[1], "r");
        if (batchFile == NULL) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[1]);
            exit(1);
        }
        shell(&paths, batchFile);
        fclose(batchFile);
        break;
    default:
        fprintf(stderr, "error: Too many arguments");
        exit(1);
        break;
    }

    //If exit is called or EOF reached, we will reach here.
    freeArray(&paths);
    exit(0);
}