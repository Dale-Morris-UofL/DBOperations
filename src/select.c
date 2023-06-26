#include <stdio.h>
#include <string.h>

#define MAX_DATA 100

int linearSearch(char *str, char *strArray[]);

int main(int argc, char *argv[]) {
    if (argc > 3) {
        printf("An unexpected number of arguments was given.\n");

        return 1;
    }

    FILE *inFile;
    int index = linearSearch("-i", argv);
    if (index >= 0) {
        inFile = fopen(argv[index+1], "r");
    } else {
        inFile = stdin;
    }

    if (inFile == NULL) {
        printf("This file cannot be opened.\n");
        
        return 1;
    }

    char str[MAX_DATA] = "";
    while (fgets(str, MAX_DATA - 1, inFile) != NULL) {
        printf("%s", str);
    }

    fclose(inFile);

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
