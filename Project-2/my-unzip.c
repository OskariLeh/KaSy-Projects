#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// decompress the file using RLE (Run Length Encoding)
// and print the decompressed data to stdout
void deCompress(char *fileName) {
    FILE *fp = fopen(fileName, "r");
    char currentChar;
    int count;

    // Check if the file was opened successfully
    if (fp == NULL) {
        fprintf(stdout, "my-unzip: cannot open file '%s'\n", fileName);
        exit(1);
    }

    while (1) {
        if ((fread(&count, sizeof(int), 1, fp) != 1) || (fread(&currentChar, sizeof(char), 1, fp) != 1)) {
            if (feof(fp)) {
                // End of file reached
                break;
            } else {
                // Error reading the file
                printf("my-unzip: error reading file '%s'\n", fileName);
                fclose(fp);
                exit(1);
            }
        }

        // Print the current character 'count' times
        for (int i = 0; i < count; i++){
            printf("%c", currentChar);
        }
    } 
    printf("\n");
    fclose(fp);
    return;
}

int main(int argc, char *argv[]) {
    // Check if the user provided at least one file name
    if (argc == 1) {
        printf("my-unzip: file1 [file2 ...]\n");
        exit(1);
    } else {
        // Loop through each file name provided in the command line arguments and call the compress function on each
        for (int i = 1; i < argc; i++) {
            deCompress(argv[i]);
        }
    }
    return 0;
}