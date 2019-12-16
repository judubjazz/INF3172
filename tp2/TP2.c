#include <pthread.h>
#include <semaphore.h>
#include "utils.c"

#define P0                     "Socrates  "
#define P1                     "Epicurus  "
#define P2                     "Pythagoras"
#define P3                     "Plato     "
#define P4                     "Aristotle "
#define SPACE                  "          "
#define HEADER                 "Code          Nom          Action   \n"
#define MENU                   "\n\n1)Consulter résultat\n2)Modifier le nom d'un philosphe\n3)Supprimer le nom d'un philosophe\n4)Modifier l'action et le nom d'un philosophe\n5)Quitter\n"
#define MAN                    "\nChoisissez un numéro entre 1 et 5\n"
#define EAT                    "mange"
#define THINK                  "pense"
#define TMP_FOLDER             "tmp.txt"
#define OUT_FOLDER             "/resultat.txt"
#define TRY_AGAIN              "Veuillez réessayer plus tard\n"
#define STDIN_PHILOSOPHER_ID   "Entrer le id du philosophe à modifier (0 à 4)\n"
#define STDIN_PHILOSOPHER_NAME "Entrer le nouveau nom\n"
#define STDIN_ACTION           "Entrer la nouvelle action du philosophe (mange ou pense)\n"
#define ERR_SIZE_NAME          "Le nom doit être de maximum 10 caractères\n"
#define ERR_ACTION             "L'action doit être mange ou pense\n"
#define ERR_PHILOSOPHER_ID     "le id doit être entre 0 et 4\n"
#define SELECT        1
#define UPDATE_NAME   2
#define DELETE        3
#define UPDATE_ACTION 4
#define QUIT          5
#define NAME          0
#define ACTION        1
#define MAX_THREADS   5
#define SLEEPTIME     0
#define SIZE_LINE     37
#define SIZE_FILE     1888
#define SIZE_NAME     12
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
pthread_cond_t S[N];
char const* filename;
char buffer[MAX_SIZE*5];
int state[N];
int phil[N] = { 0, 1, 2, 3, 4 };



char * philosopher(int id){
    switch (id){
        case 0:return P0;
        case 1:return P1;
        case 2:return P2;
        case 3:return P3;
        case 4:return P4;
        default:
            handle_error("philosopher\n")
            return NULL;
    }
}

char *get_action(char c) {
    return c == 'm' ? EAT : THINK;
}

void setcwd(int argc, char * argv[]){
    //get the current working directory
    char cwd[MAX_SIZE];
    getcwd(cwd, sizeof(cwd));
    check_args(argc, argv, cwd);
    filename = strcat(cwd, OUT_FOLDER);
}

void scan_option(long *o){
    printf("%s", MENU);
    char option[64];
    fgets(option, 64, stdin);
    *o = strtol(option, NULL, 10);
}

bool scan_name(char *name){
    printf(STDIN_PHILOSOPHER_NAME);
    char padding[15];
    memset(padding,' ',15);
    scanf("%s",name);
    if(strlen(name) > 10){
        return false;
    }
    // resize name to fit columns in the file
    strcat(name,padding);
    name[10] = '\0';
    return true;
}

bool scan_action(char * action){
    printf(STDIN_ACTION);
    scanf("%s",action);
    if (strcmp(action, THINK) !=0 && strcmp(action, EAT) !=0){
        return false;
    }
    action[5] = '\0';
    return true;
}

bool scan_id(long * id){
    printf(STDIN_PHILOSOPHER_ID);
    char option[64];
    fgets(option, 64, stdin);
    *id = strtol(option, NULL, 10);
    if(*id < 0 || *id > 4) {
        return false;
    }
    return true;
}

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

void delete_philosopher(int fd, int philosopher_id){
    char buf[SIZE_FILE];
    buf[SIZE_FILE-1] = '\0';
    char s[64];
    char * line;

    read(fd,buf,SIZE_FILE);

    int fd2 = open(TMP_FOLDER, O_RDWR | O_APPEND | O_CREAT, 0644);
    line = strtok(buf,"\n");
    do{
        if(line[0]=='C'){
            // skip header
            write(fd2,HEADER,SIZE_LINE);
        } else if (line[0] != philosopher_id+'0') {
            sprintf(s,"%s\n",line);
            write(fd2,s,SIZE_LINE);
        }
        line = strtok(NULL,"\n");
    }while(line != NULL);

    remove(filename);
    rename(TMP_FOLDER, filename);
    close(fd2);
    close(fd);
}


void delete(){
    int fd = open(filename, O_APPEND | O_CREAT | O_RDWR, 0644);
    long id;

    if(!activate_lock(fd,SIZE_FILE)){
        fprintf(stderr, TRY_AGAIN);
        return;
    }

    if(!scan_id(&id)){
        handle_error(ERR_PHILOSOPHER_ID)
        delete();
    }

    delete_philosopher(fd,id);
    release_lock(fd, SIZE_FILE);
}

