#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
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
  Tache *new = malloc(sizeof(Tache));
    ERROR_STRUCT(new,"pile.c: empiler : malloc");
    ERROR_STRUCT(p, "pile.c: empiler : p");
    ERROR_STRUCT(args, "pile.c: empiler : args");
      new -> f = nvF;
      new -> closure = args;
      new -> svt = p->first;
      p -> first = new;
      (p -> capa)++;
      //printf("fin empiler [%ld]:\n", (long)p->first);
}
Tache* depiler(Pile *p){
  ERROR_STRUCT(p, "pile.c: depiler: pile nulle");
  Tache *elementDepile = p->first;
  
  if((p != NULL) && (p -> first) != NULL){
    (p -> first) = elementDepile -> svt;
    (p -> capa)--;
  }
      //printf("fin depiler:\n";

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
  //free(p);
  if(munmap((void *) p, sizeof(Pile)) < 0)
       exit(EXIT_FAILURE);
  return 0;
}


