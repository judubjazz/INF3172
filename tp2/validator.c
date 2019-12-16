//
// Created by ju on 12/3/19.
//

#include <pthread.h>
#include <semaphore.h>
#include "utils.c"
#include "test.c"

#define P0 "Socrates  "
#define P1 "Epicurus  "
#define P2 "Pythagoras"
#define P3 "Plato     "
#define P4 "Aristotle "
#define SPACE "          "
#define HEADER "Code          Nom          Action   \n"
#define MENU "\n\n1)Consulter résultat\n2)Modifier le nom d'un philosphe\n3)Supprimer le nom d'un philosophe\n4)Modifier l'action et le nom d'un philosophe\n5)Quitter\n"
#define MAN "\nChoisissez un numéro entre 1 et 5\n"
#define SELECT        1
#define UPDATE_NAME   2
#define DELETE        3
#define UPDATE_ACTION 4
#define QUIT          5
#define NAME          0
#define ACTION        1
#define EAT           "mange"
#define THINK         "pense"
#define TMP_FOLDER    "tmp.txt"
#define MAX_THREADS   5
#define SLEEPTIME     0
#define SIZE_LINE     37
#define SIZE_FILE     1888
#define SIZE_NAME     12
#define TRY_AGAIN     "Veuillez réessayer plus tard\n"
#define STDIN_PHILOSOPHER_ID "Entrer le id du philosophe à modifier (0 à 4)\n"
#define STDIN_PHILOSOPHER_NAME "Entrer le nouveau nom\n"
#define STDIN_ACTION "Entrer la nouvelle action du philosophe (mange ou pense)\n"
#define ERR_SIZE_NAME "Le nom doit être de maximum 10 caractères\n"
#define ERR_ACTION "L'action doit être mange ou pense\n"
#define ERR_PHILOSOPHER_ID "le id doit être entre 0 et 4\n"

#define N 5
#define THINKING 2
#define HUNGRY 1
#define EATING 0
#define LEFT \
    (id + 4) % N
#define RIGHT \
    (id + 1) % N
int state[N];
int phil[N] = { 0, 1, 2, 3, 4 };


//implicit declaration
void *cogitate(void * param);
void pickup_forks(int phnum);
void return_forks(int id);
void menu();

pthread_mutex_t mutex;
pthread_cond_t S[N];
char const* filename;
char buffer[MAX_SIZE*5];


char * philosopher(int id){
    switch (id){
        case 0:return P0;
        case 1:return P1;
        case 2:return P2;
        case 3:return P3;
        case 4:return P4;
        default:handle_error("philosopher\n");
    }
}

char * get_action(char c){
    return c == 'm' ? EAT : THINK;
}

void delete_philosopher(int fd, int philosopher_id){
    char buf[SIZE_FILE];
    buf[SIZE_FILE] = '\0';
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
    int id;

    if(!activate_lock(fd,SIZE_FILE)){
        fprintf(stderr, TRY_AGAIN);
        return;
    }

    printf(STDIN_PHILOSOPHER_ID);
    scanf("%d",&id);

    if(id < 0 || id > 4) {
        fprintf(stderr, ERR_PHILOSOPHER_ID);
        delete();
    }
    delete_philosopher(fd,id);
    release_lock(fd, SIZE_FILE);
}

