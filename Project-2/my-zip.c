#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Use RLE (Run Length Encoding) to compress the file
 and write the compressed data to stdout*/
void compress(char *fileName) {
    FILE *fp = fopen(fileName, "r");
    char *buffer = NULL;
    size_t size = 0;
    __ssize_t read;

    // Check if the file was opened successfully
    if (fp == NULL) {
        fprintf(stdout, "my-zip: cannot open file '%s'\n", fileName);
        exit(1);
    }

    while ((read = getline(&buffer, &size, fp)) != -1) {
        char currentChar = buffer[0];
        int count = 1;

        for (int i = 1; i <= strlen(buffer); i++) {
            if(buffer[i] == currentChar) {
                count++;
            } else {
                fwrite(&count, sizeof(int), 1, stdout);
                fwrite(&currentChar, sizeof(char), 1, stdout);
                count = 1;
                currentChar = buffer[i];
            }
        }
    } 

    free(buffer);
    fclose(fp);
    return;
}

int main(int argc, char *argv[]) {
    // Check if the user provided at least one file name
    if (argc == 1) {
        printf("my-zip: file1 [file2 ...]\n");
        exit(1);
    } else {
        // Loop through each file name provided in the command line arguments and call the compress function on each
        for (int i = 1; i < argc; i++) {
            compress(argv[i]);
        }
    }
    return 0;
}