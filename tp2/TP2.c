#include <pthread.h>
#include "utils.c"

#define P0                      "Socrates       "
#define P1                      "Epicurus       "
#define P2                      "Pythagoras     "
#define P3                      "Plato          "
#define P4                      "Aristotle      "
#define SPACE                   "          "
#define HEADER                  "Code          Nom                  Action\n"
#define MENU                    "\n\n1)Consulter résultat\n2)Modifier le nom d'un philosophe\n3)Supprimer le nom d'un philosophe\n4)Modifier l'action et le nom d'un philosophe\n5)Quitter\n"
#define MAN                     "\nChoisissez un numéro entre 1 et 5.\n"
#define EAT                     "mange"
#define THINK                   "pense"
#define TMP_FOLDER              "tmp.txt"
#define OUT_FOLDER              "/resultat.txt"
#define STDIN_PHILOSOPHER_ID    "Entrer le id du philosophe (0 à 4):\n"
#define STDIN_PHILOSOPHER_NAME  "Entrer le nouveau nom:\n"
#define STDIN_ACTION            "Entrer la nouvelle action du philosophe (mange ou pense):\n"
#define ERR_SCAN_NAME           "Le nom doit être entre 1 et 15 caractères.\n"
#define ERR_SCAN_ACTION         "L'action doit être mange ou pense.\n"
#define ERR_SCAN_PHILOSOPHER_ID "Le id doit être entre 0 et 4.\n"
#define ERR_LOCK_S_NAME         "Le champ <<%s>> est verrouillé, veuillez essayer plus tard.\n"
#define ERR_LOCK_S_DELETE       "Le fichier <<%s>> est verrouillé, veuillez essayer plus tard.\n"
#define ERR_LOCK_ACTION         "L'enregistrement que vous voulez accéder est verrouillé, veuillez essayer plus tard.\n"
#define SELECT        1
#define UPDATE_NAME   2
#define DELETE        3
#define UPDATE_ACTION 4
#define QUIT          5
#define TEST          6
#define NAME          0
#define ACTION        1
#define MAX_THREADS   5
#define SLEEP_TIME    0
#define SIZE_LINE     42
#define SIZE_FILE     2142
#define SIZE_NAME     15
#define SIZE_ACTION   6
#define SIZE_SPACE    10
#define N             5
#define THINKING      2
#define HUNGRY        1
#define EATING        0
#define LEFT \
    (philosopher_id + 4) % N
#define RIGHT \
    (philosopher_id + 1) % N


//implicit declaration
void *cogitate(void * param);
void pickup_forks(int philosopher_id);
void return_forks(int philosopher_id);
void menu();

pthread_mutex_t mutex;
pthread_cond_t Signal[N];
char const* filename;
char buffer[MAX_SIZE*5];
int state[N];
int phil[N] = { 0, 1, 2, 3, 4 };


/**
 * Inner function to test locks
 */
void test(){
    int fd = open(filename,O_RDWR,0644);

    print("offset");
    char start[8];
    char end[8];
    long s,e;
    fgets(start, 8, stdin);
    s = strtol(start, NULL, 10);
    if(s==100)exit(0);

    print("bytes 1=SIZE_LINE 2=SIZE_NAME default=STDIN");
    fgets(end, 8, stdin);
    e = strtol(end, NULL, 10);
    if(e==1)e =SIZE_LINE;
    else if (e==2) e = SIZE_NAME;

    lseek(fd,s, SEEK_SET);
    bool b = is_locked(fd,e);
    if(b)print("file is locked");
    else print("file is unlocked");
    test();
}

/**
 * Selects a philosopher name by id
 * @param id
 * @return
 */
char * philosopher(int id){
    switch (id){
        case 0:return P0;
        case 1:return P1;
        case 2:return P2;
        case 3:return P3;
        case 4:return P4;
        default:
            handle_error("philosopher 105\n");
            return NULL;
    }
}

/**
 * Select a philosopher action
 * @param c
 * @return
 */
char *get_action(char c) {
    return c == 'm' ? EAT : THINK;
}

/**
 * Scans a stdin menu option
 * @param o
 */
