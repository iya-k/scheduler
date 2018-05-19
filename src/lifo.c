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
               do {perror(msg); exit(errno); } while (0)

struct scheduler{
	int nthreads;
	int nbr;
	pthread_mutex_t mutex;
	Pile *pile;
	pthread_t *th;
};

int sched_destroy(struct scheduler *s){
  pthread_mutex_destroy(&(s->mutex));
  freePile(s->pile);
  free(s->th);
  if(munmap((void *) s, sizeof(struct scheduler)) < 0)
       exit(EXIT_FAILURE);
  return 0;
}

struct scheduler *init(int nthreads, int qlen){
  int error = 0;
  struct scheduler *s;
  if((s = (struct scheduler*) mmap(NULL, sizeof(struct scheduler), PROT_READ | PROT_WRITE,
				   MAP_SHARED | MAP_ANONYMOUS , -1, 0)) == MAP_FAILED)
    gestion_erreur("init mmap");
  pthread_mutexattr_t psharedm;
  if((error = pthread_mutexattr_init(&psharedm)) != 0)
    gestion_erreur(" pthread_mutexattr_init");
  if((error = pthread_mutexattr_setpshared(&psharedm,PTHREAD_PROCESS_SHARED)) != 0)
     gestion_erreur(" pthread_mutexattr_setpshared");

  if((s->pile = (Pile *) mmap(NULL, sizeof(*s->pile), PROT_READ | PROT_WRITE,
				   MAP_SHARED | MAP_ANONYMOUS , -1, 0)) == MAP_FAILED)
    gestion_erreur("init mmap pile");
  
  s->pile->capa = 0;
  s->pile->qlen = qlen;

  if((error = pthread_mutex_init(&s->mutex, &psharedm)) !=0)
    gestion_erreur(" pthread_mutex_init");
  
  s->nthreads = (nthreads == 0)? sched_default_threads():nthreads;
  s->th =(pthread_t*) malloc(sizeof(pthread_t)*nthreads);
  s->nbr = 0;

return s;
	
}

void* lancerThreads(void* ptr){
  int err;
  struct scheduler *s = (struct scheduler*)ptr;
 
  do {
    if((err = pthread_mutex_lock(&s->mutex)) != 0)
      gestion_erreur("lancerThreads pthread_mutex_lock");
    Tache* t = depiler(s->pile);
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
  } while((0 != s->nbr) || !pileVide(s->pile));
  
  pthread_exit(NULL);
}

int createThreads(struct scheduler *s){
  int i = 0, err;
  for(i = 0; i<s->nthreads;i++){
    if((err = pthread_create(&s->th[i], NULL, lancerThreads, s)) != 0)
      gestion_erreur("createThreads pthread_create");
  }

  for(i = 0; i < s->nthreads;i++){
    if((err = pthread_join(s->th[i], NULL)) != 0)
      gestion_erreur("createThreads pthread_join");
  }

  return 0;
}

int sched_init(int nthreads, int qlen, taskfunc f, void *closure){

  struct scheduler *s = init(nthreads,qlen);
  empiler(s->pile,f,closure);
  createThreads(s);

  sched_destroy(s);
//puts("fin sched init\n");
  return 0;
}

int sched_spawn(taskfunc f, void *closure, struct scheduler *s){
  int err;
  if((err = pthread_mutex_lock(&s->mutex)) != 0)
    gestion_erreur("sched_spawn mutex_lock");

  empiler(s->pile,f,closure);
  
  if((err = pthread_mutex_unlock(&s->mutex)) != 0)
    gestion_erreur("sched_spawn mutex_unlock");
  //puts("fin spawn\n");

	return 0;
}

