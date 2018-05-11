#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "../include/pile.h"

int pilePleine(Pile *p){
  return ((p -> capa) == (p -> qlen));
}

int pileVide(Pile *p){
  return ((p -> capa) == 0);
}

void empiler(Pile *p, void* nvF, void *args){
  if(pilePleine(p)){
    perror("Pile Pleine");
    return;
  }
  puts("Dans empiler");
  Tache *new = malloc(sizeof(Tache));
    ERROR_STRUCT(new,"pile.c: empiler : malloc");
    ERROR_STRUCT(p, "pile.c: empiler : p");
    ERROR_STRUCT(args, "pile.c: empiler : args");
      new -> f = nvF;
      new -> closure = args;
      new -> svt = p->first;
      p -> first = new;
      (p -> capa) += 1;
      printf("Taille empiler %d : \n", (p -> capa));
}

Tache* depiler(Pile *p){
  ERROR_STRUCT(p, "pile.c: depiler: pile nulle");

  puts("Dans depiler");
  Tache *elementDepile = p->first;
  
  if (p != NULL && p->first != NULL){
    p -> first = elementDepile -> svt;
    (p -> capa) -= 1;
  }
   printf("Taille apres if depiler  %d :\n ", (p -> capa));

   return elementDepile;
}

int freePile(Pile *p){
  ERROR_STRUCT(p, "pile.c: freePile: p null");
  Tache *courant = p->first;
  Tache *tmp;
  while(courant != NULL){
    tmp = courant;
    courant = courant->svt;
    if(courant != NULL)
      free(tmp);
  }
  free(p);
  return 1;
}


