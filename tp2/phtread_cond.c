struct list {
    pthread_mutex_t lm;
    ...
}

struct elt {
    key k;
    int busy;
    pthread_cond_t notbusy;
    ...
}

/* Find a list element and reserve it. */
struct elt *
list_find(struct list *lp, key k)
{
    struct elt *ep;

    pthread_mutex_lock(&lp->lm);
    while ((ep = find_elt(l, k) != NULL) && ep->busy)
        pthread_cond_wait(&ep->notbusy, &lp->lm);
    if (ep != NULL)
        ep->busy = 1;
    pthread_mutex_unlock(&lp->lm);
    return(ep);
}

delete_elt(struct list *lp, struct elt *ep)
{
    pthread_mutex_lock(&lp->lm);
    assert(ep->busy);
    ... remove ep from list ...
    ep->busy = 0;  /* Paranoid. */
    (A) pthread_cond_broadcast(&ep->notbusy);
    pthread_mutex_unlock(&lp->lm);
    (B) pthread_cond_destroy(&rp->notbusy);
    free(ep);
}

void exemple() {
    pthread_mutex_t mutex;
    pthread_cond_t condition;

// thread 1 :
    pthread_mutex_lock(&mutex); //mutex lock
    while (!condition) {
        pthread_cond_wait(&condition, &mutex); //wait for the condition
    }
    /* do what you want */
    pthread_mutex_unlock(&mutex);

// thread 2:
    pthread_mutex_lock(&mutex);
    /* do something that may fulfill the condition */
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&condition); //wake up thread 1
}