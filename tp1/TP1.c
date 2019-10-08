#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/wait.h>
#include<string.h>
#include<errno.h>
#include <pthread.h>
#include <stdbool.h>
#include "utils.c"


#define MAX_SIZE 256
#define MSG_OK       "Bravo! Votre Sudoku est valide!\n"
#define ERR_SIZE     "La taille de la grille de Sudoku devrait être 9x9.\n"
#define ERR_IS_DIGIT "La case (%d,%d) contient un caractère non-entier.\n"
#define ERR_SPEC_CHAR  "La case (%d,%d) contient un caractère spécial non admis.\n"
#define ERR_DBL      "Il y a un doublon %d dans la grille 9 x 9.\n"
#define ERR_DBL2     "Il y a un doublon %d dans une sous-grilles 3 x 3\n"
#define ERRNO_SIZE     0
#define ERRNO_IS_DIGIT 1
#define ERRNO_CHAR     2
#define ERRNO_DBL      3
#define ERRNO_DBL2     4
#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)



/* Used as argument to thread_start() */
struct thread {
    pthread_t   thread_id;        /* ID returned by pthread_create() */
    int         thread_num;       /* Application-defined thread # */
    char        **matrix;
    struct data **data;
};

struct data {
    bool ok;
    char *msg;
    char _errno;
    int  row;
    int  col;
};




// threads functions
static void *eval_rows(void *param);
static void *eval_cols(void *param);
static void *eval_box(void *param);

void check_args(int argc, char *argv[], char *cwd){
    if (argc != 2)         handle_error("usage: a.out <integer value>\n");
    if (atoi(argv[1]) < 0) handle_error("Argument %d must be >= 0 \n");
    if (cwd == NULL)       handle_error("getcwd()");
}

int case_of(int i){
    if (i==0) return 0;
    if (i==1) return 1;
    if (i>1 && i<11) return 2;
    return -1;
}

char ** create_matrix(const char * filename) {
    char ** matrix = calloc(MAX_SIZE, sizeof(char));
    char line [MAX_SIZE];
    FILE* file = fopen(filename, "r");
    int i = 0;
    while (fgets(line, sizeof(line), file)) {
        remove_spaces(line);
        trim(line);
        matrix[i] = malloc(strlen(line) * sizeof(char));
        strcpy(matrix[i], line);
        ++i;
    }
    fclose(file);
    return matrix;
}

void free_matrix(char ** matrix){
    for (int i= 0; i<9; ++i)
        free(matrix[i]);
    free(matrix);
}


int main(int argc, char *argv[]){
    char cwd[MAX_SIZE];
    getcwd(cwd, sizeof(cwd)); 
    check_args(argc, argv, cwd);

    struct thread *t;
    pthread_t tid;                                                                               // the thread identifier */
    pthread_attr_t attr;

    int status;
    int tnum;
    int box_index = 0;
    int num_threads = 11;  // set of attributes for the thread */
    void *res;
    
    // TODO put test.txt in root project folder                                                                 
    char const* const filename = strcat(cwd, "/test/mytest.txt");

    char ** matrix = create_matrix(filename);

    // Initialize thread creation attributes
    status = pthread_attr_init(&attr);
    if (status != 0) handle_error_en(status, "pthread_attr_init");

    // Allocate memory for pthread_create() arguments
    t = calloc(num_threads, sizeof(struct thread));
    if (t == NULL) handle_error("calloc");
    size_t test = sizeof(struct data*);
    size_t test1 = sizeof(int);
    // t->data = malloc(sizeof(struct data*));
        // t->data = calloc(num_threads, sizeof(int));

    // Create one thread for each 11 sudoku evaluations
    for (tnum = 0; tnum < num_threads; tnum++) {
        t[tnum].thread_num = tnum;
        t[tnum].matrix = matrix;
        // main thread info
        t[tnum].data = calloc(11, sizeof(struct data*));
        t[tnum].data[tnum] = malloc(sizeof(struct data));
        t[tnum].data[tnum]->ok = true;
        t[tnum].data[tnum]->msg = MSG_OK;

        // 3 actions possible for a thread
        switch(case_of(tnum)) {
            case 0: pthread_create(&t[tnum].thread_id, &attr, &eval_rows, &t[tnum]);
                    break;
            case 1: pthread_create(&t[tnum].thread_id, &attr, &eval_cols, &t[tnum]);
                    break;
            case 2: t[tnum].data[tnum]->row = box_index;
                    pthread_create(&t[tnum].thread_id, &attr, &eval_box, &t[tnum]);
                    box_index += 1;
                    break;
            default: handle_error_en(tnum, "switch thread create");
        }
    }

    // Destroy the thread attributes object, since it is no longer needed
    status = pthread_attr_destroy(&attr);
    if (status != 0) handle_error_en(status, "pthread_attr_destroy");

    // Join each thread, and display its returned value
    for (tnum = 0; tnum < num_threads; tnum++) {
        status = pthread_join(t[tnum].thread_id, NULL);
        if (status != 0) handle_error_en(status, "pthread_join");
        // free(res);
    }

    if (t->data[tnum]->ok){
        printf(MSG_OK);
    } else {
    // 5 possible errors
    switch(t->data[tnum]->_errno){
        case ERRNO_SIZE: printf(t->data[tnum]->msg);
            break;
        case ERRNO_IS_DIGIT: printf(t->data[tnum]->msg, t->data[tnum]->row, t->data[tnum]->col);
            break;
        case ERRNO_CHAR: printf(t->data[tnum]->msg);
            break;
        case ERRNO_DBL: printf(t->data[tnum]->msg);
            break;
        case ERRNO_DBL2: printf(t->data[tnum]->msg);
            break;
        default:handle_error("switch pthread join");
    }
}

    // free assigned memory to thread info
    free(t);
    free_matrix(matrix);
    return 0;
}

