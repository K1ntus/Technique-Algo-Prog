//
//  TSP - BRUTE-FORCE
//
// -> la structure "point" est définie dans tools.h

double dist(point A, point B) {
  return ( sqrt(pow((A.x - B.x), 2) + pow((A.y - B.y), 2) ));
}

double value(point *V, int n, int *P) {
  double sum = 0;
  for (int i = 0; i < n - 1; i++){
    sum += dist( V[P[i]], V[P[i+1]]);
  }
  return sum + dist(V[P[n-1]], V[P[0]]);
}

double tsp_brute_force(point *V, int n, int *Q) {
  int P[n];
  for (int i = 0; i < n; i++){
    P[i] = i;
  }
  double min = value(V, n, P);
  while( NextPermutation(P, n)){
    if (min > value(V, n, P)){
      min = value(V, n, P);
      for (int i = 0; i < n; i++){
        Q[i] = P[i];
      }
    }
  }
  return min;
}

void MaxPermutation(int *P, int n, int k) {
  int i;
  double elementInsere;

  for (i = k; i < n; i++) {
      /* Stockage de la valeur en i */
      elementInsere = P[i];
      /* Décale les éléments situés avant t[i] vers la droite
         jusqu'à trouver la position d'insertion */
         int i = k, j = n-1;
         while(i < j){
             int tmp = P[i];
             P[i] = P[j];
             P[j] = tmp;

             i++, j--;

         }
         /*
      for (j = i; j > 0 && P[j - 1] > elementInsere; j--) {
          int tmp = P[i];
          P[i] = P[j];
          P[j] = tmp;
      }
      */
      /* Insertion de la valeur stockée à la place vacante */
      P[j] = elementInsere;
  }
}

double value_opt(point *V, int n, int *P, double w) {

  double sum = 0;
  int i = 0;
  for(; i < n-1 && (sum+dist(V[P[i]],V[P[i+1]]) < w); i++){
  //while((sum + dist(V[P[i]], V[P[i+1]]) < w) && i < n - 2){
    sum += dist(V[P[i]], V[P[i+1]]);
    //i++;
  }
  if (sum < w){
    return sum;
  }
  else{
    return -(i+1);
  }
}

double tsp_brute_force_opt(point *V, int n, int *Q) {
  int P[n];
  double k;
  for (int i = 0; i < n; i++){
    P[i] = i;
  }

  double min = value(V, n, P);

  while( NextPermutation(P, n)){
    if (value_opt(V, n, P, min) > 0){
      min = value(V, n, P);
      for (int i = 0; i < n; i++){
        Q[i] = P[i];
      }
    }else{
      k = -value_opt(V, n, P, min);
      MaxPermutation(P, k, n);
    }
  }

  return min;
}
