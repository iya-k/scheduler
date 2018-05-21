#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/uio.h>

#include <unistd.h>
#include <pthread.h>

#include "../include/deque.h"
#include "../include/sched.h"

typedef struct Couple{
  pthread_t *th; 
  Deque *d;
}Couple;

struct scheduler{
	int nthreads;
	int nbr;
	int qlen;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	Couple *cp;
};

int freeCouple(Couple cp){

  freeDeque(cp.d);
  free(cp.th);
  //if(munmap((void *) cp, sizeof(Couple)) < 0)
  // exit(errno);

  return 0;
}

int sched_destroy(struct scheduler *s){
int i;
  pthread_mutex_destroy(&(s->mutex));
  for (i = 0; i < s->nthreads; i++){
    freeCouple(s->cp[i]);
  }
  if(munmap((void *) s, sizeof(struct scheduler)) < 0)
     exit(errno);

  return 0;
}

struct scheduler *init(int nthreads, int qlen){
  ERROR_INT(qlen, "stealing.c: init: qlen doit etre > 0\n");
  int error = 0, i; struct scheduler *s;
  if((s = (struct scheduler*) mmap(NULL, sizeof(struct scheduler), PROT_READ | PROT_WRITE,
				   MAP_SHARED | MAP_ANONYMOUS , -1, 0)) == MAP_FAILED)
    gestion_erreur("sched_init mmap");

  pthread_mutexattr_t psharedm;
  if((error = pthread_mutexattr_init(&psharedm)) != 0)
    gestion_erreur(" pthread_mutexattr_init");
  if((error = pthread_mutexattr_setpshared(&psharedm,PTHREAD_PROCESS_SHARED)) != 0)
     gestion_erreur(" pthread_mutexattr_setpshared");

  pthread_condattr_t cattr;
  if((error = pthread_condattr_init(&cattr)) != 0)
    gestion_erreur(" pthread_mutexattr_init");
  if((error = pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED)) != 0)
    gestion_erreur("init pthread_condattr_setpshared");

  s->nthreads = (nthreads == 0)? sched_default_threads():nthreads;
  s->nbr = 0;
  s->qlen = qlen;
  if((s->cp = (Couple *) mmap(NULL, nthreads*sizeof(*s->cp), PROT_READ | PROT_WRITE,
				   MAP_SHARED | MAP_ANONYMOUS , -1, 0)) == MAP_FAILED)
    gestion_erreur("init mmap couple");

  for (i = 0; i < s->nthreads; i++){
    s->cp[i].th =(pthread_t*) malloc(sizeof(pthread_t));

    /*if((s->cp[i].d = (Deque *) mmap(NULL, sizeof(Deque), PROT_READ | PROT_WRITE,
    				      MAP_SHARED | MAP_ANONYMOUS , -1, 0)) == MAP_FAILED)
      gestion_erreur("sched_init mmap");*/
    s->cp[i].d = malloc(sizeof(Deque));
    
     initDeque(s->cp[i].d);
}

  if((error = pthread_mutex_init(&s->mutex, &psharedm)) !=0)
    gestion_erreur(" pthread_mutex_init");
  if((error = pthread_cond_init(&(s->cond),&cattr)) !=0)
    gestion_erreur(" pthread_cond_init");
  
  return s; 
}

int threadId(struct scheduler *s){
  pthread_t t = pthread_self();
  for (int i = 0; i < s->nthreads; i++)
    if(pthread_equal(t, *s->cp[i].th))
      return i;
  return -1;
}

void* lancerThreads(void* ptr){
  int err;
  struct scheduler *s = (struct scheduler*)ptr;
  threadId(s);
  /*
  while(1){
    if((err = pthread_mutex_lock(&s->mutex)) != 0)
      gestion_erreur("lancerThreads pthread_mutex_lock");
    //Tache* t = depiler(s->pile);
    if((err = pthread_mutex_unlock(&s->mutex)) !=0)
      gestion_erreur("lancerThreads pthread_mutex_unlock");
    
    if(t != NULL) {
      pthread_mutex_lock(&s->mutex);
      s->nbr++;
      pthread_mutex_unlock(&s->mutex);
      ((taskfunc) t->f)(t->closure, s);
			
      pthread_mutex_lock(&s->mutex);
      s->nbr--;
      pthread_mutex_unlock(&s->mutex);

      free(t);
    }
  }*/
  
  pthread_exit(NULL);
}

int createThreads(struct scheduler *s){
  int i, err;
  for(i = 0; i < s->nthreads;i++){
    if((err = pthread_create(s->cp[i].th, NULL, lancerThreads, s)) != 0)
      gestion_erreur("createThreads pthread_create");
  }

  for(i = 0; i < s->nthreads;i++){
    if((err = pthread_join(*s->cp[i].th, NULL)) != 0)
      gestion_erreur("createThreads pthread_join");
  }

  return 0;
}

int sched_init(int nthreads, int qlen, taskfunc f, void *closure){
  printf("sched init\n");
  srand(time(NULL));
  struct scheduler *s = init(nthreads,qlen);
  enfilerHaut(s->cp[0].d,f,closure);
  createThreads(s);
  sched_destroy(s);
  puts("fin sched init\n");
  return 0;
}

int sched_size(struct scheduler *s){
  int capa = 0, i;
  for(i = 0; i < s->nthreads; i++){
    capa += s->cp[i].d->capa;
  }
  return capa;
}

int sched_spawn(taskfunc f, void *closure, struct scheduler *s){
  puts("spawn\n");
  /* if(sched_size(s) >= s->qlen){
    perror("Ajout impossible qlen atteind\n");
    return -1;
    }*/
  int err, id;
  id = threadId(s);
  srand(time(NULL));
  if((err = pthread_mutex_lock(&s->mutex)) != 0)
    gestion_erreur("sched_spawn mutex_lock");

  enfilerHaut(s->cp[id].d,f,closure);
  pthread_cond_broadcast(&s->cond);
  
  if((err = pthread_mutex_unlock(&s->mutex)) != 0)
    gestion_erreur("sched_spawn mutex_unlock");

  return 0;
}

