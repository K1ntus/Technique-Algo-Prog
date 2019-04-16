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
  //Cast des void * en point
  const point * pA = A;
  const point * pB = B;

  //Verification des ordonnées
  if(pA->y == pB->y)
    return 0;
  else if(pA->y < pB->y)
        return -1;
      else
        return 1;
}

// fonction de comparaison (à completer ...) des abscisses de deux
// points A et B: renvoie -1 si "A<B", +1 si "A>B" et 0 sinon
int fcmp_x(const void *A, const void *B){
  //Cast des void * en point
  const point * pA = A;
  const point * pB = B;

  //Verification des abcisses
  if(pA->x == pB->x)
    return 0;
  else if(pA->x < pB->x)
        return -1;
      else
        return 1;
}


// algorithme naïf en O(n^2)
// on suppose que P contient au moins n>=2 points
paire algo_naif(point *P, int n){
  //Initialisation aux deux premiers points du tableau
  paire resultat={P[0],P[1]};

  //Double boucle pour parcourir des paires de point
  for(int i = 0; i < n-1; i++)
  {
    for(int j = i+1; j<n; j++)
    {
      if(dist(P[i],P[j]) < dist(resultat.A, resultat.B))
      {
          resultat.A = P[i];
          resultat.B = P[j];
      }
    }
  }
  return resultat;
}

//Algorithme pppprec
//Px = tableau de point trié selon les abcisses
//Py = tableau de point trié selon les coordonnées
//n = nombre de point dans les tableaux
paire rec(point *Px, point *Py, int n)
{
  if(n <= 3)
    return algo_naif(Px,n);

  //n/2 points seront situés avant le point Px[taille]
  int taille = n/2 +1;
  point median = Px[taille - 1];

  //Création du tableau Ay & By
  point Ay[taille];
  point By[n-taille];

  //Copie des valeurs de Px dans Ay & By
  for(int i = 0; i < n; i++)
  {
    if(i < taille)
      Ay[i] = Px[i];
    else
      By[i - taille] = Px[i];
  }

  //Tri des tableaux Ay & Py
  qsort(Ay,taille,sizeof(point),fcmp_y);
  qsort(By,n-taille,sizeof(point),fcmp_y);

  //Appel recursif sur les tableaux découpes selon P.x
  paire res_a = rec(Px, Ay, taille);
  paire res_b = rec(&Px[taille], By, n - taille);

  //Calcul de delta
  double delta;
  if(dist(res_a.A, res_a.B) < dist(res_b.A, res_b.B))
    delta = dist(res_a.A, res_a.B);
  else
    delta = dist(res_b.A, res_b.B);

  //Création de Sy
  point Sy[n];
  int tailleSy = 0;

  //Attribution des valeurs à Sy & détermination de sa taille
  for(int i = 0; i < n; i++)
      if( sqrt(pow(median.x - Py[i].x, 2)) < delta)
      {
        Sy[tailleSy] = Py[i];
        tailleSy++;
      }

  if(tailleSy < 2)
  {
    printf("Error (Taille = %d)!\n",tailleSy);
    if(dist(res_a.A, res_a.B) < dist(res_b.A, res_b.B))
      return res_a;
    else
      return res_b;
  }

  //On cherche le minimum dans Sy
  paire min_sy = {Sy[0],Sy[1]};
  for(int i = 0; i < tailleSy; i++)
  {
    for(int j = 1; j < 8; j++)
    {
      if(i+j < tailleSy)
      {
          if(dist(Sy[i],Sy[i+j]) < dist(min_sy.A,min_sy.B))
          {
            min_sy.A = Sy[i];
            min_sy.B = Sy[i+j];
          }
      }
    }
  }

  //On chercher la paire de point ayant la distance la plus faible entre min_sy, res_a, res_b
  if(dist(min_sy.A,min_sy.B) < delta)
    return min_sy;
  else if(dist(res_a.A, res_a.B) < dist(res_b.A, res_b.B))
        return res_a;
      else
        return res_b;
}

// algorithme récursif en O(nlogn)
// on suppose que P contient au moins n>=2 points
paire algo_rec(point *P, int n){

  //Intialisation d'un second tableau P'
  //Il n'y a pas eu d'utilisation de malloc à cause d'un bug mémoire et je n'avais pas le temps de m'en occuper
  point P_prime[n];

  //Copie de p dans p'
  for(int i = 0; i < n; i ++)
    P_prime[i] = P[i];

  //Tri de P selon les abcisses
  qsort(P,n,sizeof(point),fcmp_x);

  //Tri de P' selon les ordonnées
  qsort(P_prime,n,sizeof(point),fcmp_y);

  //Lancement de l'algo récursif
  paire resultat= rec(P,P_prime,n);

  return resultat;
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
