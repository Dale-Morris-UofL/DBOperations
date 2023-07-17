#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_DATA 100

/*
 * NOTES
 *
 * Program will work even if extra information is added in the command line with no tag.
 * E.g. select -c "salary > 2000" "this should not be here" -i ../data/employees.csv -o ../data/output.csv
 * This should probably be fixed.
 */

typedef struct Condition {
    char *operator;
    char *operand1;
    char *operand2;
} Condition;

Condition *interpretCondition(char *conditionString, int hasHeader);

Condition *copyCondition(Condition *condition);

void Condition_destroy(Condition *condition);

int isNumber(char *string);

int main(int argc, char *argv[]) {
    // Messages
    char *usageMessage = "usage: select -c condition [-h] [-i input-filename] [-o output-filename]";

    // Initializes variables for args
    int opt;
    char *conditionString = "";
    int hasHeader = 0;
    char *inFileName = "";
    char *outFileName = "";
    FILE *inFile;
    FILE *outFile;

    // Gets args
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
                fprintf(stderr, "%s", usageMessage);
                return 1;
            case '?':
                fprintf(stderr, "%s", usageMessage);
                return 1;
        }
    }

    if ((!strcmp("", conditionString)) || (optind < argc)) {
        fprintf(stderr, "%s", usageMessage);
        return 1;
    }

    // Gets condition
    Condition *mainCondition;
    mainCondition = interpretCondition(conditionString, hasHeader);
    int attribute2isConstant = isNumber(mainCondition->operand2);

    // Sets input file
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

    // Sets output file
    if (strcmp("", outFileName)) { 
        outFile = fopen(outFileName, "w");
    } else {
        outFile = stdout;
    }

    // Simplifies both header cases to a single case
    char line[MAX_DATA] = "";
    char *header = "";
    char *header2 = "";
    char *columnName = "";
    char *columnName2 = "";
    int columnNum = 0;
    int columnNum2 = 0;
    const char *headerDelim = ",";
    if (hasHeader) {
        header = fgets(line, MAX_DATA - 1, inFile);
        header2 = strdup(header);

        columnName = strtok(header, headerDelim);
        columnNum = 0;
        while (strcmp(columnName, mainCondition->operand1)) {
            columnName = strtok(NULL, headerDelim);
            columnNum++;
        }

        if (!attribute2isConstant) {
            columnName2 = strtok(header2, headerDelim);
            columnNum2 = 0;

            while (strcmp(columnName2, mainCondition->operand2)) {
                columnName2 = strtok(NULL, headerDelim);
                columnNum2++;
            }
        }
    } else {
        columnNum = atoi(strdup(mainCondition->operand1) + 1) - 1;
        if (!attribute2isConstant) {
            columnNum2 = atoi(strdup(mainCondition->operand2) + 1) - 1;
        }
    }

    // Gets and prints data
    char *attribute1 = "";
    char *attribute2 = "";
    int printable = 0;
    const char *lineDelim = ",";
    int i = 0;
    char *lineCopy = "";
    char *lineCopy2 = "";
    while ((fgets(line, MAX_DATA - 1, inFile) != NULL) && (strcmp(line, "\n"))) {
        lineCopy = strdup(line);
        lineCopy2 = strdup(line);
        
        // Gets the relevant attributes from the current line
        attribute1 = strtok(lineCopy, lineDelim);
        i = 0;
        while (i < columnNum) {
            attribute1 = strtok(NULL, lineDelim);
            i++;
        }
        lineCopy = strdup(line);

        if (attribute2isConstant) {
            attribute2 = strdup(mainCondition->operand2);
        } else {
            attribute2 = strtok(lineCopy2, lineDelim);
            i = 0;
            while (i < columnNum2) {
                attribute2 = strtok(NULL, lineDelim);
                i++;
            }
        }
        lineCopy2 = strdup(line);

        // Determines whether the current line meets the condition
        printable = 0;
        if (!strcmp(mainCondition->operator, "==")) {
            printable = !strcmp(attribute1, attribute2);
        } else if (!strcmp(mainCondition->operator, "<")) {
            printable = atoi(attribute1) < atoi(attribute2);
        } else if (!strcmp(mainCondition->operator, "<=")) {
            printable = atoi(attribute1) <= atoi(attribute2);
        } else if (!strcmp(mainCondition->operator, ">")) {
            printable = atoi(attribute1) > atoi(attribute2);
        } else if (!strcmp(mainCondition->operator, ">=")) {
            printable = atoi(attribute1) >= atoi(attribute2);
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

int isNumber(char *string) {
    int i;
    for (i = 0; string[i] != '\0'; i++) {
        if (!isdigit(string[i])) {
            return 0;
        }
    }
    return 1;
}
