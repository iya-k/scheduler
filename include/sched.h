#define ERROR_THREAD(a,str) if(a !=0){perror(str); exit(EXIT_FAILURE);}
#define ERROR_INT(a,str) if(a<0){errno = EINVAL; perror(str); exit(EXIT_FAILURE);}
#define ERROR_STRUCT(a,str) if(a == NULL){errno = EINVAL; perror(str); exit(EXIT_FAILURE);}
#define ERROR_MEMOIRE(a,str) if(a == NULL && errno != 0){perror(str); exit(EXIT_FAILURE);}

struct scheduler;

typedef void (*taskfunc)(void*, struct scheduler *);

typedef struct Tache Tache;
typedef struct Pile Pile;

static inline int
sched_default_threads(){
    return sysconf(_SC_NPROCESSORS_ONLN);
}
int sched_init(int nthreads, int qlen, taskfunc f, void *closure);
int sched_spawn(taskfunc f, void *closure, struct scheduler *s);
