#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DATA 100

typedef struct Condition {
    char *operator;
    char *operand1;
    char *operand2;
} Condition;

int badArgs(int argc, char *argv[]);

int *findTags(char *argv[]);

int linearSearch(char *str, char *strArray[]);

Condition *interpretCondition(char *conditionString, int hasHeader);

Condition *copyCondition(Condition *condition);

void Condition_destroy(Condition *condition);

int main(int argc, char *argv[]) {
    // Checks args
    int argCheck = badArgs(argc, argv);
    switch (argCheck) {
        case 1:
            fprintf(stderr, "There was an unexpected number of args.\n");
            return 1;
        case 2:
            fprintf(stderr, "The \"-c\" tag must be included.\n");
            return 1;
        case 3:
        case 4:
        case 5:
            fprintf(stderr, "Incorrect form. Try \"select -c <condition> [-h] [-i input-file] [-o output-file]\".\n");
            return 1;
    }
    int *tagIndices = findTags(argv);

    // Initializes variables
    int hasHeader = (tagIndices[1] >= 0) ? 1 : 0;
    char line[MAX_DATA] = "";

    int temp = 0; // Debug
    for (temp = 0; temp < 4; temp++) { // Debug
        printf("Tag index %d: %d\n", temp, tagIndices[temp]);
    }
    // Gets condition
    Condition *mainCondition;
    char *conditionString = argv[tagIndices[0]+1];
    mainCondition = interpretCondition(conditionString, hasHeader);

    for (temp = 0; temp < 4; temp++) { // Debug
        printf("Tag index %d: %d\n", temp, tagIndices[temp]);
    }
    printf("After condition, before input\n"); // Debug
    printf("Index of -i: %d\n", tagIndices[2]); // Debug
    printf("Input file: %s\n", argv[4 + 1]); // Debug
    // Sets input file
    FILE *inFile;
    if (tagIndices[2] >= 0) {
        inFile = fopen(argv[tagIndices[2]+1], "r");
    } else {
        inFile = stdin;
    }

    if (inFile == NULL) {
        fprintf(stderr, "File cannot be opened.\n");
        fclose(inFile);

        return 1;
    }

    printf("After input, before output\n"); // Debug

    // Sets output file
    FILE *outFile;
    if (tagIndices[3] >= 0) {
        outFile = fopen(argv[tagIndices[3]+1], "w");
    } else {
        outFile = stdout;
    }

    printf("After output, end of test 1\n"); // Debug

    // Temporary code for debugging purposes
    printf("hasHeader: %d\n", hasHeader);
    printf("operator: %s\noperand1: %s\noperand2: %s\n", mainCondition->operator, mainCondition->operand1, mainCondition->operand2);

    // Simplifies both header cases to a single case
    char *header = "";
    char *columnName = "";
    int columnNum = 0;
    const char *headerDelim = ",";
    if (hasHeader) {
        header = fgets(line, MAX_DATA - 1, inFile);

        columnName = strtok(header, headerDelim);
        columnNum = 0;
        while (strcmp(columnName, mainCondition->operand1)) {
            columnName = strtok(NULL, headerDelim);
            columnNum++;
        }
    } else {
        columnNum = atoi(strdup(mainCondition->operand1) + 1) - 1;
    }

    // For debugging
    printf("%s is column number %d\n", columnName, columnNum+1);

    // Gets and prints data
    char *attribute = "";
    int printable = 0;
    const char *lineDelim = ",";
    int i = 0;
    char *lineCopy = "";
    while ((fgets(line, MAX_DATA - 1, inFile) != NULL) && (strcmp(line, "\n"))) {
        lineCopy = strdup(line);
        
        // Gets the relevant attribute from the current line
        attribute = strtok(lineCopy, lineDelim);
        i = 0;
        while (i < columnNum) {
            attribute = strtok(NULL, lineDelim);
            i++;
        }
        lineCopy = strdup(line);

        // Determines whether the current line meets the condition
        printable = 0;
        if (!strcmp(mainCondition->operator, "==")) {
            printable = !strcmp(attribute, mainCondition->operand2);
        } else if (!strcmp(mainCondition->operator, "<")) {
            printable = atoi(attribute) < atoi(mainCondition->operand2);
        } else if (!strcmp(mainCondition->operator, "<=")) {
            printable = atoi(attribute) <= atoi(mainCondition->operand2);
        } else if (!strcmp(mainCondition->operator, ">")) {
            printable = atoi(attribute) > atoi(mainCondition->operand2);
        } else if (!strcmp(mainCondition->operator, ">=")) {
            printable = atoi(attribute) >= atoi(mainCondition->operand2);
        } else {
            printable = 0;
        }

        // Prints the current line if it's supposed to be printed
        if (printable) {
            fprintf(outFile, "%s", line);
        }
    }

    // Frees resources
    Condition_destroy(mainCondition);
    fclose(inFile);
    fclose(outFile);

    // Returns
    return 0;
}

