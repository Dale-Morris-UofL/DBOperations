#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "condition.h"

#define MAX_DATA 1024

int isNumber(char *string);

int main(int argc, char *argv[]) {
    // Preset messages
    const char *usageMessage = "Usage: %s -c condition [-h] [-i input-file] [-o output-file]";
    const char *invalidConditionMessage = "Invalid condition";
    const char *unknownErrorMessage = "An unknown error has occurred";

    // Get args
    int opt;
    char *conditionString = "";
    int hasHeader = 0;
    char *inFileName = "";
    char *outFileName = "";
    FILE *inFile;
    FILE *outFile;

    while ((opt = getopt(argc, argv, ":c:hi:o:")) != -1) {
        switch (opt) {
            case 'c':
                conditionString = strdup(optarg);
                break;
            case 'h':
                hasHeader = 1;
                break;
            case 'i':
                inFileName = strdup(optarg);
                break;
            case 'o':
                outFileName = strdup(optarg);
                break;
            case ':':
                fprintf(stderr, usageMessage, argv[0]);
                return 1;
            case '?':
                fprintf(stderr, usageMessage, argv[0]);
                return 1;
        }
    }

    if ((!strcmp("", conditionString)) || (optind < argc)) {
        fprintf(stderr, usageMessage, argv[0]);
        return 1;
    }

    // Set input and output file
    if (strcmp("", inFileName)) {
        inFile = fopen(inFileName, "r");
    } else {
        inFile = stdin;
    }

    if (inFile == NULL) {
        fprintf(stderr, "File cannot be opened\n");
        fclose(inFile);

        return 1;
    }

    if (strcmp("", outFileName)) { 
        outFile = fopen(outFileName, "w");
    } else {
        outFile = stdout;
    }

    // Get condition
    char line[MAX_DATA];
    char *lineCopy;
    const char *lineDelim = ",";
    char *currentAttr;
    int attrNum;
    char *attrNumString = (char *) malloc(32 * sizeof(char));

    Condition *mainCondition;
    mainCondition = interpretCondition(conditionString);

    if (hasHeader) {
        fgets(line, MAX_DATA - 1, inFile);

        if (!isNumber(mainCondition->operand1) && ('"' != mainCondition->operand1[0] && '\'' != mainCondition->operand1[0])) {
            // Set attribute info
            lineCopy = strdup(line);
            currentAttr = strtok(lineCopy, lineDelim);
            attrNum = 0;
            while (strcmp(currentAttr, mainCondition->operand1)) {
                currentAttr = strtok(NULL, lineDelim);
                attrNum++;
            }
            free(lineCopy);

            sprintf(attrNumString, "%d", attrNum);

            // Convert operand to column number representation
            mainCondition->operand1 = (char *) malloc((2 + strlen(attrNumString)) * sizeof(char));
            strcpy(mainCondition->operand1, "#");
            strcat(mainCondition->operand1, attrNumString);
        }

        if (!isNumber(mainCondition->operand2) && ('"' != mainCondition->operand2[0] && '\'' != mainCondition->operand2[0])) {
            // Set attribute info
            lineCopy = strdup(line);
            currentAttr = strtok(line, lineDelim);
            attrNum = 0;
            while (strcmp(currentAttr, mainCondition->operand2)) {
                currentAttr = strtok(NULL, lineDelim);
                attrNum++;
            }
            free(lineCopy);

            sprintf(attrNumString, "%d", attrNum);

            // Convert operand to column number representation
            mainCondition->operand2 = (char *) malloc((2 + strlen(attrNumString)) * sizeof(char));
            strcpy(mainCondition->operand2, "#");
            strcat(mainCondition->operand2, attrNumString);
        }
    } else if ((!isNumber(mainCondition->operand1) && ('"' != mainCondition->operand1[0] && '\'' != mainCondition->operand1[0])) ||
            (!isNumber(mainCondition->operand2) && ('"' != mainCondition->operand2[0] && '\'' != mainCondition->operand2[0]))) {
        fprintf(stderr, invalidConditionMessage);

        free(attrNumString);

        Condition_destroy(mainCondition);
        free(inFile);
        free(outFile);

        return 1;
    }
    free(attrNumString);

    // Double check condition is valid
    if (('#' != mainCondition->operand1[0] &&
            !isNumber(mainCondition->operand1) && 
            (('"' != mainCondition->operand1[0] || '"' != mainCondition->operand1[-1 + strlen(mainCondition->operand1)]) && 
            ('\'' != mainCondition->operand1[0] || '\'' != mainCondition->operand1[-1 + strlen(mainCondition->operand1)]))) || 
            ('#' != mainCondition->operand2[0] && 
            !isNumber(mainCondition->operand2) && 
            (('"' != mainCondition->operand2[0] || '"' != mainCondition->operand2[-1 + strlen(mainCondition->operand2)]) && 
            ('\'' != mainCondition->operand2[0] || '\'' != mainCondition->operand2[-1 + strlen(mainCondition->operand2)]))) && 
            (!strcmp("==", mainCondition->operator) || !strcmp("<", mainCondition->operator) || !strcmp("<=", mainCondition->operator) || 
            !strcmp(">", mainCondition->operator) || !strcmp(">=", mainCondition->operator))) {
        fprintf(stderr, invalidConditionMessage);

        Condition_destroy(mainCondition);
        free(inFile);
        free(outFile);

        return 1;
    }

    // Get and print data
    char *attribute1 = (char *) malloc(256 * sizeof(char));
    char *attribute2 = (char *) malloc(256 * sizeof(char));
    int targetColumnNum;
    int currentColumnNum;
    int printable;
    while ((fgets(line, MAX_DATA - 1, inFile) != NULL) && (strcmp(line, "\n"))) {
        // Get attributes
        if ('#' == mainCondition->operand1[0]) { // operand1 is a column number
            targetColumnNum = atoi(mainCondition->operand1 + 1);
            lineCopy = strdup(line);
            currentAttr = strtok(lineCopy, lineDelim);
            while (currentColumnNum < targetColumnNum) {
                currentAttr = strtok(NULL, lineDelim);
                currentColumnNum++;
            }

            attribute1 = currentAttr;
        } else if (isNumber(mainCondition->operand1)) { // operand1 is a constant number
            strcpy(attribute1, mainCondition->operand1);
        } else if (('"' == mainCondition->operand1[0] && '"' == mainCondition->operand1[-1 + strlen(mainCondition->operand1)]) || 
                ('\'' == mainCondition->operand1[0] && '\'' == mainCondition->operand1[-1 + strlen(mainCondition->operand1)])) { // operand1 is a constant string
            strncpy(attribute1, 1 + mainCondition->operand1, -2 + strlen(mainCondition->operand1));
        } else { // operand1 is invalid
            fprintf(stderr, unknownErrorMessage);

            free(attribute1);
            free(attribute2);

            Condition_destroy(mainCondition);
            free(inFile);
            free(outFile);
            
            return 1;
        }

        if ('#' == mainCondition->operand2[0]) { // operand2 is a column number
            targetColumnNum = atoi(mainCondition->operand2 + 1);
            lineCopy = strdup(line);
            currentAttr = strtok(lineCopy, lineDelim);
            while (currentColumnNum < targetColumnNum) {
                currentAttr = strtok(NULL, lineDelim);
                currentColumnNum++;
            }

            attribute2 = currentAttr;
        } else if (isNumber(mainCondition->operand2)) { // operand2 is a constant number
            strcpy(attribute2, mainCondition->operand2);
        } else if (('"' == mainCondition->operand2[0] && '"' == mainCondition->operand2[-1 + strlen(mainCondition->operand2)]) || 
                ('\'' == mainCondition->operand2[0] && '\'' == mainCondition->operand2[-1 + strlen(mainCondition->operand2)])) { // operand2 is a constant string
            strncpy(attribute2, 1 + mainCondition->operand2, -2 + strlen(mainCondition->operand2));
        } else { // operand2 is invalid
            fprintf(stderr, unknownErrorMessage);

            free(attribute1);
            free(attribute2);

            Condition_destroy(mainCondition);
            free(inFile);
            free(outFile);
            
            return 1;
        }

        printable = 0;
        if (!strcmp(mainCondition->operator, "==")) {
            printable = !strcmp(attribute1, attribute2);
        } else if (!strcmp(mainCondition->operator, "<")) {
            if (isNumber(attribute1) && isNumber(attribute2)) {
                printable = atof(attribute1) < atof(attribute2);
            } else {
                printable = strcmp(attribute1, attribute2) < 0;
            }
        } else if (!strcmp(mainCondition->operator, "<=")) {
            if (isNumber(attribute1) && isNumber(attribute2)) {
                printable = atof(attribute1) <= atof(attribute2);
            } else {
                printable = strcmp(attribute1, attribute2) <= 0;
            }
        } else if (!strcmp(mainCondition->operator, ">")) {
            if (isNumber(attribute1) && isNumber(attribute2)) {
                printable = atof(attribute1) > atof(attribute2);
            } else {
                printable = strcmp(attribute1, attribute2) > 0;
            }
        } else if (!strcmp(mainCondition->operator, ">=")) {
            if (isNumber(attribute1) && isNumber(attribute2)) {
                printable = atof(attribute1) >= atof(attribute2);
            } else {
                printable = strcmp(attribute1, attribute2) >= 0;
            }
        } else {
            fprintf(stderr, unknownErrorMessage);

            free(attribute1);
            free(attribute2);

            Condition_destroy(mainCondition);
            free(inFile);
            free(outFile);

            return 1;
        }

        // Print the current line if it's supposed to be printed
        if (printable) {
            fprintf(outFile, "%s", line);
        }
    }
    free(attribute1);
    free(attribute2);

    // Free resources
    Condition_destroy(mainCondition);
    fclose(inFile);
    fclose(outFile);

    // Return
    return 0;
}

int isNumber(char *string) {
    if (string[0] != '-' && string[0] != '.' && !isdigit(string[0])) {
        return 0;
    }
    
    int hasDecimal = 0;
    int i;
    for (i = 1; string[i] != '\0'; i++) {
        if (string[i] != '.' && !isdigit(string[i])) {
            return 0;
        }

        if (hasDecimal && string[i] == '.') {
            return 0;
        }

        if (string[i] == '.') {
            hasDecimal = 1;
        }
    }

    return 1;
}
