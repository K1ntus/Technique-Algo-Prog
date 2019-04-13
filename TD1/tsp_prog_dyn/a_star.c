#include "tools.h"
#include "heap.h" // il faut aussi votre code pour heap.c


// Une fonction de type "heuristic" est une fonction h() qui renvoie
// une distance (double) entre une position de départ et une position
// de fin de la grille. La fonction pourrait aussi dépendre de la
// grille, mais on ne l'utilisera pas forcément ce paramètre.
typedef double (*heuristic)(position,position,grid*);


// Heuristique "nulle" pour Dijkstra.
double h0(position s, position t, grid *G){
  return 0.0;
}


// Heuristique "vol d'oiseau" pour A*.
double hvo(position s, position t, grid *G){
  return fmax(abs(t.x-s.x),abs(t.y-s.y));
}

// Heuristique "alpha x vol_d'oiseau" pour A*.
static double alpha=0; // 0 = h0, 1 = hvo, 2 = approximation ...
double halpha(position s, position t, grid *G) {
  return alpha*hvo(s,t,G);
}

static bool tick_updater = false;

// Structure "noeud" pour le tas min Q.
typedef struct node {
  position pos;        // position (.x,.y) d'un noeud u
  double cost;         // coût[u]
  double score;        // score[u] = coût[u] + h(u,end)
  struct node* parent; // parent[u] = pointeur vers le père, NULL pour start
} *node;


// Les arêtes, connectant les cases voisines de la grille (on
// considère le 8-voisinage), sont valuées par seulement certaines
// valeurs possibles. Le poids de l'arête u->v, noté w(u,v) dans le
// cours, entre deux cases u et v voisines est déterminé par la valeur
// de la case finale v. Plus précisément, si la case v de la grille
// contient la valeur C, le poids de u->v vaudra weight[C] dont les
// valeurs numériques exactes sont définies ci-après. La liste des
// valeurs possibles d'une case est donnée dans "tools.h": V_FREE,
// V_WALL, V_WATER, ... Remarquer que weight[V_WALL]<0, ce n'est pas
// une valuation correcte. Ce n'est pas choquant puisque si en
// position (i,j) si G.value[i][j] = V_WALL, alors c'est que le sommet
// à cette position n'existe pas.

double weight[]={
    1.0,  // V_FREE
  -99.0,  // V_WALL
    3.0,  // V_SAND
    9.0,  // V_WATER
    2.3,  // V_MUD
    1.5,  // V_GRASS
    0.1,  // V_TUNNEL
};




void print_heap(heap h, char format[]) {

  printf("--\n");
  if (h == NULL) {
    printf("heap = NULL\n\n");
    return;
  }

  int i, k = 1;
  int n = 0; // n=nombre d'éléments affichés
  int p = 0; // p=hauteur

  while (n < h->size) {
    k = (1 << p);
    for (i = 0; (i < k) && (n < h->size); i++, n++)
      printf(format, *(int *)h->array[n + 1]);
    printf("\n");
    p++;
  }

  for (i = 0; i < k; i++)
    printf("---");
  printf("\n\n");
}

double dist(double x, double y, position B) {
  return ( sqrt(pow((x - B.x), 2) + pow((y - B.y), 2) ));
}

node generate_node(double x, double y, position start, position end, node parent, heuristic h, grid G){
  node res = malloc(sizeof(*res));
  res->pos.x = x;
  res->pos.y = y;

  res->cost = dist(x, y, start);
  res->score = res->cost + h(start, end, &G) * dist(x, y, end);

  res->parent = parent;

  return res;
}

int get_node_indice_heap(heap Q, double key){
  for(int i = 1; i < Q->size; i++){
    node res = (node) Q->array[i];
    if(res->cost == key)
      return i;
  }

  return heap_top(Q);
}

void refresh_node_value(heap Q, node new_node, double key){//key == node.cost
  int i = get_node_indice_heap(Q, key);
  Q->array[i] = new_node;
}

