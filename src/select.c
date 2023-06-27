#include <stdio.h>
#include <string.h>

#define MAX_DATA 100

/*
 * NOTES
 *
 * Program will work even if extra information is added in the command line with no tag.
 * E.g. select -c "salary > 2000" -h this-should-not-be-here -i ../data/employees.csv -o ../data/output.csv
 * This should probably be fixed.
 */

int linearSearch(char *str, char *strArray[]);

int main(int argc, char *argv[]) {
    // Checks for a sensible number of arguments
    if (argc > 8) {
        fprintf(stderr, "An unexpected number of arguments was given.\n");

        return 1;
    }

    // Initializes variables
    int i = 0;
    char str[MAX_DATA] = "";
    int index = 0;
    int hasHeader = (linearSearch("-h", argv) >= 0) ? 1 : 0;

    // Gets condition
    char *condition = "";
    index = linearSearch("-c", argv);
    if (index >= 0) {
        if ((argc <= index+1) || (argv[index+1][0] == '-')) {
            fprintf(stderr, "\"-c\" should be followed by a condition.\n");

            return 1;
        }

        condition = argv[index+1];
    } else {
        fprintf("The \"-c\" tag is required.\n");

        return 1;
    }

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

    // Temporary code for debugging purposes
    printf("hasHeader: %d\n", hasHeader);
    printf("condition: %s\n", condition);

    // Gets and prints data
    while ((fgets(str, MAX_DATA - 1, inFile) != NULL) && (strcmp(str, "\n"))) {
        fprintf(outFile, "%s", str);
    }

    // Frees resources
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
