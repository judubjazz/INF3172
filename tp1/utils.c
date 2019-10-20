#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <ctype.h>

/**
 *
 * @param filename
 * @return
 */
char *read_file(char *filename) {
    char *buffer = NULL;
    size_t string_size, read_size;
    FILE *handler = fopen(filename, "r");

    if (handler) {
        // Seek the last byte of the file
        fseek(handler, 0, SEEK_END);
        // Offset from the first to the last byte, or in other words, filesize
        string_size = ftell(handler);
        // go back to the start of the file
        rewind(handler);

        // Allocate a string that can hold it all
        buffer = (char *) malloc(sizeof(char) * (string_size + 1));

        // Read it all in one operation
        read_size = fread(buffer, sizeof(char), string_size, handler);

        // fread doesn't set it so put a \0 in the last position
        // and buffer is now officially a string
        buffer[string_size] = '\0';

        if (string_size != read_size) {
            // Something went wrong, throw away the memory and set
            // the buffer to NULL
            free(buffer);
            buffer = NULL;
        }
        // Always remember to close the file.
        fclose(handler);
    }
    return buffer;
}

///**
// *
// * @param str
// * @param seps
// * @return
// */
//char *ltrim(char *str, const char *seps){
//    size_t totrim;
//    if (seps == NULL) {
//        seps = "\t\n\v\f\r ";
//    }
//    totrim = strspn(str, seps);
//    if (totrim > 0) {
//        size_t len = strlen(str);
//        if (totrim == len) {
//            str[0] = '\0';
//        }
//        else {
//            memmove(str, str + totrim, len + 1 - totrim);
//        }
//    }
//    return str;
//}
//
///**
// *
// * @param str
// * @param seps
// * @return
// */
//char *rtrim(char *str, const char *seps){
//    int i;
//    if (seps == NULL) {
//        seps = "\t\n\v\f\r ";
//    }
//    i = strlen(str) - 1;
//    while (i >= 0 && strchr(seps, str[i]) != NULL) {
//        str[i] = '\0';
//        i--;
//    }
//    return str;
//}
//
///**
// *
// * @param str
// * @param seps
// * @return
// */
//char *trim(char *str, const char *seps){
//    return ltrim(rtrim(str, seps), seps);
//}


/**
 * if you can modify the string
 * @param str
 * @return
 */
char *trim(char *str) {
    char *end;

    // Trim leading space
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)  // All spaces?
        return str;

    int b = isspace(str[1]);
    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}


/**
 *
 * @param s
 */
void remove_spaces(char* s) {
    const char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while (*s++ = *d++);
}


int only_digits_in(char *s, char * c, int * pos){
    for (int i = 0; i < strlen(s); ++i){
        if (s[i] < '0' || s[i] > '9'){
            *c = s[i];
            *pos = i;
            return 0;
        }
    }
    return 1;
}

/**
 * Verify if a string contains only digits
 * if false assigns the error position to index
 * @param s string to be verified
 * @param index error position 
 */
int not_digits_at_index(char *s, int* index){
    for (int i = 0; i < strlen(s)-1; ++i){
        if (s[i] < '0' || s[i] > '9'){
            *index = i;
            return 0;
        }
    }
    return 1;
}

void strncpylower(char *s, const char *t, int length) {
    strncpy(s, t, length);
    int i = 0;
    while (s[i] != '\0') {
        s[i] = tolower(s[i]);
        ++i;
    }
}