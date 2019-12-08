#include <assert.h>
#include<sys/wait.h>

void test(const char * filename){
    int fd;
    struct flock lock;
    printf ("opening %s\n", filename);
    /* Open a file descriptor to the file. */
    fd = open (filename, O_WRONLY);
    printf ("locking\n");
    /* Initialize the flock structure. */
    memset (&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;
    /* Place a write activate_lock on the file. */
    fcntl (fd, F_SETLKW, &lock);

    printf ("locked; hit Enter to unlock... ");
    /* Wait for the user to hit Enter. */
    getchar ();

    printf ("unlocking\n");
    /* Release the activate_lock. */
    lock.l_type = F_UNLCK;
    fcntl (fd, F_SETLKW, &lock);

    close (fd);
}

void test_delete(const char *filename, int philosophe_id, void (*delete_line2)(FILE *, FILE *, int)){
    int pid = fork();
    int child= 1, parent=1;

    if(pid>0){
        parent = lock_to_delete(filename, philosophe_id, delete_line2);
        wait(NULL);
        assert(parent ==1);
        printf("parent %d\n", parent);
        printf("child in parent %d\n", child);
    }else {
        child = lock_to_delete(filename, philosophe_id, delete_line2);
        printf("child %d\n", child);
        assert(child == 0);
    }
}


void test_update(const char *filename, int philosophe_id, char * name, char * action, void (*update)(FILE *, FILE *, int, char *, char *)){
    int pid = fork();
    int updated;
    if(pid>0){
        updated = lock_to_update(filename, philosophe_id, name, action, update);
        wait(NULL);
        assert(updated ==1);
    }else {
        updated = lock_to_update(filename, philosophe_id, name, action, update);
        assert(updated == 0);
    }
}

void test_update2(const char *filename, int philosophe_id, char * name, char * action, void (*update)(FILE *, FILE *, int, char *, char *)){
    int pid = fork();
    int updated;
    if(pid>0){
        updated = lock_to_update(filename, 0, name, action, update);
        wait(NULL);
        assert(updated ==1);
    }else {
        updated = lock_to_update(filename, 1, name, action, update);
        assert(updated == 0);
    }
}
