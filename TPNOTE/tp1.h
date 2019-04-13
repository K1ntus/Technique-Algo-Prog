#ifndef TP1_H
#define TP1_H

#define FORM "%.08lf" // format d'affichage d'une coordonnées

typedef struct{
  double x;
  double y;
} point;


typedef struct{
  point A;
  point B;
} paire;


// distance euclidienne entre les points A et B
extern double dist(point A, point B);

// lit un fichier de points de n points avec n>1, renvoie n ainsi que
// tableau de points
extern point *read(char *file, int *n);

// génère un ensemble de n points aléatoires à coordonnées dans [0,1[
extern point *point_random(int n);

#endif