void scan_option(long *o){
    printf(MENU);
    char option[64];
    char * endptr;
    fgets(option, 64, stdin);
    *o = strtol(option, &endptr, 10);
    if(option == endptr)*o =-1;
}

/**
 * Scans a stdin philosopher name
 * @param name
 * @return
 */
bool scan_name(char *name){
    printf(STDIN_PHILOSOPHER_NAME);
    char padding[SIZE_NAME];
    memset(padding,' ',SIZE_NAME);
    fgets(name,MAX_SIZE,stdin);
    int l = strlen(name)-1;
    if(!l || l > SIZE_NAME){
        return false;
    }
    // resize name to fit columns in the file
    name[strlen(name)-1]=' ';
    strcat(name,padding);
    name[SIZE_NAME] = '\0';
    return true;
}

/**
 * Scans a stdin philosopher name
 * @param action
 * @return
 */
bool scan_action(char * action){
    printf(STDIN_ACTION);
    fgets(action,MAX_SIZE,stdin);
    trim(action);
    if (strcmp(action, THINK) !=0 && strcmp(action, EAT) !=0){
        return false;
    }
    action[5] = '\0';
    return true;
}

/**
 * Scans a stdin philosopherID
 * @param id
 * @return
 */
bool scan_id(long * id){
    printf(STDIN_PHILOSOPHER_ID);
    char option[64];
    char * endptr;
    fgets(option, 64, stdin);
    *id = strtol(option, &endptr, 10);
    if(endptr==option || *id < 0 || *id > 4) {
        return false;
    }
    return true;
}

/**
 * Appends an array with corresponding rows number matching a philosopherID
 * @param id
 * @param philosophers_lines
 */
void get_philosophers_lines(int id, int *philosophers_lines){
    FILE* fileptr;
    char line[64];
    int i = 0;

    fileptr = fopen(filename, "r");
    for(int no_line=1;fgets(line, sizeof(line),fileptr) != NULL ;++no_line){
        if(line[0]==(id + '0')){
            philosophers_lines[i]= no_line;
            ++i;
        }
    }
    fclose(fileptr);
}

/**
 * Writes in file descriptor2 the lines that does not match the philosopherId
 * On success, fd2 becomes fd.
 * @param fd
 * @param fd2
 * @param philosopher_id
 */
void delete_philosopher(int fd, int fd2, int philosopher_id){
    char buf[SIZE_FILE+1];
    char s[64];
    char * line;
    buf[SIZE_FILE] = '\0';

    read(fd,buf,SIZE_FILE);

    line = strtok(buf,"\n");
    while(line != NULL){
        if(line[0]=='C'){
            // headers
            write(fd2,HEADER,SIZE_LINE);
        } else if (line[0] != philosopher_id+'0') {
            // keep the philosopher line that doesn't match the id
            sprintf(s,"%s\n",line);
            write(fd2,s,SIZE_LINE);
        }
        // next line
        line = strtok(NULL,"\n");
    }

    remove(filename);
    rename(TMP_FOLDER, filename);
}

/**
 * Asks stdin a philosopherID and delete all lines matching it
 * Thread safe
 */
void delete(){
    int fd = open(filename, O_RDWR, 0644);
    long id;

    // lock the entire file
    if(!activate_lock(fd,SIZE_FILE)){
        fprintf(stderr,ERR_LOCK_S_DELETE,filename);
        usleep(250);
        menu();
    }
    // ask the user for a philosopher to delete
    while(!scan_id(&id)){
        handle_error(ERR_SCAN_PHILOSOPHER_ID);
    }

    // open a temporary file to paste in
    int fd2 = open(TMP_FOLDER, O_RDWR|O_CREAT|O_APPEND,0644);
    delete_philosopher(fd,fd2,id);

    // reset
    release_lock(fd, SIZE_FILE);
    close(fd2);
    close(fd);
}

/**
 * Writes in file descriptor 2, an updated lines that initially match the philosopherID
 * On success, fd2 becomes fd
 * @param fd
 * @param fd2
 * @param philosopher_id
 * @param name
 * @param action if not Null, also updates the philosopher's action
 */
