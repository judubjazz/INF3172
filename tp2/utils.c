#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <ctype.h>
#include <stdbool.h>
#include <zconf.h> //lseek
#include <fcntl.h> //open

#define MAX_SIZE 512

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */


/**
 * Flushes stdin input buffer
 */
void stdin_flush(){
    char name[64];
    fseek(stdin,0,SEEK_SET);
    while(!strchr(name, '\n'))
        if(!fgets(name,(sizeof name),stdin))
            break;
}

void handle_error(char * msg){
    fprintf(stderr,BOLDRED "%s\n" RESET,msg);
    usleep(250);
}

void print(char * msg){
    printf("%s\n",msg);
}
/**
 * read a file
 * @param filename
 * @return a char buffer
 */
char *read_file(const char *filename) {
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
//        buffer[1887] = '\0';

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

void write_file(const char* filename, char * s){
    int      result;
    FILE     *handler;

    handler = fopen(filename,"w");

/* Seek to end of file */
    result = fseek(handler, 0, SEEK_END);
    if(result)handle_error("fseek");

/* Write in the next line */
    fprintf(handler, "%s\n",s);
    fclose(handler);
}


bool set_position(int fd, int bytes){
    if (lseek(fd, bytes, SEEK_SET) == -1) {
        return false;
    }
    return true;
}

bool is_locked(int fd, int bytes){
    // return 0 if unlocked
    return lockf(fd, F_TEST, bytes);
}

bool activate_lock(int fd, int bytes){
    int r = is_locked(fd,bytes);
    if(r)return false;
    return !lockf(fd, F_LOCK, bytes);
}

bool release_lock(int fd, int bytes){
    return !lockf(fd, F_ULOCK, bytes);
}


/**
 * trim fonction
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

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}

/**
 * Validates command line arguments
 * @param argc
 * @param argv
 * @param cwd
 */
void check_args(int argc, char *argv[], char const* cwd){
    long v = strtol(argv[0],NULL,10);
    if (argc > 1)         handle_error("argc");
    if (v < 0)            handle_error("Argument %d must be >= 0 \n");
    if (cwd == NULL)      handle_error("getcwd()");
}