void manage_neighbor (node main, grid G, heap Q, heuristic h){
  int x_main = main->pos.x;
  int y_main = main->pos.y;

  int x = x_main, y = y_main;
  node left, right, bottom, top;
  node topleft, bottomleft, topright, bottomright;

  y = y_main;
  x = x_main-1;//left
  if(G.mark[x][y] != M_FRONT  && G.value[x][y] != V_WALL){
    left = generate_node(x, y, G.start, G.end, main, h, G);
    left->cost = main->cost + 1;
    heap_add(Q, left);
    G.mark[x][y] = M_FRONT;
  } else if (G.mark[x][y] == M_FRONT && G.value[x][y] != V_FREE && G.value[x][y] != V_WALL) {

    left = get_node_indice_heap(Q, main->cost+1);
    if(main->cost+1 <= left->cost){
      left->cost = main->cost+1;
      left->parent = main;
    }
  }

/*
  y = y_main+1;//bottomleft
  if(G.mark[x][y] != M_FRONT  && G.value[x][y] != V_WALL){
    bottomleft = generate_node(x, y, G.start, G.end, main, h, G);
    bottomleft->cost = main->cost + 5;
    heap_add(Q, bottomleft);
    G.mark[x][y] = M_FRONT;
  } else if (G.mark[x][y] == M_FRONT && G.value[x][y] != V_FREE && G.value[x][y] != V_WALL) {

    bottomleft = get_node_indice_heap(Q, main->cost+1);
    if(main->cost+1 <= bottomleft->cost){
      bottomleft->cost = main->cost+1;
      bottomleft->parent = main;
    }
  }

  y = y_main-1; //Top left
  if(G.mark[x][y] != M_FRONT  && G.value[x][y] != V_WALL){
    topleft = generate_node(x, y, G.start, G.end, main, h, G);
    topleft->cost = main->cost + 5;
    heap_add(Q, topleft);
    G.mark[x][y] = M_FRONT;
  } else if (G.mark[x][y] == M_FRONT && G.value[x][y] != V_FREE && G.value[x][y] != V_WALL) {

    topleft = get_node_indice_heap(Q, main->cost+1);
    if(main->cost+1 <= topleft->cost){
      topleft->cost = main->cost+1;
      topleft->parent = main;
    }
  }
  */

  x = x_main+1;//right
  if(G.mark[x][y] != M_FRONT && G.value[x][y] == V_FREE){
    topright = generate_node(x, y, G.start, G.end, main, h, G);
    topright->cost = main->cost + 1;
    heap_add(Q, topright);
    G.mark[x][y] = M_FRONT;
  } else if (G.mark[x][y] == M_FRONT && G.value[x][y] != V_FREE && G.value[x][y] != V_WALL) {

    topright = get_node_indice_heap(Q, main->cost+1);
    if(main->cost+1 <= topright->cost){
      topright->cost = main->cost+1;
      topright->parent = main;
    }
  }

/*
  y = y_main-1;//bottomright
  if(G.mark[x][y] != M_FRONT && G.value[x][y] == V_FREE){
    bottomright = generate_node(x, y, G.start, G.end, main, h, G);
    bottomright->cost = main->cost + 2;
    heap_add(Q, bottomright);
    G.mark[x][y] = M_FRONT;
  } else if (G.mark[x][y] == M_FRONT && G.value[x][y] != V_FREE && G.value[x][y] != V_WALL) {

    bottomright = get_node_indice_heap(Q, main->cost+1);
    if(main->cost+1 <= bottomright->cost){
      bottomright->cost = main->cost+1;
      bottomright->parent = main;
    }
  }

  y = y_main +1;//bottomright
  if(G.mark[x][y] != M_FRONT && G.value[x][y] == V_FREE){
    right = generate_node(x, y, G.start, G.end, main, h, G);
    right->cost = main->cost + 2;
    heap_add(Q, right);
    G.mark[x][y] = M_FRONT;
  } else if (G.mark[x][y] == M_FRONT && G.value[x][y] != V_FREE && G.value[x][y] != V_WALL) {

    right = get_node_indice_heap(Q, main->cost+1);
    if(main->cost+1 <= right->cost){
      right->cost = main->cost+1;
      right->parent = main;
    }
  }
*/


  /////////////////////////////////////
  x=x_main;
  y = y_main+1;
  if(G.mark[x][y] != M_FRONT && G.value[x][y] == V_FREE){
    bottom = generate_node(x, y, G.start, G.end, main, h, G);
    bottom->cost = main->cost + 1;
    heap_add(Q, bottom);
    G.mark[x][y] = M_FRONT;
  } else if (G.mark[x][y] == M_FRONT && G.value[x][y] != V_FREE && G.value[x][y] != V_WALL) {

    bottom = get_node_indice_heap(Q, main->cost+1);
    if(main->cost+1 <= bottom->cost){
      bottom->cost = main->cost+1;
      bottom->parent = main;
    }
  }

  y = y_main-1;
  if(G.mark[x][y] != M_FRONT && G.value[x][y] == V_FREE){
    top = generate_node(x, y, G.start, G.end, main, h, G);
    top->cost = main->cost + 1;
    heap_add(Q, top);
    G.mark[x][y] = M_FRONT;
  } else if (G.mark[x][y] == M_FRONT && G.value[x][y] != V_FREE && G.value[x][y] != V_WALL) {

    top = get_node_indice_heap(Q, main->cost+1);
    if(main->cost+1 <= top->cost){
      top->cost = main->cost+1;
      top->parent = main;
    }
  }


}