void update_philosopher(int fd, int fd2, int philosopher_id, char *name, char *action){
    char * line;
    char s[SIZE_LINE];
    char buf[SIZE_FILE];
    buf[SIZE_FILE-1] = '\0';

    // rewind to read entire file
    lseek(fd,0,SEEK_SET);
    read(fd,buf,SIZE_FILE);


    // scan each lines
    line = strtok(buf,"\n");
    while(line != NULL){
        if(line[0]=='C'){
            // headers
            write(fd2,HEADER,SIZE_LINE);
        } else if (line[0] != philosopher_id+'0') {
            // lines whom are not matching the philosopherID
            sprintf(s,"%s\n",line);
            write(fd2,s,SIZE_LINE);
        } else {
            if(action == NULL) {
                // need to scan the action
                action = get_action(line[31]);
                sprintf(s, "%c%s%s%s%s\n", line[0], SPACE, name, SPACE, action);
                action = NULL;
            } else {
                // action must be updated
                sprintf(s, "%c%s%s%s%s\n", line[0], SPACE, name, SPACE, action);
            }
            write(fd2,s,SIZE_LINE);
        }
        line = strtok(NULL,"\n");
    }
}

/**
 * Asks stdin a philospherID
 * Update all lines matching it
 * Lines bytes offsets follow as ID=1 SPACE=10 NAME=15 SPACE=10 ACTION=5 EOL=1 TOTAL = 42
 * Thread safe
 * @param option (Action/Name) if option Action selected, it also updates the philosopher's action
 */
void update(int option){
    char name[64];
    char action [16];
    long id;
    int  fd;
    int  philosophers_lines[10];


    // ask the user for a philosopherID to delete
    while(!scan_id(&id)){
        handle_error(ERR_SCAN_PHILOSOPHER_ID);
    }

    // map philosopherIDs lines to their rows number
    get_philosophers_lines(id, philosophers_lines);

    // lock fields
    fd = open(filename, O_RDWR|O_CREAT|O_APPEND,0644);
    for(int i = 0; i < 10; ++i){
        // calculate offset that match a philosopherID line
        int philosopher_line = philosophers_lines[i];
        int offset = philosopher_line * SIZE_LINE - SIZE_LINE;
        // move the cursor from the beginning of the file to the offset
        lseek(fd,offset,SEEK_SET);

//        // test the offset, it should read the line of the queried philosopher
//        printf("offset is %i\n", offset);
//        char t[SIZE_LINE];
//        read(fd,t,SIZE_LINE);
//        printf("%s",t);

        switch(option){
            case ACTION:
                // lock the entire line
                if(!activate_lock(fd,SIZE_LINE)){
                    handle_error(ERR_LOCK_ACTION);
                    menu();
                }
                break;
            case NAME:
                // move cursor on the name field (SEEK_CUR+ID+SPACE)
                lseek(fd, 1+SIZE_SPACE, SEEK_CUR);
                // only the name field is locked
                if(!activate_lock(fd,SIZE_NAME)){
                    read(fd,name,SIZE_NAME);
                    strtok(name," ");
                    fprintf(stderr,ERR_LOCK_S_NAME,name);
                    menu();
                }
                break;
            default:
                handle_error(ERR_LOCK_ACTION);
                menu();
        }
    }

    // ask the user for the philosopher's new name
    while(!scan_name(name)){
        handle_error(ERR_SCAN_NAME);
    }

    // ask the user for the philosopher's new action
    if(option == ACTION){
        while(!scan_action(action)){
            handle_error(ERR_SCAN_ACTION);
        }
    }

    // create a temporary folder to paste in
    int fd2 = open(TMP_FOLDER, O_RDWR|O_CREAT|O_APPEND,0644);


    // run selected update function
    if(option == ACTION){
        update_philosopher(fd,fd2,id,name,action);
    } else {
        update_philosopher(fd,fd2,id,name,NULL);
    }

    // reset
    remove(filename);
    rename(TMP_FOLDER,filename);
    lseek(fd ,0,SEEK_SET);
    release_lock(fd ,SIZE_FILE);
    close(fd);
    close(fd2);
}


