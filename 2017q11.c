#include "header.h"

char* deleteSubstring(char* source, char* substring) {
    char* substrPtr = strstr(source, substring);
    if (!substrPtr) {
        char* result = malloc(sizeof(char) * (strlen(source) + 1));
        strcpy(result, source);
        return result;
    }
    const int firstLen = substrPtr - source;
    const int substringLen = strlen(substring);
    const int secondLen = strlen(source) - firstLen - substringLen;
    char* result = malloc(sizeof(char) * (firstLen + secondLen + 1));
    strncpy(result, source, firstLen);
    strncpy(result + firstLen, source + firstLen + substringLen, secondLen);
    return result;
}

int main() {
    char* source = "My toronto";
    char* substring = "My tor";
    printf("%s\n", deleteSubstring(source, substring));
    return 0;
}