int fcmp_score_nodes(const node left, const node right) { return left->score - right->score; }

bool generate_astar_heap(heap Q, grid G, heuristic h){
  heap_add(Q, generate_node(G.start.x, G.start.y, G.start, G.end, NULL, h, G));
  char fmt[] = "%02i ";
  //print_heap(Q, fmt);
  //heap_add(Q, generate_node(G.end.x, G.end.y, G.start, G.end, NULL));

  printf("Start: (%d,%d)\n", G.start.x, G.start.y);
  printf("End: (%d,%d)\n", G.end.x, G.end.y);

  return true;
}

void get_heap_path(grid G, heap Q, node parent){
  node current = parent;
  double cost=0;
  int nb_node_visited = 0;
  //node* res = (node*) malloc(sizeof(node) * dist(G.start.x, G.start.y, parent.position));
  while(current->parent != NULL){
    nb_node_visited +=1;
    cost += current->cost;
    G.mark[current->pos.x][current->pos.y] = M_PATH;
    current = current->parent;
  }

  printf("*********************************\n");
  printf("* Path found.\n");
  printf("* Number of Nodes: %d\n", nb_node_visited);
  printf("* Cost of the path: %d\n", cost);
  printf("*********************************\n");

}
// Votre fonction A_star(G,h) doit construire un chemin dans la grille
// G entre la position G.start et G.end selon l'heuristique h(). S'il
// n'y a pas de chemin, affichez un simple message d'erreur. Sinon,
// vous devez remplir le champs .mark de la grille pour que le chemin
// trouvé soit affiché plus tard par drawGrid(G). La convention est
// qu'en retour G.mark[i][j] = M_PATH ssi (i,j) appartient au chemin
// trouvé (cf. "tools.h").
//
// Pour gérer l'ensemble P, servez-vous du champs G.mark[i][j] (=
// M_USED ssi (i,j) est dans P). Par défaut, ce champs est initialisé
// partout à M_NULL par initGrid().
//
// Pour gérer l'ensemble Q, vous devez utiliser un tas min de noeuds
// (type node) avec une fonction de comparaison qui dépend du champs
// .score des noeuds. Pensez que la taille du tas Q est au plus la
// somme des degrés des sommets dans la grille. Pour visualiser un
// noeud de coordonnées (i,j) qui passe dans le tas Q vous pourrez
// mettre G.mark[i][j] = M_FRONT au moment où vous l'ajoutez.

