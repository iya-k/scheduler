#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/uio.h>

#include <unistd.h>
#include <pthread.h>

#include "../include/pile.h"
#include "../include/sched.h"

struct scheduler{
  int nthreads;
  Pile *p;
  pthread_mutex_t mutex;
  pthread_t *th;
  pthread_cond_t cond;
};

int freeSched(struct scheduler *s){
  pthread_mutex_destroy(&(s->mutex));
  pthread_cond_destroy(&(s->cond));
  freePile( s-> p);
  free(s-> th);
  free(s);
	
  return 1;
}

int init(int nthreads, int qlen, struct scheduler *s){
  int error = 0;
  ERROR_INT(qlen, "qlen doit être positif");
  
  s->nthreads = (nthreads == 0)? sched_default_threads():nthreads;
  s->p =  malloc(sizeof(Pile));
  s->p->capa = 0;
  s->p->qlen = qlen;
  s->th =  (pthread_t*) malloc(sizeof(pthread_t) *nthreads);

  /* pthread_mutexattr_t mattr;
  pthread_mutexattr_init(&mattr);
  pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&(s->mutex), &mattr);
  //*/s->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
  /*
  pthread_condattr_t cattr;
  pthread_condattr_init(&cattr);
  pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
  pthread_cond_init(&(s->cond),&cattr);
  */
  printf("INIT **Taille %d :\n ", (s->p ->capa));
  
  return 0;
}

static void * lancer_threads(void *ptr){
  int err = 0;
  ERROR_STRUCT(ptr, "pileSched.c: lancer_thread : ptr");
  struct scheduler *s = (struct scheduler*)ptr;
  
  do{
    puts("boucle dans lancer");
    err = pthread_mutex_lock(&(s->mutex));
    Tache* t = depiler(s-> p);
    err = pthread_mutex_unlock(&(s->mutex));
    
    printf("apres depiler \n"); 
    if(t != NULL){
      ((taskfunc)t->f)(t->closure, s);
      free(t);
    }
    printf("fin creation tache \n");
    
  }while(!pileVide(s-> p));
  printf("fin lancer\n");
  
  pthread_exit(NULL);
}

int createThreads(struct scheduler *s){
   int err = 0;
 
   for(int i = 0; i < (s->nthreads); i++){
     err = pthread_create(&(s -> th[i]), NULL, lancer_threads, &s);
     ERROR_THREAD(err," lifo: createThreads: pthread_create");
     
     printf("%d: thread [%ld]\n",i, pthread_self() );
  }

   for(int i = 0; i < (s -> nthreads); i++){
    err = pthread_join((s -> th[i]), NULL);
    printf("join thread [%ld]\n", pthread_self() );
    ERROR_THREAD(err," lifo: createThreads: pthread_join");
    }

  return 0;
}

int sched_init(int nthreads, int qlen, taskfunc f, void *closure){
  
  struct scheduler *s = malloc(sizeof(struct scheduler));
  ERROR_STRUCT(s," lifo: sched_init: s nulle");
  
  init(nthreads, qlen, s);
  empiler((s-> p), f, closure);
  createThreads(s);
  freeSched(s);
  
  exit(EXIT_SUCCESS);
}

int sched_spawn(taskfunc f, void *closure, struct scheduler *s){
  puts("Dans spawn");
  pthread_mutex_lock(&(s->mutex));
  empiler((s->p), f, closure);
  pthread_mutex_unlock(&(s->mutex));

  exit(EXIT_SUCCESS);
}
