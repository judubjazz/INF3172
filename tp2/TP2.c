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
#define EAT          0
#define THINK        1
#define MAX_THREADS  5
#define SLEEPTIME    0
#define SPACE "          "
#define HEADER "Code          Nom          Action\n"
#define MENU "\n\n1)Consulter résultat\n2)Modifier le nom d'un philosphe\n3)Supprimer le nom d'un philosophe\n4)Modifier l'action et le nom d'un philosophe\n5)Quitter\n"
#define MAN "\nChoisissez un numéro entre 1 et 5\n"
#define SELECT       1
#define UPDATE_NAME   2
#define DELETE       3
#define UPDATE_ACTION 4
#define QUIT         5
#define NAME         0
#define ACTION       1


//implicit declaration
void *pickup_forks(void * param);
void *return_forks(void * param);
void *delete_philosopher(void * param);
void menu();

pthread_mutex_t mutex;
char const* filename;
char buffer[MAX_SIZE*5];
//pthread_mutex_init (&mutex, NULL);
//pthread_cond_init (&cond_var, NULL);

struct thread {
    pthread_t      thread_id;        /* ID returned by pthread_create() */
    pthread_cond_t notbusy;
    int            philosopher_id;       /* Application-defined thread # */
    char *         philosopher_name;
    int            action;

};


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

int philosopher_id(char * name){
    //TODO refactor this
    char const* p0 = "Socrates";
    char const* p1 = "Epicurus";
    char const* p2 = "Pythagoras";
    char const* p3 = "Plato";
    char const* p4 = "Aristotle";


    if      (!strcmp(name,p0)) return 0;
    else if (!strcmp(name,p1)) return 1;
    else if (!strcmp(name,p2)) return 2;
    else if (!strcmp(name,p3)) return 3;
    else if (!strcmp(name,p4)) return 4;
    fprintf(stderr, "Le philosophe %s n'existe pas\n", name);
    menu();
}

void delete_line2(FILE * fileptr1, FILE * fileptr2, int philosopher_id){
    char c;
    char s[MAX_SIZE];
    c = getc(fileptr1);
    while (c != EOF) c = getc(fileptr1);

    rewind(fileptr1);

    char s2[64];
    char id[2];

    while (fscanf(fileptr1, "%s", s) == 1) {
        if(s[0]==(philosopher_id+'0')){
            // match the id of the philospher
            // scan the name and the action
            fscanf(fileptr1,"%s",s);
            fscanf(fileptr1,"%s",s);
        } else if(s[0]=='C'){
            // skip header
            fputs(HEADER,fileptr2);
            fscanf(fileptr1,"%s",s);
            fscanf(fileptr1,"%s",s);
        } else {
            char * p = philosopher(philosopher_id -'0');
            sprintf(id,"%c",s[0]);
            // skip the philophser name
            fscanf(fileptr1,"%s",s);
            // scan the action
            fscanf(fileptr1,"%s",s);
            sprintf(s2,"%s%s%s%s%s\n",id,SPACE,p,SPACE,s);
            // put the new line on file2
            fputs(s2,fileptr2);
        }
    }
}



void *delete_philosopher(void *param){
    struct thread *t = param;
    pthread_mutex_lock(&mutex);
    int fd = open(filename,O_RDWR);
    int status = lockf(fd,F_TEST,0);
    fprintf(stderr,"file descriptor: %d\n",fd);
    fprintf(stderr,"\nstatus : %d\n", status);
//    delete_line(filename, t->philosopher_id);
    pthread_mutex_unlock (&mutex);
    pthread_exit(0);

}

void delete(){
    printf("Entrer le id du philosophe (1 à 5)\n");
    int id;
    scanf("%d",&id);
    if(id < 1 || id > 5) {
        fprintf(stderr, "le id doit être entre 1 et 5\n");
        delete();
    }
    lock_to_delete(filename, id, delete_line2);
}

void update_name(FILE * fileptr1, FILE * fileptr2, int philosopher_id, char* name, char * action){
    char line[MAX_SIZE];
    char * pid;
    char line2[MAX_SIZE];

    while (fgets(line, sizeof(line),fileptr1) != NULL ) {
        if(line[0]== (philosopher_id + '0')){
            pid = strtok(line," ");
            //skip name
            strtok(NULL," ");

            if(action == NULL){
                // need to scan the action
                action = strtok(NULL," ");
                sprintf(line2,"%s%s%s%s%s",pid,SPACE,name,SPACE,action);
                action = NULL;
            } else {
                // update the action
                strtok(NULL," ");
                sprintf(line2,"%s%s%s%s%s",pid,SPACE,name,SPACE,action);
            }
            fputs(line2,fileptr2);
        } else {
            fputs(line,fileptr2);
        }
    }
}