/**
 * Concatenate a philosopher line in the buffer
 * Thread safe
 * @param action
 * @param philosopher_id
 */
void buffercat(char * action, int philosopher_id ){
    char s[64];
    char * p = philosopher(philosopher_id);
    sprintf(s,"%d%s%s%s%s\n",philosopher_id,SPACE, p, SPACE, action);
//    printf("SIZE LINE %d\n", strlen(s));
    strcat(buffer, s);
}


/**
 * Check the current state of a philosopher
 * To be eating, a philosopher must be hungry and sits between philosophers whom are not eating
 * Thread safe
 * @param id
 */
void check_state(int philosopher_id){
    if (state[philosopher_id] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
        state[philosopher_id] = EATING;
        buffercat(EAT,philosopher_id);
        // wake up hungry philosophers during return_fork
        pthread_cond_signal(&Signal[philosopher_id]);
    }
}


/**
 * Pickup forks when a philospher is hungry
 * Thread safe
 * @param philosopher_id
 */
void pickup_forks(int philosopher_id){
    pthread_mutex_lock(&mutex);

    state[philosopher_id] = HUNGRY;

    // eat if neighbours are not eating
    check_state(philosopher_id);

    pthread_cond_signal(&Signal[philosopher_id]);
    pthread_mutex_unlock(&mutex);

}

/**
 * Put down forks after a philosopher has eaten
 * Thread safe
 * @param philosopher_id
 */
void return_forks(int philosopher_id){
    pthread_mutex_lock(&mutex);
    while (state[philosopher_id] == HUNGRY)
        pthread_cond_wait(&Signal[philosopher_id], &mutex);

    state[philosopher_id] = THINKING;
    buffercat(THINK,philosopher_id);

    // state of philosophers next to current one can change
    check_state(LEFT);
    check_state(RIGHT);

    pthread_mutex_unlock(&mutex);
}


/**
 * Each philosophers alternate from eating to thinking 5 times
 * @param param
 * @return
 */
void * cogitate(void * param){
    int * id = param;
    for (int i = 0; i<5;++i){
        pickup_forks(*id);
        sleep(SLEEP_TIME);
        return_forks(*id);
        sleep(SLEEP_TIME);
    }
    pthread_exit(0);
}

/**
 * Populate the database
 * Each philosopher thinks five times
 * Each philosopher eats five times
 * Total of 50 lines + HEADER = (42 * 51 = 2142 bytes)
 */
void create_db(){
    pthread_t thread_id[N];

    // put the header in the buffer
    strcat(buffer,HEADER);

    // Create one thread for each 5 philosophers
    for (int i = 0; i < MAX_THREADS; i++)
        if (pthread_create(&thread_id[i], NULL, &cogitate, &phil[i]))
            handle_error("pthread create");

    for (int i = 0; i < N; i++)
        if(pthread_join(thread_id[i], NULL))
            handle_error("pthread join");


    //write the buffer on file
    buffer[SIZE_FILE] = '\0';
    write_file(filename,buffer);
}

/**
 * Navigate the user through 5 possible actions
 * 1)Select * From Philosopher
 * 2)Delete From Philosopher Where Id
 * 3)Update Philosopher Set Name
 * 4)Update Philosopher Set Name,Action
 * 5)Quit
 */
void menu(){
    long o;
    scan_option(&o);
    switch(o){
        case SELECT:
            printf("%s",read_file(filename));
            menu();
            break;
        case DELETE:
            delete();
            menu();
            break;
        case UPDATE_NAME:
            update(NAME);
            menu();
            break;
        case UPDATE_ACTION:
            update(ACTION);
            menu();
            break;
        case QUIT:
            print(buffer);
            exit(0);
        case TEST:
            test();
            menu();
        default:
            printf("%s\n", MAN);
            menu();
    }
}

int main(int argc, char *argv[]) {
    //set the output filename
    char cwd[MAX_SIZE];
    getcwd(cwd, sizeof(cwd));
    check_args(argc, argv, cwd);
    filename = strcat(cwd, OUT_FOLDER);

    pthread_mutex_init (&mutex, NULL);
    create_db();
    menu();
    return 0;
}

