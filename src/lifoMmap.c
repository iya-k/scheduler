#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/uio.h>

#include <unistd.h>
#include <pthread.h>

#include "../include/pile.h"
#include "../include/sched.h"

#define gestion_erreur(msg) \
               do {perror(msg); exit(EXIT_FAILURE); } while (0)

struct scheduler{
  int nthreads;
  int nbr;
  Pile *p;
  pthread_mutex_t mutex;
  pthread_t *th;
  pthread_cond_t cond;
};

static int freeSched(struct scheduler *s){
  pthread_mutex_destroy(&(s->mutex));
  pthread_cond_destroy(&(s->cond));
  freePile( s-> p);
  free(s-> th);
  //free(s);
  munmap((void *) s, sizeof(struct scheduler));
	
  return 1;
}

void init(struct scheduler *s, int nthreads, int qlen){
  int error = 0;
  pthread_mutexattr_t psharedm;
  error = pthread_mutexattr_init(&psharedm);
  if(error != 0) gestion_erreur(" pthread_mutexattr_init");
  error = pthread_mutexattr_setpshared(&psharedm,PTHREAD_PROCESS_SHARED);
  if(error != 0) gestion_erreur(" pthread_mutexattr_setpshared");
  
  ERROR_INT(qlen, "qlen doit être positif");
  
  s->nthreads = (nthreads == 0)? sched_default_threads():nthreads;
  s->p =  malloc(sizeof(Pile));
  s->p->capa = 0;
  s->p->qlen = qlen;
  s->th =  (pthread_t*) malloc(sizeof(pthread_t) *nthreads);
  s->nbr = 0;
  //error =  pthread_mutex_init(&s->mutex, &psharedm);
  //if(error != 0) gestion_erreur(" pthread_mutex_init");
  
  s->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
  /*
  pthread_condattr_t cattr;
  pthread_condattr_init(&cattr);
  pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
  pthread_cond_init(&(s->cond),&cattr);
  
  printf("INIT **Taille %d :\n ", (s->p ->capa));
  */
  //return (s);
}

static void * lancer_threads(void *ptr){
  int err = 0;
  ERROR_STRUCT(ptr, "pileSched.c: lancer_thread : ptr");
  struct scheduler *s = (struct scheduler*)ptr;
  
  do{
    err = pthread_mutex_lock(&(s->mutex));
    if(err == 0){
      Tache* t = depiler(s-> p);
    err = pthread_mutex_unlock(&(s->mutex));
      if(t != NULL){
	(s->nbr)++;
	((taskfunc)t->f)(t->closure, s);
	(s->nbr)--;
	free(t);
      }
    
    }
    else{
      printf("impossible \n");	
      gestion_erreur(" pthread_mutex_lock");
    }
    
    printf("fin creation tache nbr %d \t etat pile %d \n",s->nbr, pileVide(s-> p));
    
  }while(!pileVide(s-> p) || 0 != (s->nbr));
  
  printf("fin lancer\n");
  
  pthread_exit(NULL);
}

static void createThreads(struct scheduler *s){
  int err = 0; int i;
 
   for(i = 0; i < (s->nthreads); i++){
     err = pthread_create(&(s -> th[i]), NULL, lancer_threads, &s);
     ERROR_THREAD(err," lifo: createThreads: pthread_create");
     
     printf("%d: thread [%ld]\n",i, pthread_self() );
   }
   for(i = 0; i < (s -> nthreads); i++){
    err = pthread_join((s -> th[i]), NULL);
    ERROR_THREAD(err," lifo: createThreads: pthread_join");
    printf("join thread [%ld]\n", pthread_self() );
    }

   //return NULL;
}

int sched_init(int nthreads, int qlen, taskfunc f, void *closure){
struct scheduler *s = malloc(sizeof(struct scheduler));
/*(struct scheduler*) mmap(NULL, sizeof(struct scheduler), PROT_READ | PROT_WRITE,
    				MAP_SHARED | MAP_ANONYMOUS , -1, 0);
    	if(s == MAP_FAILED){
    	      perror("erreur mmap:");
    	      exit(1);
}*/
  init(s, nthreads, qlen);
  
  empiler((s-> p), f, closure);
  
  createThreads(s);
  
  freeSched(s);
  
  return (0);
}

int sched_spawn(taskfunc f, void *closure, struct scheduler *s){
  puts("Dans spawn");
  pthread_mutex_lock(&(s->mutex));
  empiler((s->p), f, closure);
  pthread_mutex_unlock(&(s->mutex));

  exit(EXIT_SUCCESS);
}
