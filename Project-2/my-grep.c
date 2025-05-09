#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to search for a term in a string and print the string if found
void grep(char *string, char *searchTerm) {
    int searchChar = 0;

    for (int i = 0; i < strlen(string); i++) {
        if (string[i] == searchTerm[searchChar]) {
            searchChar++;
            if (searchChar == strlen(searchTerm)) {
                fprintf(stdout, "%s", string);
                break;
            }
        } else {
            searchChar = 0;
        }
    }
    return;
}

// Function to read a file line by line and search for a term in each line
void grepFile(char *fileName, char *searchTerm) {
    FILE *file = fopen(fileName, "r");
    char *buffer = NULL;
    size_t size = 0;
    __ssize_t read;

    if (file == NULL) {
        fprintf(stdout, "my-grep: cannot open file '%s'\n", fileName);
        exit(1);
    }
    while ((read = getline(&buffer, &size, file)) != -1) {
         grep(buffer, searchTerm);
    }  
    
    free(buffer);
    fclose(file);
    return;
}

int main(int argc, char *argv[]) {
    char buffer[128];

    if (argc == 1) {
        fprintf(stdout, "my-grep: searchterm [file ...]\n");
        exit(1);
    } else if (argc == 2) {
        while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            grep(buffer, argv[1]);
        }
        
    } else if (argc >= 3) {
        for (int i = 2; i < argc; i++) {
            grepFile(argv[i], argv[1]);
        }
    }

    return 0;
}