void update_philosopher(int fd, int fd2, int philosopher_id, char *name, char *action){
    char * line;
    char s[SIZE_LINE];
    char buf[SIZE_FILE];
    buf[SIZE_FILE] = '\0';

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
    char padding[15];
    char action [8];
    int id,fd,i;
    memset(padding,' ',15);
    FILE * fileptr1;
    int philosophes_lines[10];
    char line[MAX_SIZE];

    printf(STDIN_PHILOSOPHER_ID);
    scanf("%d",&id);
    if(id < 0 || id > 4) {
        fprintf(stderr, ERR_PHILOSOPHER_ID);
        update(option);
    }

    // seek philosophers lines
    i = 0;
    fileptr1 = fopen(filename, "r");
    for(int no_line=1;fgets(line, sizeof(line),fileptr1) != NULL ;++no_line){
        if(line[0]==(id + '0')){
            philosophes_lines[i]= no_line;
            ++i;
        }
    }
    fclose(fileptr1);

    fd = open(filename, O_RDWR| O_APPEND | O_CREAT, 0644);
    if(!activate_lock(fd, SIZE_FILE)){
        fprintf(stderr, TRY_AGAIN);
        update(option);
    }

    printf(STDIN_PHILOSOPHER_NAME);
    scanf("%s",name);
    if(strlen(name) > 10){
        fprintf(stderr, ERR_SIZE_NAME);
        update(option);
    }

    if(option == ACTION){
        printf(STDIN_ACTION);
        scanf("%s",action);
        if (strcmp(action, THINK) && strcmp(action, EAT)){
            fprintf(stderr, ERR_ACTION);
            update(option);
        }
        action[5] = '\0';
    }

    // resize name to fit columns in the file
    strcat(name,padding);
    name[10] = '\0';

    release_lock(fd, SIZE_FILE);
    // activate write locks on fields
    for(i = 0; i < 10; ++i){
        int philosophe_line = philosophes_lines[i];
        int start = philosophe_line * SIZE_LINE;
        set_position(fd,start);
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
//        if(!activate_lock(fd,SIZE_LINE)){
//            fprintf(stderr,TRY_AGAIN);
//            return;
//        }
    }

    set_position(fd,0);
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

void menu(){
    printf("%s", MENU);
    char option[64];
    scanf("%s",option);
    int o = atoi(option);
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


void buffercat(char * action, int philosopher_id ){
    char s[64];
    char * p = philosopher(philosopher_id);
    sprintf(s,"%d%s%s%s%s\n",philosopher_id,SPACE, p, SPACE, action);
//    printf("SIZE LINE %d\n", strlen(s));
    strcat(buffer, s);
}



void check_state(int id){
    if (state[id] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
        state[id] = EATING;
        buffercat(EAT,id);
        // wake up hungry philosophers during return_fork
        pthread_cond_signal(&S[id]);
    }
}


// take up chopsticks
void pickup_forks(int id){
    pthread_mutex_lock(&mutex);

    state[id] = HUNGRY;

    // eat if neighbours are not eating
    check_state(id);

    pthread_cond_signal(&S[id]);
    pthread_mutex_unlock(&mutex);

}

// put down chopsticks
void return_forks(int id){

    pthread_mutex_lock(&mutex);
    while (state[id] == HUNGRY)
        pthread_cond_wait(&S[id], &mutex);

    state[id] = THINKING;
    buffercat(THINK,id);

    check_state(LEFT);
    check_state(RIGHT);

    pthread_mutex_unlock(&mutex);
}


void * cogitate(void * param){
    int * id = param;
    for (int i = 0; i<5;++i){
        pickup_forks(*id);
//        sleep(1);
        return_forks(*id);
//        sleep(1);
    }
//    printf("END FOR PHIL %d\n", *id);
    pthread_exit(0);
}

void create_db(){
    pthread_t thread_id[N];

    strcat(buffer,HEADER);
    // Create one thread for each 5 philosophers
    for (int i = 0; i < MAX_THREADS; i++)
        if (pthread_create(&thread_id[i], NULL, &cogitate, &phil[i]))
            handle_error("pthread create");

    for (int i = 0; i < N; i++)
        if(pthread_join(thread_id[i], NULL))
            handle_error("pthread join");

    buffer[SIZE_FILE] = '\0';
    write_file(filename,buffer);
}





int main(int argc, char *argv[]) {
    char cwd[MAX_SIZE];
    getcwd(cwd, sizeof(cwd));
    check_args(argc, argv, cwd);
    filename = strcat(cwd, "/resultat.txt");
    pthread_mutex_init (&mutex, NULL);
    create_db();
    menu();

//    test_delete(filename, 0, delete_line2);
//    test_update(filename, 1,"roger", "rote", update_name);

    return 0;
}

