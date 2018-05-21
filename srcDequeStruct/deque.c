#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "../include/deque.h"

void initDeque(Deque *p_new){
    puts("intDeque");
    if (p_new != NULL)
    {
        p_new->capa = 0;
        p_new->p_head = NULL;
        p_new->p_tail = NULL;
    }

}


int estVide(Deque *d){
  return ((d -> p_head) == NULL);
}

void enfilerHaut(Deque *p_list, void* nvF, void *args){
  
    ERROR_STRUCT(p_list, "Deque.c: enfilerHaut : p");
    ERROR_STRUCT(args, "Deque.c: enfilerHaut : args");

    if (p_list != NULL)
    {
       Tache *p_new = malloc(sizeof(Tache));
        if (p_new != NULL)
        {
            p_new->f = nvF;
            p_new-> closure = args;
            p_new->prec = NULL;
            if (p_list->p_tail == NULL)
            {
                p_new->svt = NULL;
                p_list->p_head = p_new;
                p_list->p_tail = p_new;
            }
            else
            {
                p_list->p_head->prec = p_new;
                p_new->svt = p_list->p_head;
                p_list->p_head = p_new;
            }
            p_list->capa++;
       }
    }      


      printf("fin enfilerHaut\n");
}

void enfilerBas(Deque *p_list, void* nvF, void *args){
    if (p_list != NULL) /* On vérifie si notre liste a été allouée */
    {
        Tache *p_new = malloc(sizeof *p_new); /* Création d'un nouveau node */
        if (p_new != NULL) /* On vérifie si le malloc n'a pas échoué */
        {
            p_new->f = nvF; 
            p_new-> closure = args;/* On 'enregistre' notre donnée */
            p_new->svt = NULL; 
            if (p_list->p_tail == NULL) /* Cas où notre liste est vide (pointeur vers fin de liste à  NULL) */
            {
                p_new->prec = NULL; /* On fait pointer p_prev vers NULL */
                p_list->p_head = p_new; /* On fait pointer la tête de liste vers le nouvel élément */
                p_list->p_tail = p_new; /* On fait pointer la fin de liste vers le nouvel élément */
            }
            else /* Cas où des éléments sont déjà présents dans notre liste */
            {
                p_list->p_tail->svt = p_new; /* On relie le dernier élément de la liste vers notre nouvel élément (début du chaînage) */
                p_new->prec = p_list->p_tail; /* On fait pointer p_prev vers le dernier élément de la liste */
                p_list->p_tail = p_new; /* On fait pointer la fin de liste vers notre nouvel élément (fin du chaînage: 3 étapes) */
            }
            p_list->capa++; /* Incrémentation de la taille de la liste */
        }
    }
    //return p_list; /* on retourne notre nouvelle liste */
}

Tache *defilerBas(Deque *d){
	Tache *del = d->p_head;
	if(del != NULL && del->svt == NULL){
		d->p_tail->svt = NULL;
		d->p_tail = del->prec;
		d->capa--;
	}
	return del;
}

Tache *defilerHaut(Deque *d){
	Tache *del = d->p_head;

	if(del != NULL && del->prec == NULL){
		d->p_head->prec = NULL;
		d->p_head = del->svt;
		(d->capa) -= 1;
	}
	return del;
}


int freeDeque(Deque *p){
  ERROR_STRUCT(p, "Deque.c: freeDeque: p null");
  Tache *courant = p->p_head;
  Tache *tmp;
  while(courant != NULL){
    tmp = courant;
    courant = courant->svt;
      free(tmp);
  }
  free(courant);
  free(p);

  return 0;
}


