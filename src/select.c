#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int linearSearch(char *str, char *strArray[]);

Condition *interpretCondition(char *conditionString, int hasHeader);

Condition *copyCondition(Condition *condition);

void Condition_destroy(Condition *condition);

int main(int argc, char *argv[]) {
    // Checks for a sensible number of arguments
    if (argc > 8) {
        fprintf(stderr, "An unexpected number of arguments was given.\n");

        return 1;
    }

    // Initializes variables
    int index = 0;
    int hasHeader = (linearSearch("-h", argv) >= 0) ? 1 : 0;
    char line[MAX_DATA] = "";

    // Gets condition
    Condition *mainCondition;
    char *conditionString = "";
    index = linearSearch("-c", argv);
    if (index >= 0) {
        if ((argc <= index+1) || (argv[index+1][0] == '-')) {
            fprintf(stderr, "\"-c\" should be followed by a condition.\n");

            return 1;
        }

        conditionString = argv[index+1];
    } else {
        fprintf(stderr, "The \"-c\" tag is required.\n");

        return 1;
    }

    mainCondition = interpretCondition(conditionString, hasHeader);

    // Sets input file
    FILE *inFile;
    index = linearSearch("-i", argv);
    if (index >= 0) {
        if ((argc <= index+1) || (argv[index+1][0] == '-')) {
            fprintf(stderr, "\"-i\" should be followed by a file name.\n");

            return 1;
        }

        inFile = fopen(argv[index+1], "r");
    } else {
        inFile = stdin;
    }

    if (inFile == NULL) {
        fprintf(stderr, "File cannot be opened.\n");
        fclose(inFile);

        return 1;
    }

    // Sets output file
    FILE *outFile;
    index = linearSearch("-o", argv);
    if (index >= 0) {
        if ((argc <= index+1) || (argv[index+1][0] == '-')) {
            fprintf(stderr, "\"-o\" should be followed by a file name.\n");
            fclose(inFile);

            return 1;
        }

        outFile = fopen(argv[index+1], "w");
    } else {
        outFile = stdout;
    }

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
