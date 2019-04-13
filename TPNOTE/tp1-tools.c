#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "tp1.h"


#define RAND01  ((double)random()/(double)RAND_MAX) /* réel aléatoire dans [0,1[ */


inline double dist(point A, point B){
  return hypot(B.x-A.x,B.y-A.y);
}


point *read(char *file, int *n){

  // ouvre le fichier
  FILE *f=fopen(file,"r");
  if(f==NULL){ printf("Cannot open file \"%s\"\n",file); exit(1); }

  // lit le nombre de points
  fscanf(f,"%i\n",n);

  // lit les points
  point *P=malloc((*n)*sizeof(*P));
  int i=0;
  while(!feof(f)){
    fscanf(f,"%lf %lf\n",&(P[i].x),&(P[i].y));
    i++; // compte le nombre de points lus
  }
  fclose(f);

  if((i!=(*n))||((*n)<2)||(P==NULL)){ printf("Incorrect number of points.\n"); exit(1); }
  return P;
}


point *point_random(int n){
  if(n<1) return NULL;
  point *P=malloc(n*sizeof(*P));
  for(int i=0;i<n;i++){
    P[i].x=RAND01;
    P[i].y=RAND01;
  }
  return P;
}
