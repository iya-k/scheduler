#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/uio.h>

#include <unistd.h>
#include <pthread.h>

#include "../include/deque.h"
#include "../include/sched.h"


struct scheduler{
	Deque **d;
	pthread_t *th;
	pthread_mutex_t mutex;
	int nthreads;
	int nbr;
};

int freeSched(struct scheduler *s){
	for (int i = 0; i < s->nthreads; ++i)
		freeDeque(s->d[i]);
	free(s->th);
	munmap((void *) s, sizeof(struct scheduler));
	return 0;
}

int init(struct scheduler *s,int nthreads,int qlen){
  int error = 0;
  pthread_mutexattr_t psharedm;
  if((error = pthread_mutexattr_init(&psharedm)) != 0)
    gestion_erreur(" pthread_mutexattr_init");
  if((error = pthread_mutexattr_setpshared(&psharedm,PTHREAD_PROCESS_SHARED)) != 0)
     gestion_erreur(" pthread_mutexattr_setpshared");

	s->nbr = 0;
	s->nthreads = (nthreads == 0) ? sched_default_threads() : nthreads;
	s->th =(pthread_t*) malloc(sizeof(pthread_t)*nthreads);
	
	s->d = malloc(sizeof(*s->d) * nthreads);
	for (int i = 0; i < s->nthreads; ++i){
		s->d[i] = initDeque(qlen);
	}
	
  if((error = pthread_mutex_init(&s->mutex, &psharedm)) !=0)
    gestion_erreur(" pthread_mutex_init");

  return 0;
}

int getThreadId(struct scheduler *s){
	pthread_t t = pthread_self();
	for (int i = 0; i < s->nthreads; ++i)
		if(pthread_equal(t, s->th[i]))
			return i;
	return -1;
}

int oisif(struct scheduler *s) {
	for (int i = 0; i < s->nthreads; ++i)
		if(!estVide(s->d[i]))
			return 0;
	return 1;
}

Tache *work(struct scheduler *s){
	int k = rand() % s->nthreads, i = 0;
	Tache *e = NULL;
	while(i < s->nthreads || e == NULL){
		Deque *d = s->d[(k + i) % s->nthreads];
		if(!estVide(d)){
			e = defilerBas(d);
		}
		i++;
	}
	return e;
}

void* lancerThreads(void* ptr){
	struct scheduler *s = (struct scheduler*)ptr;
	printf("+ in\n");
	
	int id = getThreadId(s);
	
	do {
		if(estVide(s->d[id])) {
			pthread_mutex_lock(&s->mutex);
			Tache *e = work(s);
			pthread_mutex_unlock(&s->mutex);
			
			if(e != NULL){
				((taskfunc) e->f)(e->closure, s);
			}
			else
				sleep(1);
		} else {
			pthread_mutex_lock(&s->mutex);
			Tache *e = defilerHaut(s->d[id]);
			pthread_mutex_unlock(&s->mutex);

			if(e != NULL)
				((taskfunc) e->f)(e->closure, s);
		}
	} while(0 != s->nbr || !oisif(s));


	pthread_exit(NULL);
}

int createThreads(struct scheduler *s){
	int i = 0,ret;
	for(i = 0; i<s->nthreads;i++){
		ret = pthread_create(&s->th[i],NULL,lancerThreads,s);
		if(ret != 0){
			gestion_erreur("pthread_create");
			exit(EXIT_FAILURE);
		}
	}

	for(i = 0; i < s->nthreads;i++){
		pthread_join(s->th[i],NULL);
	}

	return 0;
}

int sched_init(int nthreads, int qlen, taskfunc f, void *closure){
	srand(time(NULL));
struct scheduler *s;
	if((s = (struct scheduler*) mmap(NULL, sizeof(struct scheduler), PROT_READ | PROT_WRITE,
				   MAP_SHARED | MAP_ANONYMOUS , -1, 0)) == MAP_FAILED)
    gestion_erreur("sched_init mmap");

	init(s,nthreads,qlen);
	enfilerHaut(s->d[0],f,closure);
	createThreads(s);
	freeSched(s);
	return 1;
}

int sched_spawn(taskfunc f, void *closure, struct scheduler *s){
	int id = getThreadId(s);
	pthread_mutex_lock(&s->mutex);
	enfilerHaut(s->d[id],f,closure);
	pthread_mutex_unlock(&s->mutex); 

	return 1;
}
