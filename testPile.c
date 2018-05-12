#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

typedef struct Tache Tache;
struct Tache
{
    int nombre;
    void *svt;
};

typedef struct Pile Pile;
struct Pile
{
    Tache *first;
    int capa;
};

Pile *initialiser()
{
    Pile *pile = malloc(sizeof(*pile));
    pile->capa = 0;
    pile->first = NULL;
}

void empiler(Pile *pile, int nvNombre)
{
    Tache *new = malloc(sizeof(*new));
    if (pile == NULL || new == NULL)
    {
        exit(EXIT_FAILURE);
    }

    new->nombre = nvNombre;
    new->svt = pile->first;
    pile->first = new;
    (pile->capa)++;
}

int depiler(Pile *pile)
{
    if (pile == NULL)
    {
        exit(EXIT_FAILURE);
    }

    int nombreDepile = 0;
    Tache *TacheDepile = pile->first;

    if (pile != NULL && pile->first != NULL)
    {
        nombreDepile = TacheDepile->nombre;
        pile->first = TacheDepile->svt;
        free(TacheDepile);
    	(pile->capa)--;
    }

    return nombreDepile;
}

void afficherPile(Pile *pile)
{
    if (pile == NULL)
    {
        exit(EXIT_FAILURE);
    }
    Tache *actuel = pile->first;

    while (actuel != NULL)
    {
        printf("%d\n", actuel->nombre);
        actuel = actuel->svt;
    }

    printf("\n");
}

int main()
{
    Pile *maPile = initialiser();

    empiler(maPile, 4);
    empiler(maPile, 8);
    empiler(maPile, 15);
    empiler(maPile, 16);
    empiler(maPile, 23);
    empiler(maPile, 42);

    printf("\nEtat de la pile :\n");printf("\ntaille de la pile : %d\n",maPile->capa);
    afficherPile(maPile);

    printf("Je depile %d\n", depiler(maPile));
    printf("Je depile %d\n", depiler(maPile));
printf("\ntaille de la pile : %d\n",maPile->capa);

    printf("\nEtat de la pile :\n");
    afficherPile(maPile);

    return 0;
}


/*
struct _s{
pthread_t t;
deque d;
}

ntreads & n deques
enfHaut();
enfBas();
defHaut();
defBas();


initialisation n-1 deque vide
i deque (contient la tache initiale)

exec{
thread n
fin tache
defilerBas
 si vide
  a: workStealing()
    si pas de travail 
	sleep 1ms puis goto a;

workStealing(){
  tirer au hasard k parmi n
  essai de defHaut
  si ok exec la tache et continuer
  sinon
   iteration k+1[n]
   essai d'exec
   echoue
}

nouvelle tache créer
 enfBas() par le thread qui a fait à schedSpawn(création locale)
 ordonnanceur termine
  tous les thread sont oisif
  ils sont tous endormis après n'avoir pas pu voler



*/
