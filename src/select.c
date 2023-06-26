#include <stdio.h>
#include <string.h>

#define MAX_DATA 100

int linearSearch(char *str, char *strArray[]);

int main(int argc, char *argv[]) {
    // Checks for a sensible number of arguments
    if (argc > 5) {
        printf("An unexpected number of arguments was given.\n");

        return 1;
    }

    // Initializes variables
    char str[MAX_DATA] = "";
    int index = 0;
    int hasHeader = (linearSearch("-h", argv) >= 0) ? 1 : 0;

    // Sets input file
    FILE *inFile;
    index = linearSearch("-i", argv);
    if (index >= 0) {
        if ((argc <= index+1) | (argv[index+1][0] == '-')) {
            printf("\"-i\" should be followed by a file name.\n");

            return 1;
        }

        inFile = fopen(argv[index+1], "r");
    } else {
        inFile = stdin;
    }

    if (inFile == NULL) {
        printf("File cannot be opened.\n");
        
        return 1;
    }

    // Sets output file
    FILE *outFile;
    index = linearSearch("-o", argv);
    if (index >= 0) {
        if ((argc <= index+1) | (argv[index+1][0] == '-')) {
            printf("\"-o\" should be followed by a file name.\n");

            return 1;
        }

        outFile = fopen(argv[index+1], "w");
    } else {
        outFile = stdout;
    }

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
