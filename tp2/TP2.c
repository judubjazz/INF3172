//
// Created by ju on 12/3/19.
//

#include <pthread.h>
#include <semaphore.h>
#include "utils.c"

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
#define UPDATENAME   2
#define DELETE       3
#define UPDATEACTION 4
#define QUIT         5


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
    fprintf(stderr, "Le philospose %s n'existe pas\n", name);
    menu();
}

void *delete_philosopher(void *param){
    struct thread *t = param;
    pthread_mutex_lock(&mutex);
    int fd = open(filename,O_RDWR);
    int status = lockf(fd,F_TEST,0);
    fprintf(stderr,"file descriptor: %d\n",fd);
    fprintf(stderr,"\nstatus : %d\n", status);
    delete_line(filename, t->philosopher_id);
    pthread_mutex_unlock (&mutex);
    pthread_exit(0);

}

void delete(){
    printf("Entrer le nom du philosophe à supprimer\n");
    char name[32];
    scanf("%s",name);

    struct thread *t = calloc(1, sizeof(struct thread));
    t->thread_id = DELETE;
    t->philosopher_id = philosopher_id(name);
    delete_line(filename, t->philosopher_id);

//    if(pthread_create(&t->thread_id, NULL, &delete_philosopher, &t[0]))
//        handle_error("phtread delete");
//    if(pthread_join(t->thread_id, NULL))
//        handle_error("pthread_join delete");
    free(t);
}


void update(){

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
        case UPDATENAME:
            update();
            menu();
            break;
        case UPDATEACTION:
            printf("choice 3\n");
            break;
        case QUIT:
            printf("%s",buffer);
            exit(0);
        default:
            printf("%s\n", MAN);
            menu();
    }

}

void test(){
    int fd;
    struct flock lock;
    printf ("opening %s\n", filename);
    /* Open a file descriptor to the file. */
    fd = open (filename, O_WRONLY);
    printf ("locking\n");
    /* Initialize the flock structure. */
    memset (&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;
    /* Place a write lock on the file. */
    fcntl (fd, F_SETLKW, &lock);

    printf ("locked; hit Enter to unlock... ");
    /* Wait for the user to hit Enter. */
    getchar ();

    printf ("unlocking\n");
    /* Release the lock. */
    lock.l_type = F_UNLCK;
    fcntl (fd, F_SETLKW, &lock);

    close (fd);
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

void * return_forks(void* param){
    struct thread *t = param;
    pthread_mutex_lock(&mutex);
    while (t->action ==EAT)
        pthread_cond_wait(&t->notbusy, &mutex);
    buffercat(t);
    t->action = EAT;
    pthread_mutex_unlock (&mutex);
    pthread_exit(0);
}
void * pickup_forks(void * param){
    struct thread *t = param;
    pthread_mutex_lock(&mutex);
    buffercat(t);
    t->action = THINK;
    pthread_mutex_unlock (&mutex);
    pthread_cond_signal(&t->notbusy);
    pthread_exit(0);

}

void * cogitate(void * param){
    for (int i = 0; i<5;++i){
        pickup_forks(param);
        sleep(1);
        return_forks(param);
        sleep(1);
    }
}

void create_db(){
    write_file(filename, HEADER);
    struct thread * t;
    t = calloc(MAX_THREADS, sizeof(struct thread));


    // Create one thread for each 11 sudoku evaluations
    for (int tnum = 0; tnum < MAX_THREADS; tnum++) {
        t[tnum].thread_id = tnum;
        t[tnum].philosopher_id = tnum;
        t[tnum].action = EAT;

        if (pthread_create(&t[0].thread_id, NULL, &cogitate, &t[tnum]))
            handle_error("[htrehad create");
    }
    if(pthread_join(t[0].thread_id, NULL))
        handle_error("pthread join");

    write_file(filename,buffer);
}
int main(int argc, char *argv[]) {
    char cwd[MAX_SIZE];
    getcwd(cwd, sizeof(cwd));
    check_args(argc, argv, cwd);
    filename = strcat(cwd, "/resultat.txt");
//    int fd = open(filename,O_RDWR);
//    test_lock(3, filename);
//    menu();
//    delete_line(filename,0);
    create_db();
    test();

    return 0;
}

