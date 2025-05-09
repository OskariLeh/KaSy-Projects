#include <stdio.h>
#include <stdlib.h>

// Function to read a file line by line and print each line
void printFile(char *fileName) {
    FILE *file = fopen(fileName, "r");
    char buffer[128];

    // Check if the file was opened successfully
    if (file == NULL) {
        fprintf(stdout, "my-cat: cannot open file '%s'\n", fileName);
        exit(1);
    }

    // Read and print each line from the file
    // If the file is empty, print an error message
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        fprintf(stdout, "%s", buffer);
    }
    if (ferror(file)) {
        fprintf(stdout, "my-cat: error reading file '%s'\n", fileName);
        fclose(file);
        exit(1);
    }
    
    fclose(file);
    return;
}

int main(int argc, char *argv[]) {
    // Check if the user provided at least one file name
   if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            printFile(argv[i]);
        }
   }

    return 0;
}