void update(int option){
    char name[64];
    char empty[15];
    char action [8];
    int id;
    memset(empty,' ',15);

    printf("Entrer le id du philosophe à modifier (0 à 4)\n");
    scanf("%d",&id);
    if(id < 0 || id > 4) {
        fprintf(stderr, "le id doit être entre 0 et 4\n");
        update(option);
    }

    printf("Entrer le nouveau nom\n");
    scanf("%s",name);
    if(strlen(name) > 10){
        fprintf(stderr, "le nom doit être de maximum 10 caractères\n");
        update(option);
    }

    if(option == ACTION){
        const char * pense = "pense";
        const char * mange = "mange";
        printf("Entrer la nouvelle action du philosophe (mange ou pense)\n");
        scanf("%s",action);
        if (strcmp(action, pense) && strcmp(action, mange)){
            fprintf(stderr, "L'action doit être mange ou pense\n");
            update(option);
        }
        action[5] = '\n';
    }

    // resize name to fit columns in the file
    strcat(name,empty);
    name[10] = '\0';


    if(option == ACTION){
        lock_to_update(filename, id, name, action, update_name);
    } else {
        lock_to_update(filename, id, name, NULL, update_name);
    }

}

void menu(){
    printf("%s", MENU);
    char s[64];
    scanf("%s",s);
    int n = atoi(s);
    switch(n){
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

char * action (int a){
    return a == EAT ? "mange" : "pense";
}

void buffercat(struct thread *t){
    char s[64];
    char * a = action(t->action);
    char * p = philosopher(t->philosopher_id);
    sprintf(s,"%d%s%s%s%s\n",t->philosopher_id,SPACE, p, SPACE, a);
    strcat(buffer, s);
}

void *return_forks(void* param){
    struct thread *t = param;
    pthread_mutex_lock(&mutex);
    while (t->action ==EAT)
        pthread_cond_wait(&t->notbusy, &mutex);
    buffercat(t);
    t->action = EAT;
    pthread_mutex_unlock (&mutex);
}

void *pickup_forks(void * param){
    struct thread *t = param;
    pthread_mutex_lock(&mutex);
    buffercat(t);
    t->action = THINK;
    pthread_mutex_unlock (&mutex);
    pthread_cond_signal(&t->notbusy);
}

void * cogitate(void * param){
    struct thread *t = param;
    for (int i = 0; i<5;++i){
        pickup_forks(param);
//        sleep(1);
        return_forks(param);
//        sleep(1);
    }
    pthread_exit(0);
}

void create_db(){
    struct thread * t;
    t = calloc(MAX_THREADS, sizeof(struct thread));
    strcat(buffer, HEADER);

    // Create one thread for each 5 philosophers
    for (int tnum = 0; tnum < MAX_THREADS; tnum++) {
        t[tnum].thread_id = tnum;
        t[tnum].philosopher_id = tnum;
        t[tnum].action = EAT;

        if (pthread_create(&t[0].thread_id, NULL, &cogitate, &t[tnum]))
            handle_error("pthread create");
    }

    //wait the threads response
    if(pthread_join(t[0].thread_id, NULL))
        handle_error("pthread join");

    write_file(filename,buffer);
    free(t);
}

int main(int argc, char *argv[]) {
    char cwd[MAX_SIZE];
    getcwd(cwd, sizeof(cwd));
    check_args(argc, argv, cwd);
    filename = strcat(cwd, "/resultat.txt");


    create_db();
//    if(!delete_line(filename,0, delete_line2)){
//        perror("Impossible de supprimer, Réessayer plus tard");
//    }
    menu();
    //TODO check for new name to be valid size
//    update(ACTION);
//    update(ACTION);

//    if(!update_line(filename, 0, "roger", NULL, update_name)){
//        printf("Veuillez essayer plus tard");
//    }
//    test_delete(filename, 0, delete_line2);
//    test_update(filename, 1,"roger", "rote", update_name);

    return 0;
}

