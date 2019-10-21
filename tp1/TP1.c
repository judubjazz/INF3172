#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/wait.h>
#include<string.h>
#include<errno.h>
#include <pthread.h>
#include <stdbool.h>
#include "utils.c"


#define USAGE          "Le fichier test doit se nommer test.txt et se trouver à la raciner du projet."
#define MSG_OK         "Bravo! Votre Sudoku est valide!\n"
#define ERR_SIZE       "La taille de la grille de Sudoku devrait être 9x9.\n"
#define ERR_DIGIT      "La case (%d,%d) contient un caractère non-entier.\n"
#define ERR_SPEC_CHAR  "La case (%d,%d) contient un caractère spécial non admis.\n"
#define ERR_DBL        "Il y a un doublon %d dans la grille 9 x 9.\n"
#define ERR_DBL2       "Il y a un doublon %d dans une sous-grilles 3 x 3 numéro %d.\n"
#define MAX_SIZE 256
#define CHECK_ROW      0
#define CHECK_COL      1
#define CHECK_BOX      2
#define ERRNO_SIZE     0
#define ERRNO_DIGIT    1
#define ERRNO_CHAR     2
#define ERRNO_DBL      3
#define ERRNO_DBL2     4



struct thread {
    pthread_t   thread_id;        /* ID returned by pthread_create() */
    int         thread_num;       /* Application-defined thread # */
    char        **matrix;
    struct data *data;
    int         nb_sudoku;
};

struct data {
    bool ok;
    char _errno;
    int  row;
    int  col;
    int  box;
    char doublon;
};


// threads functions
static void *eval_rows(void *param);
static void *eval_cols(void *param);
static void *eval_box(void *param);


/**
 * Validates command line arguments
 * @param argc
 * @param argv
 * @param cwd
 */
void check_args(int argc, char *argv[], char *cwd){
    if (argc > 1)         handle_error(USAGE);
    // if (atoi(argv[0]) < 0) handle_error("Argument %d must be >= 0 \n");
    if (cwd == NULL)       handle_error("getcwd()");
}


/**
 * Returns the switch case of a thread number
 * @param i thread number
 * @return 0 1 or 2
 */
int case_of(int i){
    if (i==0) return 0;
    if (i==1) return 1;
    if (i>1 && i<11) return 2;
    return -1;
}


/**
 * Create a 2d matrises on heap from a file
 * Matrises must have been separated by only one space in between
 *
 * @param filename the file containing the matrises
 * @param offset the number of lines to read before reaching the next matrix
 * @param eof end of file
 * @return a new 2d matrix to be freed
 */
char ** create_matrix(const char * filename, int * offset, bool * eof) {
    char ** matrix = calloc(MAX_SIZE, sizeof(char));
    char line [MAX_SIZE];
    FILE* file = fopen(filename, "r");
    int i = 0;
    int nb_line = 0;

    // read offset lines
    for(int off = 0; off < *offset; ++off){
        fgets(line, sizeof(line), file);
    }
    while (true) {
        char * c = fgets(line, sizeof(line), file);
        if(c == NULL){
            *eof = true;
            break;
        } 
            
        if(line[0] == '\n'){
          *offset += nb_line + 1;
          break;  
        } 

        remove_spaces(line);
        trim(line);
        matrix[i] = malloc(strlen(line) * sizeof(char));
        strcpy(matrix[i], line);
        ++i;
        ++nb_line;
    }
    fclose(file);
    return matrix;
}

/**
 * free memory of a 2d matrix
 * @param matrix
 */
void free_matrix(char ** matrix){
    for (int i= 0; i<9; ++i)
        free(matrix[i]);
    free(matrix);
}


/**
 * Set sudoku box index for the 9 possible cases
 * @param box the box number where 0 is upper left, 8 bottom right
 * @param i
 * @param j
 */
void set_box_index(int box, int *i, int *j) {
    switch(box){
        case 0:
            *i = 0;
            *j = 0;
            break;
        case 3:
            *i = 3;
            *j = 0;
            break;
        case 6:
            *i = 6;
            *j = 0;
            break;
        case 1:
            *i = 0;
            *j = 3;
            break;
        case 4:
            *i = 3;
            *j = 3;
            break;
        case 7:
            *i = 6;
            *j = 3;
            break;
        case 2:
            *i = 0;
            *j = 6;
            break;
        case 5:
            *i = 3;
            *j = 6;
            break;
        case 8:
            *i = 6;
            *j = 6;
            break;
        default: handle_error("set_box_index");
    }
}

/**
 * Checks if a string of nine numbers contains a duplicated number
 *
 * @param s the string to be checked
 * @param ret the duplicate number
 * @return true if the string contains a duplicate number
 */
int contains_doublon(const char * s, char * ret){
    int flags [] = {0,0,0,0,0,0,0,0,0,0};
    for (int i = 0; i<9; ++i) {
        int n = s[i] - '0';
        if(flags[n]==1){
            *ret = n;
            return true;
        } else{
            flags[n] = 1;
        }
    }
    return false;
}


/**
 * Validates a sudoku
 *
 * @param t thread
 * @param s string
 * @param i matrix row position
 * @param j matrix colon position
 */
