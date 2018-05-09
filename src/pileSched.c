#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/uio.h>
#include <pthread.h>
//gcc -o test pileSched.c -lpthread -Wall -pedantic

struct Tache{
  taskfunc f;
  void *closure;
  Tache *svt;
};
struct Pile{
  int capa; //capacité de la Pile
  Tache *first; //premier element
};

struct scheduler{
  int qlen; //capacité minimum de tache simultanées
  int nthreads;
  Pile *pile;
  pthread_mutex_t mutex;
  pthread_mutexattr_t attr;
  //pthread_cond_t cond;
  //pthread_cond_t attrCond;
};

Pile *initPile(int c){
    Pile *pile = malloc(sizeof(Pile));
    ERROR_MEMOIRE(pile,"pileSched.c: initPile : malloc");
    pile->capa = c ;
    pile->first = NULL;
    return pile;
}

void empiler(Pile *pile, taskfunc nvF, void *args){
  int t = 0;
    Tache *new = malloc(sizeof(Tache));
    ERROR_MEMOIRE(new,"pileSched.c: empiler : malloc");
    ERROR_STRUCT(pile, "pileSched.c: empiler : pile");
    ERROR_STRUCT(new, "pileSched.c: empiler : new");
    ERROR_STRUCT(args, "pileSched.c: empiler : args");
    if(t < (pile-> capa)){
      printf("%d \n",t);
      new->f = nvF;
      new->closure = args;
      new->svt = pile->first;
      pile->first = new;
      t++;
    }
    else{
      perror("Capacité de la pile atteinte");
      exit(EXIT_FAILURE);
    }
}

Tache* depiler(Pile *pile){
  ERROR_STRUCT(pile, "pileSched.c: depiler: pile");
  Tache *t = malloc(sizeof(Tache));
  ERROR_MEMOIRE(t, "pileSched.c: depiler: t");
  Tache *elementDepile = pile->first;
    if (pile != NULL && pile->first != NULL){
      t = elementDepile;
      pile->first = elementDepile->svt;
      free(elementDepile);
	//(pile->capa)--;
    }

    return elementDepile;
}

int init(int nthreads, int qlen, taskfunc f, void *closure){

  int error;
  ERROR_INT(qlen, "qlen doit être positif");
  struct scheduler *sched = malloc(sizeof(struct scheduler));
  ERROR_MEMOIRE(sched, "pileSched.c: init : malloc");
  
  sched->pile = initPile(qlen);
  sched->qlen = qlen;
  
  error = pthread_mutexattr_init(&sched->attr);
  ERROR_THREAD(error,"pileSched.c: init : pthread_mutexattr_init");
  error = pthread_mutexattr_setpshared(&sched->attr, PTHREAD_PROCESS_SHARED);
  ERROR_THREAD(error, "pileSched.c: init: pthread_mutexattr_setpshared");
  error = pthread_mutex_init(&sched->mutex, &sched->attr);
  ERROR_THREAD(error, "pileSched.c: init: pthread_mutex_init");
  
  if(nthreads <= 0){
    sched -> nthreads = *(int*)sched_default_threads;
  }
  else{
    sched -> nthreads = nthreads;
    empiler(sched->pile, f, closure);
  }

  return 0;
}

static void * lancer_threads(void *ptr){
  int err = 0;
  struct scheduler *s = (struct scheduler*)ptr;
  ERROR_STRUCT(s, "pileSched.c: lancer_thread : ptr");
    
  Tache *t = malloc(sizeof(Tache));
  ERROR_STRUCT(t, "pileSched.c: lancer_threads : t");
  
  err = pthread_mutex_lock(&s->mutex);
  ERROR_THREAD(err,"pileSched.c:lancer_threads: mutex_lock");
  
  printf("mutex pris avant depiler \n");

  t = depiler(s -> pile);
  printf("apres depiler \n"); 
  t->f(t->closure, s);
  printf("fin creation tache \n");
  
  err = pthread_mutex_unlock(&s->mutex);
  ERROR_THREAD(err,"pileSched.c: lancer_thread: mutex_unlock");
  
printf("fin lancer\n");
  pthread_exit(NULL);
}

int sched_init(int nthreads, int qlen, taskfunc f, void *closure){
  struct scheduler *s = malloc(sizeof(struct scheduler));
  ERROR_MEMOIRE(s," pileSched: sched_init: pthread_create")
    
  init(nthreads, qlen, f, s);
  int err = 0;
  int nth = 0; pthread_t th[nthreads];
  
  do{
    err = pthread_create(&th[nth], NULL, lancer_threads, &s);
      ERROR_THREAD(err," pileSched: sched_init: pthread_create");
      
      printf("%d: thread [%ld]\n",nth, pthread_self() );
      nth++;

  }while((nth < nthreads) || (s->pile != NULL));

  for(int i = 0; i < nth; i++){
    err = pthread_join(th[nth], NULL);
    ERROR_THREAD(err," pileSched: sched_init: pthread_join");
  }
  
  return 0;
}

int sched_spawn(taskfunc f, void *closure, struct scheduler *s){
  pthread_mutex_lock(&s->mutex);
  empiler(s->pile, f, closure);
  pthread_mutex_unlock(&s->mutex);
  return 0;
}

void afficherPile(Pile *pile){

   ERROR_STRUCT(pile, "pileSched.c: afficherPile : pile");
    Tache *actuel = pile->first;

    while (actuel != NULL){
        printf("%p\n", actuel->closure);
        actuel = actuel->svt;
    }
    printf("\n");
}
/*
static void sched_mutex_destroy(struct scheduler *sched){
   ERROR_STRUCT(sched, "pileSched.c: mutex_destroy : sched");
   pthread_mutex_destroy(&sched->attr);
    pthread_mutex_destroy(&sched->mutex);
}   
static void sched_cond_destroy(struct scheduler *sched){
  error = pthread_condattr_init(&sched->attrCond);
  ERROR_THREAD(error, "pileSched.c : mutex_destroy : pthread_condattr_init : attrCond");
  error = pthread_condattr_setpshared(&sched->attrCond, PTHREAD_PROCESS_SHARED);
  ERROR_THREAD(error, "pileSched.c : mutex_destroy : pthread_condattr_setpshared : attrCond");
  error = pthread_cond_init(&sched->cond, &sched->attrCond);
  ERROR_THREAD(error, "pileSched.c : mutex_destroy : pthread_cond_init : cond");
}
*/
