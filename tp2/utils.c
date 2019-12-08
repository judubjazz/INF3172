#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <ctype.h>
#include <stdbool.h>
#include <zconf.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h> //flock


#define MAX_SIZE 512
#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
               do { fprintf(stderr,"%s\n",msg); exit(EXIT_FAILURE); } while (0)

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

char intochar(int i){
    return i +'0';
}


void test_lock(int fd1, const char *filename){

    int fd = open(filename, O_RDWR | O_CREAT, 0666);
    int status = lockf(fd,F_TEST,1000);
    fprintf(stderr,"file descriptor: %d\n",fd);
    fprintf(stderr,"\nstatus : %d\n", status);

    struct flock test;

    test.l_type = F_WRLCK;
    test.l_whence = SEEK_SET;
    test.l_start = 0;
    test.l_len = 0;
    test.l_pid = -1;

    fcntl(fd, F_GETLK, &test);

    if (test.l_type == F_UNLCK) {
        printf("WriteLock would fail. F_GETLK returned:\n");
    }
    else {
        printf("F_WRLCK - Lock would succeed\n");
    }
}

void test_lock2(int fd1, const char *filename){

    int fd = open(filename, O_RDWR | O_CREAT, 0666);
    int status = lockf(fd,F_TEST,1000);
    fprintf(stderr,"file descriptor: %d\n",fd);
    fprintf(stderr,"\nstatus : %d\n", status);

    struct flock test;

    test.l_type = F_RDLCK;
    test.l_whence = SEEK_SET;
    test.l_start = 0;
    test.l_len = 0;
    test.l_pid = -1;

    fcntl(fd, F_GETLK, &test);

    if (test.l_type == F_UNLCK) {
        printf("ReadLock would fail. F_GETLK returned:\n");
    }
    else {
        printf("F_WRLCK - Lock would succeed\n");
    }
}



bool is_locked(const char *filename, int fd){
    struct flock test;
    test.l_type = F_WRLCK;
    test.l_whence = SEEK_SET;
    test.l_start = 0;
    test.l_len = 0;
    test.l_pid = -1;

    fcntl(fd, F_GETLK, &test);

    if (test.l_type == F_UNLCK) {
        printf("UNLOCK\n");
        return false;
    }
    else {
        printf("LOCK\n");
        return true;
    }
}

bool activate_lock(int type, const char *filename, int *fd, struct flock fl, int start, int end){
    *fd = open(filename, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if(is_locked(filename, *fd)){
        return false;
    };

    fl.l_type = type;
    fl.l_start = start;
    fl.l_len = end;

    fcntl (*fd, F_SETLKW, &fl);
    return true;
}


int lock_to_delete(const char *filename, int n, void (*delete)(FILE *, FILE *, int)){
    int fd;
    struct flock fl;
    FILE * fileptr1;
    FILE * fileptr2;


    //Initialize the flock structure.
    memset (&fl, 0, sizeof(fl));
    if(!activate_lock(F_WRLCK, filename, &fd, fl, 0, 0)) return 0;
    fileptr1= fopen(filename, "r");
    fileptr2 = fopen("replica.txt", "w");


//  start deleting ************
    (*delete)(fileptr1,fileptr2,n);
// ************


    printf ("locked; hit Enter to unlock... \n");
    /* Wait for the user to hit Enter. */
    getchar();
    printf ("unlocking\n");

    remove(filename);
    rename("replica.txt", filename);

    // Release the fl
    fl.l_type = F_UNLCK;
    fcntl (fd, F_SETLKW, &fl);
    close (fd);
    fclose(fileptr1);
    fclose(fileptr2);
    return 1;
}


int lock_to_update(const char *filename, int philospher_id, char *name, char *action, void (*update)(FILE *, FILE *, int, char *, char *)){
    int fd;
    struct flock fl;
    FILE * fileptr1;
    FILE * fileptr2;
    int philosophes_lines[10];
    int i = 0;
    char line[MAX_SIZE];

    // prevent other thread from aquiring a writing lock
    memset (&fl, 0, sizeof(fl));
    if(!activate_lock(F_RDLCK, filename, &fd, fl, 0, 0))return 0;

    // seek philosophers lines
    fileptr1 = fopen(filename, "r");
    for(int no_line=1;fgets(line, sizeof(line),fileptr1) != NULL ;++no_line){
        if(line[0]==(philospher_id + '0')){
            philosophes_lines[i]= no_line;
            ++i;
        }
    }

    //release read lock
    fl.l_type = F_UNLCK;
    fcntl (fd, F_SETLKW, &fl);

    // activate write locks on fields
    int offset = strlen(line);
    for(i = 0; i < 10; ++i){
        int philosophe_line = philosophes_lines[i];
        int start = philosophe_line * offset;
        int end = start + offset;
        if(!activate_lock(F_WRLCK, filename, &fd, fl, start,end))return 0;
    }

    // update fields
    fileptr2 = fopen("replica.txt", "w");
    rewind(fileptr1);

    (*update)(fileptr1,fileptr2,philospher_id, name, action);

    /* Wait for the user to hit Enter. */
    printf ("Enter to unlock... \n");
    getchar();
    getchar();
    printf ("unlocking\n");

    remove(filename);
    rename("replica.txt", filename);

    // Release locks
    fl.l_type = F_UNLCK;
    fcntl (fd, F_SETLKW, &fl);
    close (fd);
    fclose(fileptr1);
    fclose(fileptr2);
    return 1;
}


/**
 *
 * @param str
 * @param seps
 * @return
 */
char *ltrim(char *str, const char *seps){
    size_t totrim;
    if (seps == NULL) {
        seps = "\t\n\v\f\r ";
    }
    totrim = strspn(str, seps);
    if (totrim > 0) {
        size_t len = strlen(str);
        if (totrim == len) {
            str[0] = '\0';
        }
        else {
            memmove(str, str + totrim, len + 1 - totrim);
        }
    }
    return str;
}

/**
 *
 * @param str
 * @param seps
 * @return
 */
char *rtrim(char *str, const char *seps){
    int i;
    if (seps == NULL) {
        seps = "\t\n\v\f\r ";
    }
    i = strlen(str) - 1;
    while (i >= 0 && strchr(seps, str[i]) != NULL) {
        str[i] = '\0';
        i--;
    }
    return str;
}

/**
 * trim fonction if you cannot modify the string
 * @param str
 * @param seps
 * @return
 */
char *trim(char *str, const char *seps){
    return ltrim(rtrim(str, seps), seps);
}


/**
 * trim fonction
 * if you can modify the string
 * @param str
 * @return
 */
char *trim2(char *str) {
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
 * remove spaces from a string
 * @param s the string to be trimed
 */
void remove_spaces(char* s) {
    const char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while ((*s++ = *d++));
}


/**
 *
 * @param s
 * @param c
 * @param pos
 * @return
 */
int strdigits(char *s, char *c, int *pos){
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

/**
 * Validates command line arguments
 * @param argc
 * @param argv
 * @param cwd
 */
void check_args(int argc, char *argv[], char *cwd){
    if (argc > 1)         handle_error("argc");
    // if (atoi(argv[0]) < 0) handle_error("Argument %d must be >= 0 \n");
    if (cwd == NULL)       handle_error("getcwd()");
}