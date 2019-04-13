#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "tp1.h"


// fonction de comparaison (à completer ...) ordonnées de deux points
// A et B: renvoie -1 si "A<B", +1 si "A>B" et 0 sinon
int fcmp_y(const void *A, const void *B){
  const point * a =  A;
  const point * b =  B;
  int cmp = a->y - b->y;

  if(cmp > 0)
    return 1;
  else if(cmp < 0)
    return -1;
  return 0;
}

// fonction de comparaison (à completer ...) des abscisses de deux
// points A et B: renvoie -1 si "A<B", +1 si "A>B" et 0 sinon
int fcmp_x(const void *A, const void *B){
  const point * a =  A;
  const point * b =  B;
  int cmp = a->x - b->x;

  if(cmp > 0)
    return 1;
  else if(cmp < 0)
    return -1;
  return 0;
}


// algorithme naïf en O(n^2)
// on suppose que P contient au moins n>=2 points
paire algo_naif(point *P, int n){
  paire res = {P[0], P[1]};

  if(n < 2) //Cas impossible n < 2
    return res;

  if(n == 2){ //Cas n = 2
    res.A = P[0];
    res.B = P[1];
    return res;
  }

  //Cas n > 2
  for(unsigned int i = 0; i < n; i++){
    for(unsigned int j = i+1; j < n; j++){
      if(P[i].x == P[j].x){ continue; }
      double dist_tmp = dist(P[i], P[j]);
      double dist_res = dist(res.A, res.B);
      if(dist_tmp < dist_res){
        res.A = P[i];
        res.B = P[j];
      }
    }
  }

  return res;
}

//Algorithme pppprec
//Px = tableau de point trié selon les abcisses
//Py = tableau de point trié selon les coordonnées
//n = nombre de point dans les tableaux
paire rec(point *Px, point *Py, int n) {
  paire res;

  if(n == 2 || n == 3)
    return algo_naif(Px, n);

  point median = Px[n/2];


  point * A_x = (point *) malloc(sizeof(point) * n/2);
  point * A_y = (point *) malloc(sizeof(point) * n/2);

  point * B_x = (point *) malloc(sizeof(point) * n/2);
  point * B_y = (point *) malloc(sizeof(point) * n/2);


  for(int x = 0; x < n/2; x++){
    for(int y = 0; y < n; y++){
      A_x[x] = Px[x];
      A_y[x] = Py[x];

      B_x[x] = Px[n-x];
      B_y[x] = Py[n-x];
    }
  }

  paire res_aa = rec(A_x, A_y, n/2);
  paire res_bb = rec(B_x, B_y, n/2);

  double theta;
  if(dist(res_aa.A, res_aa.B) < dist(res_bb.A, res_bb.B)){
    theta = dist(res_aa.A, res_aa.B);
  } else {
    theta = dist(res_bb.A, res_bb.B);
  }

  //point * S = (point *) malloc(sizeof(point) * (median-x));

  point Sy[n];
  unsigned int size_sy = 0;
  for(int i = 0; i < n; i++){
    if(abs(median.x - Py[i].x) < theta){
      Sy[size_sy] = Py[i];
      size_sy += 1;
    }
  }

  paire res_ss;
  for(int i = 0; i < size_sy; i++){
    for(int j = i+1; j < i+8; j++){
      double dist_tmp = dist(Sy[i], Sy[j]);
      double dist_ss = dist(res_ss.A, res_ss.B);
      if(dist_tmp < dist_ss){
        res_ss.A = Sy[i];
        res_ss.B = Sy[j];
      }
    }
  }

  if(dist(res_ss.A, res_ss.B) < theta){
    res = res_ss;
  } else if(dist(res_aa.A, res_aa.B) < dist(res_bb.A, res_bb.B)){
    res = res_aa;
  } else {
    res = res_bb;
  }

  free(A_x);
  free(A_y);
  free(B_x);
  free(B_y);

  return res;
}

// algorithme récursif en O(nlogn)
// on suppose que P contient au moins n>=2 points
paire algo_rec(point *P, int n){
  point* P_y = (point *) malloc(sizeof(point) * n);

  for(unsigned int i = 0; i < n; i++){
    P_y[i] = P[i];
  }

  //Tri de P selon les abcisses
  qsort(P,n,sizeof(point),fcmp_x);

  //Tri de P' selon les ordonnées
  qsort(P_y,n,sizeof(point),fcmp_y);

  return rec(P, P_y, n);
}


int main(int argc, char *argv[]){

  srandom(time(NULL));

  if(argc==2){
    //
    // partie à ne pas modifier
    //
    if(!isdigit(argv[1][0])){ printf("Forgot \"naif\" or \"rec\"?.\n"); exit(1); }
    int n=atoi(argv[1]);
    point *P=point_random(n);
    if((P==NULL)||(n<1)){ printf("Empty point set.\n"); exit(1); }
    printf("%i\n",n);
    for(int i=0;i<n;i++)
      printf(FORM" "FORM"\n",P[i].x,P[i].y);
    return 0;
  }

  if(argc==3){
    //
    // partie à ne pas modifier
    //
    int n;
    paire res={{-1,0},{0,0}};
    point *P=read(argv[1],&n);
    if(!strcmp(argv[2],"naif")) res=algo_naif(P,n);
    if(!strcmp(argv[2],"rec")) res=algo_rec(P,n);
    if(res.A.x<0){ printf("Unknown algorithm.\n"); exit(1); }
    printf("point A: "FORM" "FORM"\n",res.A.x,res.A.y);
    printf("point B: "FORM" "FORM"\n",res.B.x,res.B.y);
    printf("distance: "FORM"\n",dist(res.A,res.B));
    return 0;
  }

  // pour vos tests personnels, si nécessaire, modifier le main()
  // seulement ci-dessous

  ;;;;
  ;;;;
  ;;;;

  return 0;
}