void A_star(grid G, heuristic h){
  drawGrid(G); // dessine la grille

  heap Q = heap_create(G.X*G.Y*8, fcmp_score_nodes);

  // Pensez à dessiner la grille avec drawGrid(G) à chaque fois que
  // possible, par exemple, lorsque vous ajoutez un sommet à P mais
  // aussi lorsque vous reconstruisez le chemin à la fin de la
  // fonction. Lorsqu'un sommet passe dans Q vous pourrez le marquer
  // M_FRONT (dans son champs .mark) pour le distinguer des sommets de
  // P (couleur différente).
  generate_astar_heap(Q, G, h);



  char fmt[] = "%02i ";
  int i = 0;
  while(!heap_empty(Q)){
    if(i < 5 && tick_updater){

      print_heap(Q, fmt);
    }
    i++;

    node current = heap_pop(Q);
    //print_heap(Q, fmt);
    if(current->pos.x == G.end.x && current->pos.y == G.end.y){ //if the node is the same as the end
      printf("path found\n");//break;
      get_heap_path(G, Q, current);
      break;
    } else {
      if(G.mark[current->pos.x][current->pos.y] == M_USED)  //if already present in P
        continue;

      G.mark[current->pos.x][current->pos.y] = M_FRONT;
      manage_neighbor(current, G, Q, h);

      if(tick_updater)
        drawGrid(G); // dessine la grille
    }
  }

  ;;;
  // Après avoir extrait un noeud de Q, il ne faut pas le détruire,
  // sous peine de ne plus pouvoir reconstruire le chemin trouvé! Vous
  // pouvez réfléchir à une solution simple pour libérer tous les
  // noeuds devenus inutiles à la fin de la fonction. Une fonction
  // createNode() peut simplifier votre code.
  ;;;
  // Les bords de la grille sont toujours constitués de murs (V_WALL) ce
  // qui évite d'avoir à tester la validité des indices des positions
  // (sentinelle).
  ;;;
  ;;;
  // Améliorations quand vous aurez fini:
  //
  // (1) Une fois la cible atteinte, afficher son coût ainsi que le
  // nombre de sommets visités (somme des .mark != M_NULL). Cela
  // permettra de comparer facilement les différences d'heuristiques,
  // h0() vs. hvo().
  //
  // (2) Le chemin a tendance à zizaguer, c'est-à-dire à utiliser
  // aussi bien des arêtes horizontales que diagonales (qui ont le
  // même coût), même pour des chemins en ligne droite. Essayer de
  // rectifier ce problème d'esthétique en modifiant le calcul de
  // score[v] de sorte qu'à coût[v] égale les arêtes (u,v)
  // horizontales ou verticales soient favorisées.
  //
  // (3) Modifier votre implémentation du tas dans heap.c de façon à
  // utiliser un tableau de taille variable, en utilisant realloc() et
  // une stratégie "doublante": lorsqu'il n'y a pas plus assez de
  // place dans le tableau, on double sa taille. On peut imaginer que
  // l'ancien paramètre 'n' devienne non pas le nombre maximal
  // d'éléments, mais la taille initial du tableau (comme par exemple
  // n=4).
  //
  // (4) Gérer plus efficacement la mémoire en libérant les noeuds
  // devenus inutiles.
  //
  ;;;
}

int main(int argc, char *argv[]){

  unsigned seed=time(NULL)%1000;
    //seed =100;
  printf("seed: %u\n",seed); // pour rejouer la même grille au cas où
  srandom(seed);

  // tester les différentes grilles et positions s->t ...

  //grid G = initGridPoints(80,60,V_FREE,1); // grille uniforme //default
  //grid G = initGridPoints(320,240,V_WALL,0.2); // grille de points aléatoires

  //grid G = initGridPoints(640,480,V_WALL,0.2); // grille de points aléatoires
  //position s={G.X/4,G.Y/2}, t={G.X/2,G.Y/4}; G.start=s; G.end=t; // s->t
  //position s={G.X/4,G.Y/2}, t={G.X/2,G.Y/4}; G.start=s; G.end=t; // s->t

  //grid G = initGridLaby(64,48,4); // labyrinthe aléatoire
  //grid G = initGridLaby(width/8,height/8,3); // labyrinthe aléatoire
  //position tmp; SWAP(G.start,G.end,tmp); // t->s (inverse source et cible)
  grid G = initGridFile("mygrid.txt"); // grille à partir d'un fichier
  //position s={G.X/4,G.Y/2}, t={G.X/2,G.Y/4}; G.start=s; G.end=t; // s->t

  // pour ajouter à G des "régions" de différent types:

  // addRandomBlob(G, V_WALL,   (G.X+G.Y)/20);
  // addRandomBlob(G, V_SAND,   (G.X+G.Y)/15);
  // addRandomBlob(G, V_WATER,  (G.X+G.Y)/3);
  // addRandomBlob(G, V_MUD,    (G.X+G.Y)/3);
  // addRandomBlob(G, V_GRASS,  (G.X+G.Y)/15);
  // addRandomBlob(G, V_TUNNEL, (G.X+G.Y)/4);

  // constantes à initialiser avant init_SDL_OpenGL()
  scale = fmin((double)width/G.X,(double)height/G.Y); // zoom courant
  delay = 80; // délais pour l'affichage (voir tools.h)
  hover = false; // interdire déplacement de points
  init_SDL_OpenGL(); // à mettre avant le 1er "draw"
  drawGrid(G); // dessin de la grille avant l'algo
  update = false; // accélère les dessins répétitifs

  tick_updater = true; //Update frame per frame the algo
  alpha=1;
  A_star(G,halpha); // heuristique: h0, hvo, alpha*hvo


  update = true; // force l'affichage de chaque dessin
  while (running) { // affiche le résultat et attend
    drawGrid(G); // dessine la grille
    handleEvent(true); // attend un évènement
  }

  freeGrid(G);
  cleaning_SDL_OpenGL();
  return 0;
}