/**
 *
 * @param params
 * @return
 */
static void *eval_rows(void *params){
    struct thread *t = params;
    char s[MAX_SIZE];
    int i,j = 0;

    while (t->matrix[i] != NULL){
        strcpy(s,t->matrix[i]);
        trim(s);
        if (!not_digits_at_index(s,&j)) {
            t->data[t->thread_num]->ok   = false;
            t->data[t->thread_num]->msg  = ERR_IS_DIGIT;
            t->data[t->thread_num]->_errno = ERRNO_IS_DIGIT;
            t->data[t->thread_num]->row = i;
            t->data[t->thread_num]->col = j;
            break;
        } else if (strlen(s) != 9){
            t->data[t->thread_num]->ok   = false;
            t->data[t->thread_num]->msg  = ERR_SIZE;
            t->data[t->thread_num]->_errno = ERRNO_SIZE;
            t->data[t->thread_num]->row = i+1;
            t->data[t->thread_num]->col = i;
            break;
        }
        ++i;
    }
    pthread_exit(0);
}

/**
 *
 * @param params
 * @return
 */
static void *eval_cols(void *params){
    struct thread *t = params;
    int i,j = 0;
    char s[MAX_SIZE];

    while (t->matrix[0][j] != 0){
        while(t->matrix[i] != NULL){
            char c = t->matrix[i][j];
            s[i] = c;
            ++i;
        }
        if (!only_digits_in(s)) {
            t->data[t->thread_num]->ok   = false;
            t->data[t->thread_num]->msg  = ERR_SPEC_CHAR;
            t->data[t->thread_num]->_errno = ERRNO_CHAR;
            t->data[t->thread_num]->row = j;
            t->data[t->thread_num]->col = i;
            break;
        } else if (strlen(s) != 9){
            t->data[t->thread_num]->ok   = false;
            t->data[t->thread_num]->msg  = ERR_SPEC_CHAR;
            t->data[t->thread_num]->_errno = ERRNO_CHAR;
            t->data[t->thread_num]->row = j;
            t->data[t->thread_num]->col = i;
            break;
        }
        i = 0;
        ++j;
    }
    pthread_exit(0);
}

/**
 *
 * @param params
 * @return
 */
static void *eval_box(void *params){
    struct thread *t = params;
    int index, u, v = 0;
    set_box_index(t->data[t->thread_num]->row, &u,&v);
    char s[MAX_SIZE];

    for (int i = 0; i<2; ++i){
        for (int j = 0; j < 2; ++j){
            s[i] = t->matrix[u+i][v+i];
        }
    }
    if (!only_digits_in(s)) {
        t->data[t->thread_num]->ok   = false;
        t->data[t->thread_num]->msg  = "allo ca va";
        t->data[t->thread_num]->_errno = ERRNO_CHAR;
        t->data[t->thread_num]->row = 0;
        t->data[t->thread_num]->col = 0;    
    } else if (strlen(s) != 9){
        t->data[t->thread_num]->ok   = false;
        t->data[t->thread_num]->msg  = ERR_SPEC_CHAR;
        t->data[t->thread_num]->_errno = ERRNO_CHAR;
        t->data[t->thread_num]->row = 0;
        t->data[t->thread_num]->col = 0;
    }
    pthread_exit(0);
}

/**
 * Set sudoku box index for the 9 possible cases
 * @ param box the box number where 0 is upper left, 8 bottom right
 */
void set_box_index(int box, int *i, int *j) {
    switch(box){
        case 0: *i = *j = 0;
                break;
        case 1: *i = 3; 
                *j = 0;
                break;
        case 2: *i = 6; 
                *j = 0;
                break;
        case 3: *i = 0; 
                *j = 3;
                break;
        case 4: *i = 3; 
                *j = 3;
                break;
        case 5: *i = 6; 
                *j = 3;
                break;
        case 6: *i = 0; 
                *j = 6;
                break;
        case 7: *i = 3; 
                *j = 6;
                break;
        case 8: *i = 6; 
                *j = 6;
                break;
        default: handle_error("set_box_index");    
    }
}


