#ifndef H_PILE
#define H_PILE

#define ERROR_THREAD(a,str) if(a !=0){perror(str); exit(EXIT_FAILURE);}
#define ERROR_INT(a,str) if(a<0){errno = EINVAL; perror(str); exit(EXIT_FAILURE);}
#define ERROR_STRUCT(a,str) if(a == NULL){errno = EINVAL; perror(str); exit(EXIT_FAILURE);}

typedef struct Tache Tache;
typedef struct Pile Pile;

struct Tache{
  //taskfunc f;
  void *f;
  void *closure;
  Tache *svt;
};
struct Pile{
  int capa; //taille de la Pile
  int qlen; //capacité max de la pile
  Tache *first; //premier element
};


//empiler une tâche
void empiler(Pile *pile, void * nvF, void *args);

//depiler une tâche
Tache* depiler(Pile *pile);

// tester si la pile est pleine
int pilePleine(Pile *p);

// tester si la pile est vide
int pileVide(Pile *p);

// enlever une pile de la memoire
int freePile(Pile *p);

#endif
