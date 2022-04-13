#include "header.h"

char* alignTextSpace(char* inStrings[], int lineLength) {
    char* outString;
    int charcount = 0;

    char* end = ".";
    char* space = " ";
    char* newLine = "\n";

    int bufferLen = 0;
    int outLen = 0;
    for (int i = 0; strcmp(inStrings[i], end); ++i) {
        bufferLen += strlen(inStrings[i]);
        outLen += (strlen(inStrings[i]) + 1); // Worst case: each world takes a new line.
    }

    // Merging into a buffer with space inserted.
    char* buffer = malloc(bufferLen + 2); // Saving for one space in the end.
    buffer[0] = '\0';
    for (int i = 0; strcmp(inStrings[i], end); ++i) {
        strcat(buffer, inStrings[i]);
    }
    strcat(buffer, end);
    strcat(buffer, space);

    // Insert new line to proper locations.
    outString = malloc(outLen + 1);
    outString[0] = '\0';
    char* lineStart = buffer;
    char* current = buffer;
    while (true) {
        char* nextSpace = strstr(current + 1, space);
        if (!nextSpace) {
            break;
        }
        if (nextSpace - lineStart > lineLength) {
            strncat(outString, lineStart, current - lineStart);
            strcat(outString, newLine);
            lineStart = current;
        }
        current = nextSpace;
    }
    if (current != lineStart) {
        strncat(outString, lineStart, current - lineStart);
        strcat(outString, newLine);
    }

    free(buffer);
    return outString;
}

char* alignText(char* inStrings[], int lineLength) {
    char* outString;
    int charcount = 0;

    static const char* end = ".";
    static const char* space = " ";
    static const char* newLine = "\n";

    int outLen = 0;
    for (int i = 0; strcmp(inStrings[i], end); ++i) {
        outLen += (strlen(inStrings[i]) + 1); // Worst case: each world takes a new line.
    }

    // Insert new line to proper locations.
    outString = malloc(outLen + 1);
    outString[0] = '\0';
    charcount = 0;
    for (int i = 0; strcmp(inStrings[i], end); ++i) {
        int currentLen = strlen(inStrings[i]);
        if (currentLen + charcount > lineLength) {
            strcat(outString, newLine);
            charcount = 0;
        }
        strcat(outString, inStrings[i]);
        charcount += currentLen;
    }
    strcat(outString, end);
    strcat(outString, newLine);

    return outString;
}

// Better performance
char* alignTextPerf(char* inStrings[], int lineLength) {
    char* outString;
    int charcount = 0;

    static const char* end = ".";
    static const char* space = " ";
    static const char* newLine = "\n";

    int outLen = 0;
    for (int i = 0; strcmp(inStrings[i], end); ++i) {
        outLen += (strlen(inStrings[i]) + 1); // Worst case: each world takes a new line.
    }

    // Insert new line to proper locations.
    outString = malloc(outLen + 1);
    outString[0] = '\0';
    charcount = 0;
    char* current = outString;
    for (int i = 0; strcmp(inStrings[i], end); ++i) {
        int currentLen = strlen(inStrings[i]);
        if (currentLen + charcount > lineLength) {
            strcpy(current, newLine);
            ++current;
            charcount = 0;
        }
        strcpy(current, inStrings[i]);
        charcount += currentLen;
        current += currentLen;
    }
    strcpy(current, end);
    strcpy(current + 1, newLine);

    return outString;
}

int main() {
    char* inStrings[] = {
        "This",
        " is",
        " my",
        " example",
        " for",
        " the",
        " exam",
        " question",
        " and",
        " you",
        " should",
        " try",
        " to",
        " solve",
        " it",
        " with",
        " C",
        " string",
        " library",
        "."
    };
    char* buffer = alignTextPerf(inStrings, 15);
    printf("%s", buffer);
    return 0;
}