void validate(struct thread *t, char *s, int i, int j){
    char c = '-';
    int pos = 0;
    if (!strdigits(s, &c, &pos)) {
        t->data->ok                   = false;
        if(isalpha(c))t->data->_errno = ERRNO_DIGIT;
        else          t->data->_errno = ERRNO_CHAR;

        switch (case_of(t->thread_num)){
            case CHECK_ROW:
                t->data->row     = i;
                t->data->col     = pos;
                break;
            case CHECK_COL:
                t->data->row     = pos;
                t->data->col     = j;
                break;
            case CHECK_BOX:
                if(pos <3){
                    j += pos;  
                } else if(pos < 6){
                    i +=1;
                    j += (pos-3);
                } else {
                    i +=2;
                    j += (pos-6);
                }
                t->data->row     = i;
                t->data->col     = j;
                break;    
            default: handle_error("validate");
        }

    } else if (strlen(s) != 9){
        t->data->ok      = false;
        t->data->_errno  = ERRNO_SIZE;
        t->data->row     = i;
        t->data->col     = j;
    } else if(contains_doublon(s, &c)){
        t->data->ok      = false;
        t->data->_errno  = ERRNO_DBL;
        t->data->row     = i;
        t->data->col     = j;
        t->data->doublon = c;
    }
}

/**
 * Validates rows of a 9X9 sudoku
 *
 * @param params
 * @return a thread assigned with a code error
 */
static void *eval_rows(void *params){
    struct thread *t = params;
    char s[MAX_SIZE];
    int i = 0;
    int j = 0;

    while (t->matrix[i] != NULL){
        strcpy(s,t->matrix[i]);
        trim(s);
        validate(t, s, i, j);
        ++i;
    }
    pthread_exit(0);
}

/**
 * Validates colons of a 9X9 sudoku
 *
 * @param params
 * @return a thread assigned with a code error
 */
static void *eval_cols(void *params){
    struct thread *t = params;
    int i = 0;
    int j = 0;
    char s[MAX_SIZE];

    while (t->matrix[0][j] != 0){
        while(t->matrix[i] != NULL){
            char c = t->matrix[i][j];
            s[i] = c;
            ++i;
        }
        validate(t, s, i, j);
        i = 0;
        ++j;
    }
    pthread_exit(0);
}

/**
 * Validates a box of a 9X9 sudoku
 * up-left box is #0, down-right box is #8
 * @param params
 * @return a thread assigned with a code error
 */
static void *eval_box(void *params){
    struct thread *t = params;
    int u,v;
    set_box_index(t->data->box, &u,&v);
    char s[MAX_SIZE];

    int k = 0;
    for (int i = 0; i<3; ++i){
        for (int j = 0; j<3; ++j){
            s[k] = t->matrix[u+i][v+j];
            ++k;
        }
    }
    validate(t, s, u, v);
    if(t->data->_errno == ERRNO_DBL) t->data->_errno = ERRNO_DBL2;
    pthread_exit(0);
}


int main(int argc, char *argv[]){
    char cwd[MAX_SIZE];
    getcwd(cwd, sizeof(cwd)); 
    check_args(argc, argv, cwd);

    struct thread *t;
    pthread_t tid;
    pthread_attr_t attr;
    int status, tnum, box_index, offset = 0;
    int num_threads = 11;  
    bool eof = false;
    int nb_sudoku = 1;

    char const* const filename = strcat(cwd, "/test.txt");

    while(true){
        char ** matrix = create_matrix(filename, &offset, &eof);
        if(eof) break; 
        printf("\nÉvaluation du sudoku # %d \n\n", nb_sudoku);

        // create_threads(&t);
        // Initialize thread creation attributes
        status = pthread_attr_init(&attr);
        if (status != 0) handle_error_en(status, "pthread_attr_init");

        // Allocate memory for pthread_create() arguments
        t = calloc(num_threads, sizeof(struct thread));
        if (t == NULL) handle_error("calloc");

        // Create one thread for each 11 sudoku evaluations
        for (tnum = 0; tnum < num_threads; tnum++) {
            t[tnum].thread_num = tnum;
            t[tnum].matrix = matrix;
            t[tnum].nb_sudoku = nb_sudoku;
            t[tnum].data = malloc(sizeof(struct data));
            t[tnum].data->ok = true;

            // 3 actions possible for a thread
            switch(case_of(tnum)) {
                case CHECK_ROW: pthread_create(&t[tnum].thread_id, &attr, &eval_rows, &t[tnum]);
                                break;
                case CHECK_COL: pthread_create(&t[tnum].thread_id, &attr, &eval_cols, &t[tnum]);
                                break;
                case CHECK_BOX: t[tnum].data->box = box_index;
                                pthread_create(&t[tnum].thread_id, &attr, &eval_box, &t[tnum]);
                                box_index += 1;
                                // reset box_index for next sudoku
                                if(box_index == 9)box_index = 0;
                                break;
                default: handle_error_en(tnum, "switch thread create");
            }
        }

        // Destroy the thread attributes object, since it is no longer needed
        status = pthread_attr_destroy(&attr);
        if (status != 0) handle_error("pthread_attr_destroy");

        // Join each thread, and display its returned value
        for (tnum = 0; tnum < num_threads; tnum++) {
            status = pthread_join(t[tnum].thread_id, NULL);
            if (status != 0) handle_error("pthread_join");
        
            if (t[tnum].data->ok){
                printf(MSG_OK);
            } else {
                // 5 possible errors
                switch(t[tnum].data->_errno){
                    case ERRNO_SIZE:  printf(ERR_SIZE);
                        break;
                    case ERRNO_DIGIT: printf(ERR_DIGIT, t[tnum].data->row, t[tnum].data->col);
                        break;
                    case ERRNO_CHAR:  printf(ERR_SPEC_CHAR, t[tnum].data->row, t[tnum].data->col);
                        break;
                    case ERRNO_DBL:   printf(ERR_DBL, t[tnum].data->doublon);
                        break;
                    case ERRNO_DBL2:  printf(ERR_DBL2, t[tnum].data->doublon, t[tnum].data->box);
                        break;
                    default:handle_error("switch pthread join");
                }
            }
            free(t[tnum].data);
        }
        // free assigned memory to thread info
        free_matrix(matrix);
        free(t);
        ++nb_sudoku;
    }
    return 0;
}