/*
 * Returns:
 * 0 if args are good
 * 1 if there are too many args
 * 2 if there is no -c tag
 * 3 if there are duplicate tags
 * 4 if there is a -c, -i, or -o tag with no corresponding non-tag arg
 * 5 if there is a non-tag arg with no corresponding tag
 */
int badArgs(int argc, char *argv[]) {
    int i = 0;

    if (argc > 8) {
        return 1;
    }
    
    int hasCondition = 0;
    int hasHeader = 0;
    int hasInput = 0;
    int hasOutput = 0;
    char *previousArg = strdup(argv[0]);
    for (i = 1; i < argc; i++) {
        if (!strcmp("-c", argv[i])) {
            if (hasCondition) {
                return 3;
            }

            if (!strcmp("-i", previousArg) || !strcmp("-o", previousArg)) {
                return 4;
            }

            hasCondition = 1;
            previousArg = strdup(argv[i]);
            continue;
        }

        if (!strcmp("-h", argv[i])) {
            if (hasHeader) {
                return 3;
            }

            hasHeader = 1;
            previousArg = strdup(argv[i]);
            continue;
        }

        if (!strcmp("-i", argv[i])) {
            if (hasInput) {
                return 3;
            }

            if (!strcmp("-c", previousArg) || !strcmp("-o", previousArg)) {
                return 4;
            }

            hasInput = 1;
            previousArg = strdup(argv[i]);
            continue;
        }

        if (!strcmp("-o", argv[i])) {
            if (hasOutput) {
                return 3;
            }

            if (!strcmp("-c", previousArg) || !strcmp("-i", previousArg)) {
                return 4;
            }

            hasOutput = 1;
            previousArg = strdup(argv[i]);
            continue;
        }

        if (strcmp("-c", previousArg) && strcmp("-i", previousArg) && strcmp("-o", previousArg)) {
            return 5;
        }

        previousArg = strdup(argv[i]);
    }
    if (!hasCondition) {
        return 2;
    }
    if (!strcmp("-c", previousArg) || !strcmp("-i", previousArg) || !strcmp("-o", previousArg)) {
        return 4;
    }

    return 0;
}

int *findTags(char *argv[]) {
    int tagIndices[4];

    tagIndices[0] = linearSearch("-c", argv);
    tagIndices[1] = linearSearch("-h", argv);
    tagIndices[2] = linearSearch("-i", argv);
    tagIndices[3] = linearSearch("-o", argv);

    int *tagPointer = tagIndices;
    return tagPointer;
}

// Basic linear search for an array of strings
int linearSearch(char *str, char *strArray[]) {
    int i = 0;
    while (strArray[i] != NULL) {
        if (!strcmp(str, strArray[i])) {
            return i;
        }

        i++;
    }

    return -1;
}

Condition *interpretCondition(char *conditionString, int hasHeader) {
    Condition *condition = malloc(sizeof(Condition));
    char *newConditionString = strdup(conditionString);
    const char *delim = " ";

    condition->operand1 = strtok(newConditionString, delim);
    condition->operator = strtok(NULL, delim);
    condition->operand2 = strtok(NULL, delim);

    return condition;
}

Condition *copyCondition(Condition *condition) {
    Condition *copy = malloc(sizeof(Condition));

    copy->operator = strdup(condition->operator);
    copy->operand1 = strdup(condition->operand1);
    copy->operand2 = strdup(condition->operand2);

    return copy;
}

void Condition_destroy(Condition *condition) {
    assert(condition != NULL);

    condition->operator = NULL;
    condition->operand1 = NULL;
    condition->operand2 = NULL;

    free(condition->operator);
    free(condition->operand1);
    free(condition->operand2);

    free(condition);
}
