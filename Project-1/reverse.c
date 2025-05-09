#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Node structure for linked list
typedef struct Node {
    char *data;
    struct Node *next;
}Node;

// Following sources helped with the linked list implementation
// https://stackoverflow.com/questions/62648640/building-a-linked-list-of-strings
// https://www.programiz.com/dsa/linked-list
Node *createNode(char *data) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->data = (char*)malloc(sizeof(char) * (strlen(data) + 1));

    if((newNode == NULL) || (newNode->data == NULL)) {
        fprintf(stderr, "malloc failed");
        exit(1);
    }

    strcpy(newNode->data, data);
    newNode->next = NULL;
    return newNode;
}

void addNode(Node **head, Node *node) {
    if ((*head) == NULL) {
        (*head) = node;
    } else {
        node->next = (*head);
        (*head) = node;
    }
    return;
}

// Free the memory allocated for the linked list
void freeMemory(Node *head) {
    while(head != NULL) {
        Node *tmp = head;
        head = head->next;
        free(tmp->data);
        free(tmp);
    }
}

// Help for understanding double pointers: https://www.geeksforgeeks.org/c-pointer-to-pointer-double-pointer/
// Function to parse a file and create a linked list of nodes
void parseFile(char *fileName, Node **head) {
    FILE *inputFile = NULL;
    char *dataBuffer;
    size_t size = 0;
    __ssize_t read;

    inputFile = fopen(fileName, "r"); 
    if (inputFile == NULL) {
        fprintf(stderr, "error: cannot open file '%s'\n", fileName);
        freeMemory(*head);
        exit(1);
    }

    while((read = getline(&dataBuffer, &size, inputFile)) != -1) {
        Node *newNode = createNode(dataBuffer);
        addNode(head, newNode);
    }
    
    free(dataBuffer);
    fclose(inputFile);
    return;
}

// Function to parse standard input and create a linked list of nodes
void parseStdIn(Node **head) {
    char *dataBuffer;
    size_t size = 0;
    __ssize_t read;

    while((read = getline(&dataBuffer, &size, stdin)) != -1) {
        if (strcmp(dataBuffer, "\n") == 0) {
            break;
        }
        Node *newNode = createNode(dataBuffer);
        addNode(head, newNode);
    }
    
    free(dataBuffer);
    return;
}

// Write the linked list to a file
void writeFile(char *fileName, Node *head){
    FILE *outputFile = NULL;

    outputFile = fopen(fileName, "w"); 
    if (outputFile == NULL) {
        fprintf(stderr, "error: cannot open file '%s'\n", fileName);
        exit(1);
        freeMemory(head);
    }
    Node *tmp = head; 
    while(tmp != NULL) {
        fprintf(outputFile, "%s", tmp->data);
        tmp = tmp->next;
    }

    fclose(outputFile);
    return;
}


void printList(Node *head) { 
    Node *tmp = head; 
    while (tmp != NULL) { 
        fprintf(stdout, "%s", tmp->data); 
        tmp = tmp->next; 
    } 
}

int main(int argc, char *argv[]) {
    Node *head = NULL;

    if (argc > 3) {
        // 3 or more arguments is error
        fprintf(stderr, "usage: reverse <input> <output>\n");
        freeMemory(head);
        exit(1);
    } else if (argc == 3) { 
        // 3 arguments, input and output file
        // Check if input and output file are the same
        if(strcmp(argv[1], argv[2]) == 0) {
            fprintf(stderr, "Input and output file must differ");
            freeMemory(head);
            exit(1);
        }

        parseFile(argv[1], &head);
        writeFile(argv[2], head);
    } else if (argc == 2) {
        // 2 arguments, input file and output to stdout
        parseFile(argv[1], &head);
        printList(head);
    } else if (argc == 1) {
        // 1 argument, no input file, read from stdin output to stdout
        parseStdIn(&head);
        printList(head);
    } else {
        // Some other error in arguments
        fprintf(stderr, "Program ran into an unexpected error\n");
        freeMemory(head);
        exit(1);
    }

    freeMemory(head);
    exit(0);
}
