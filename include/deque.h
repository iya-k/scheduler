#ifndef H_DEQUE
#define H_DEQUE

#define ERROR_THREAD(a,str) if(a !=0){perror(str); exit(EXIT_FAILURE);}
#define ERROR_INT(a,str) if(a<0){errno = EINVAL; perror(str); exit(EXIT_FAILURE);}
#define ERROR_STRUCT(a,str) if(a == NULL){perror(str); exit(EXIT_FAILURE);}

#define gestion_erreur(msg) \
               do {perror(msg); exit(EXIT_FAILURE); } while (0)

typedef struct Tache Tache;
struct Tache{
  void *f;
  void *closure;
  Tache *prec;
  Tache *svt;
};

typedef struct Deque Deque;
struct Deque{
  int capa; //taille de la Deque
  Tache *p_head; //première tache
  Tache *p_tail; //dernière tache
};

//initialisation de la deque
void initDeque(Deque *d);
//enfiler une tâche
void enfilerHaut(Deque *deque, void * nvF, void *args);
void enfilerBas(Deque *deque, void * nvF, void *args);

//defiler une tâche
Tache* defilerHaut(Deque *deque);
Tache* defilerBas(Deque *deque);

// tester si la pile est pleine
int estPleine(Deque *deque);

// tester si la pile est vide
int estVide(Deque *deque);

// enlever une pile de la memoire
int freeDeque(Deque *deque);

#endif
