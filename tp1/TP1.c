#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/wait.h>
#include<string.h>
#include<errno.h>
#include <pthread.h>


#define MAX_SIZE 256
#define ERR_IS_DIGIT "seulement des chiffres sont acceptes"

int fibs[MAX_SIZE];
void *runner(void *param); /* the thread */


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

char *trim(char *str, const char *seps){
    return ltrim(rtrim(str, seps), seps);
}

int only_digits_in(const char *s){
    char *str = malloc(sizeof(*s));
    strcpy(str, s);
    trim(str, NULL);
    while (*str) {
        if (*str < '0' || *str > '9'){
            return 0;
        }
        *str++;
    }
    return 1;
}

int main(int argc, char *argv[]){
    int i;
    pthread_t tid; /* the thread identifier */
    pthread_attr_t attr; /* set of attributes for the thread */

    if (argc != 2) {
        fprintf(stderr,"usage: a.out <integer value>\n");
        return -1;
    }

    if (atoi(argv[1]) < 0) {
        fprintf(stderr,"Argument %d must be >= 0 \n",atoi(argv[1]));
        return -1;
    }

/* get the default attributes */
    pthread_attr_init(&attr);

/* create the thread */
    pthread_create(&tid,&attr,runner,argv[1]);

/* now wait for the thread to exit */
    pthread_join(tid,NULL);

///** now output the Fibonacci numbers */
//    for (i = 0; i < atoi(argv[1]); i++)
//        printf("%d\n", fibs[i]);

    char const* const fileName = "/home/ju/Projects/CLionProjects/inf3172/tp1/test/mytest.txt";
    FILE* file = fopen(fileName, "r"); /* should check the result */
    char line[256];
    int col, row = 0;
    while (fgets(line, sizeof(line), file)) {
        /* note that fgets don't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
        if (!only_digits_in(line)) {
            perror(ERR_IS_DIGIT);
            exit(-1);
        }
        printf("%s\n", line);
        ++row;
        printf("%d\n", row);
    }

    fclose(file);
    return 0;
}

/**
 * Generate primes using the Sieve of Eratosthenes.
 */
void *runner(void *param){
    int i;
    int upper = atoi(param);

    if (upper== 0)
        pthread_exit(0);
    else if (upper == 1)
        fibs[0] = 0;
    else if (upper== 2) {
        fibs[0] = 0;
        fibs[1] = 1;
    }
    else { // sequence > 2
        fibs[0] = 0;
        fibs[1] = 1;

        for (i = 2; i < upper; i++)
            fibs[i] = fibs[i-1] + fibs[i-2];
    }

    pthread_exit(0);
}




//int main()
//{
//    int pids[10];
//    int status;
//    for(int i = 0; i < 10; i++){
//        int pid = fork();
//        pids[i] = pid;
//        if(pid == -1)
//            printf("Error in fork(): %s", strerror(errno));
//        else if(pid == 0){
//            printf("Je suis le numéro %d, mon PID est %d et mon père est %d\n", i, getpid(), getppid());
//            exit(0);
//        }
//    }
//
//    for(int i = 0; i < 10; i++){
//        int ret = waitpid(pids[i], &status, NULL);
//        if(ret == -1)
//            printf("Error in waitpid(): %s", strerror(errno));
//        else
//            printf("le processus n°%d, PID %d, vient de terminer\n", i, pids[i]);
//    }
//    return 0;
//}