void update_philosopher(int fd, int fd2, int philosopher_id, char *name, char *action){
    char * line;
    char s[SIZE_LINE];
    char buf[SIZE_FILE];
    buf[SIZE_FILE-1] = '\0';

    // rewind to read all file
    set_position(fd,0);
    read(fd,buf,SIZE_FILE);

    line = strtok(buf,"\n");
    while(line != NULL){
        if(line[0]=='C'){
            write(fd2,HEADER,SIZE_LINE);
        } else if (line[0] != philosopher_id+'0') {
            sprintf(s,"%s\n",line);
            write(fd2,s,SIZE_LINE);
        } else {
            if(action == NULL) {
                // need to scan the action
                action = get_action(line[31]);
                sprintf(s, "%c%s%s%s%s\n", line[0], SPACE, name, SPACE, action);
                action = NULL;
            } else {
                sprintf(s, "%c%s%s%s%s\n", line[0], SPACE, name, SPACE, action);
            }
            write(fd2,s,SIZE_LINE);
        }
        line = strtok(NULL,"\n");
    }
}


void update(int option){
    char name[64];
    char action [8];
    long id;
    int fd;
    int philosophers_lines[10];

    if(!scan_id(&id)){
        handle_error(ERR_PHILOSOPHER_ID)
        update(option);
    }

    // lock all file to prevent other thread writing in the philospher's line
    fd = open(filename, O_RDWR| O_APPEND | O_CREAT, 0644);
    if(!activate_lock(fd, SIZE_FILE)){
        handle_error(TRY_AGAIN)
        update(option);
    }

    get_philosophers_lines(id, philosophers_lines);

    if(!scan_name(name)){
        handle_error(ERR_SIZE_NAME)
        update(option);
    }

    if(option == ACTION){
        if(!scan_action(action)){
            handle_error(ERR_ACTION)
            update(option);
        }
    }

    // global lock is not needed anymore
    release_lock(fd, SIZE_FILE);

    // activate write locks only on desire fields
    for(int i = 0; i < 10; ++i){
        int philosopher_line = philosophers_lines[i];
        int offset = philosopher_line * SIZE_LINE;
        set_position(fd,offset);
        switch(option){
            case ACTION:
                if(!activate_lock(fd,SIZE_LINE)){
                    fprintf(stderr,TRY_AGAIN);
                    return;
                }
                break;
            case NAME:
                if(!activate_lock(fd,SIZE_NAME)){
                    fprintf(stderr,TRY_AGAIN);
                    return;
                }
                break;
            default:
                fprintf(stderr,TRY_AGAIN);
                return;
        }
    }

    // create a temporary folder to paste in
    int fd2 = open(TMP_FOLDER, O_RDWR| O_APPEND | O_CREAT, 0644);

    if(option == ACTION){
        update_philosopher(fd,fd2,id,name,action);
    } else {
        update_philosopher(fd,fd2,id,name,NULL);
    }

    remove(filename);
    rename(TMP_FOLDER,filename);
    set_position(fd,0);
    release_lock(fd,SIZE_FILE);
    close(fd);
    close(fd2);

}


/**
 * concatenate a philosopher line in the buffer
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
 * check the current state of a philosopher
 * to be eating, a philospher must be hungry and sits between philosphers whom are not eating
 * @param id
 */
void check_state(int philosopher_id){
    if (state[philosopher_id] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
        state[philosopher_id] = EATING;
        buffercat(EAT,philosopher_id);
        // wake up hungry philosophers during return_fork
        pthread_cond_signal(&S[philosopher_id]);
    }
}


/**
 * pickup forks when a philospher is hungry
 * @param philosopher_id
 */
void pickup_forks(int philosopher_id){
    pthread_mutex_lock(&mutex);

    state[philosopher_id] = HUNGRY;

    // eat if neighbours are not eating
    check_state(philosopher_id);

    pthread_cond_signal(&S[philosopher_id]);
    pthread_mutex_unlock(&mutex);

}

/**
 * put down sticks after a philospher has eaten
 * @param philosopher_id
 */
void return_forks(int philosopher_id){
    pthread_mutex_lock(&mutex);
    while (state[philosopher_id] == HUNGRY)
        pthread_cond_wait(&S[philosopher_id], &mutex);

    state[philosopher_id] = THINKING;
    buffercat(THINK,philosopher_id);

    check_state(LEFT);
    check_state(RIGHT);

    pthread_mutex_unlock(&mutex);
}


/**
 * each philosopher alternates thinking and eating 5 times
 * @param param
 * @return
 */
void * cogitate(void * param){
    int * id = param;
    printf("filename %s\n", filename);
    for (int i = 0; i<5;++i){
        pickup_forks(*id);
        sleep(SLEEPTIME);
        return_forks(*id);
        sleep(SLEEPTIME);
    }
    printf("filename %s\n", filename);
    pthread_exit(0);
}

void create_db(){
    pthread_t thread_id[N];

    // put the header in the buffer
    strcat(buffer,HEADER);

    // Create one thread for each 5 philosophers
    for (int i = 0; i < MAX_THREADS; i++)
        if (pthread_create(&thread_id[i], NULL, &cogitate, &phil[i]))
            handle_error("pthread create")

    for (int i = 0; i < N; i++)
        if(pthread_join(thread_id[i], NULL))
            handle_error("pthread join")


    //write the buffer on file
    buffer[SIZE_FILE] = '\0';
    write_file(filename,buffer);
}


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
            printf("%s",read_file(filename));
            exit(0);
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

