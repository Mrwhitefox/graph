// LICENSE: Creative Commmons BY-SA-NC
// AUTHOR: V. Marquet

// compilation on linux:
// gcc -Wall -g simu.c -lcsfml-graphics -o simu -lm

// launch:
// ./simu < graphs/exemple.txt  <- change "exemple.txt" by the name of a file in the graphs/ directory


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SFML/Graphics.h>

#define m 1  // masse
#define def_lrepos 5  // longueur au repos au début du programme (peut changer)
#define lrepos_minus 40  // longueur qu'on enlève à lrepos si des noeuds sont en dehors de l'écran
#define lrepos_min 20  // valeur minimale pour la longueur d'un ressort
#define lambda 20   // constante de frottements
#define lambda2 0.75  // autre méthode pour les frottements

struct node
{
   int node_number;  // à partir de 0
   int graph_number; // pour des graphes non reliés
   float pos_x;
   float pos_y;
   float vit_x;
   float vit_y;
   float acc_x;
   float acc_y;
   float force_x;
   float force_y;
   struct node* next_node;
};
typedef struct node node;

struct link
{
   int node_1;
   int node_2;
   struct link* next_link;
};
typedef struct link link;

int simulation(float deltaT, int nbNode, node* node_list, node** node_tab, link* link_list, int lrepos, int k, int coef_repulsion)
{
   node* temp = node_list; int i;
   
   // FORCE DU RESSORT f = -k(x-lrepos)
   // TODO: c'est mal optimisé, il faudrait parcourir directement la liste des liens
   for (i=0; i<nbNode; i++)  // pour chaque noeud
   {
      link* ll_tmp = link_list;
      while (ll_tmp != NULL) // on cherche s'il est lié à un autre dans la liste des liens
      {
         int j;
         // on teste si le noeud est à une des extrémités du lien
         if (ll_tmp->node_1==i)
            j = ll_tmp->node_2;
         else if (ll_tmp->node_2==i)
            j = ll_tmp->node_1;
         else
         {
            ll_tmp = ll_tmp->next_link;  // si non, on passe au lien suivant
            continue;
         }
         
         // si le noeud était à une des extrémités du lien
         
         // on calcule la distance entre les 2 noeuds
         float delta_x = fabsf(node_tab[i]->pos_x-node_tab[j]->pos_x);
         float delta_y = fabsf(node_tab[i]->pos_y-node_tab[j]->pos_y);
         float dist = sqrtf(delta_x*delta_x + delta_y*delta_y);
         
         // on en déduit la force
         float f = -k*(dist-lrepos);  // négatif = étiré, positif = contracté
         
         // pour éviter de diviser par 0
         if (dist == 0)
         {
            ll_tmp = ll_tmp->next_link;
            continue;
         }
         
         // on cherche la projection de la force sur x et y
         float cos_theta = delta_x/dist;
         float sin_theta = delta_y/dist;
         
         if (node_tab[i]->pos_x <= node_tab[j]->pos_x)
         {
            node_tab[i]->force_x -= f*cos_theta;
            node_tab[j]->force_x += f*cos_theta;
         }
         else
         {
            node_tab[i]->force_x += f*cos_theta;
            node_tab[j]->force_x -= f*cos_theta;
         }
         
         if (node_tab[i]->pos_y <= node_tab[j]->pos_y)
         {
            node_tab[i]->force_y -= f*sin_theta;
            node_tab[j]->force_y += f*sin_theta;
         }
         else
         {
            node_tab[i]->force_y += f*sin_theta;
            node_tab[j]->force_y -= f*sin_theta;
         }
         
         ll_tmp = ll_tmp->next_link;  // on passe au lien d'après
      }
      // on passe au noeud d'après
   }
   
   
   // FORCE de répulsion en 1/r avec r la distance entre les noeuds
   for (i=0; i<nbNode-1; i++)  // on parcourt tout les noeuds (sauf le dernier, c'est inutile)
   {
      int j;
      for (j=i+1; j<nbNode; j++)  // on parcours tous les noeuds pas encore traités
      {
         // la force repulsive ne s'applique qu'aux noeuds non directement reliés par ressort
         link* ll_tmp2 = link_list; int no_repulsion = 1;
         while (ll_tmp2 != NULL)
         {
            if ((ll_tmp2->node_1==i && ll_tmp2->node_2==j) || (ll_tmp2->node_1==j && ll_tmp2->node_2==i))
            {
               no_repulsion = 0;  // les 2 noeuds sont reliés,
               break;             // donc on n'applique pas la force entre eux 
            }
            if (node_tab[i]->graph_number != node_tab[j]->graph_number)
            {
               no_repulsion = 0;  // les 2 noeuds sont dans des graphes non reliés
               break;             // donc on n'applique pas la force entre eux 
            }
            ll_tmp2 = ll_tmp2->next_link;
         }
         if (no_repulsion==0)
           continue;
         
         // si les noeuds sont liées:
         
         // on calcule la distance entre les points
         float delta_x = fabsf(node_tab[i]->pos_x-node_tab[j]->pos_x);
         float delta_y = fabsf(node_tab[i]->pos_y-node_tab[j]->pos_y);
         float dist = sqrtf(delta_x*delta_x + delta_y*delta_y);
         if (dist==0)  // pour ne pas diviser par zéro après
            continue;
            
         // norme de la force en 1/r
         float f = coef_repulsion/dist;
         
         // on calcule les projections sur x et sur y
         float cos_theta = delta_x/dist;
         float sin_theta = delta_y/dist;
         
         if (node_tab[i]->pos_x <= node_tab[j]->pos_x)
         {
            node_tab[i]->force_x -= f*cos_theta;
            node_tab[j]->force_x += f*cos_theta;
         }
         else
         {
            node_tab[i]->force_x += f*cos_theta;
            node_tab[j]->force_x -= f*cos_theta;
         }
         
         if (node_tab[i]->pos_y <= node_tab[j]->pos_y)
         {
            node_tab[i]->force_y -= f*sin_theta;
            node_tab[j]->force_y += f*sin_theta;
         }
         else
         {
            node_tab[i]->force_y += f*sin_theta;
            node_tab[j]->force_y -= f*sin_theta;
         }
         
      }
   }
  
   // FORCE DE FROTTEMENT FLUIDE f = -lambda*v
   temp = node_list;
   for (i=0; i<nbNode; i++)
   {
      // attention: nécéssite de ne pas avoir réinitialisé la vitesse
      temp->force_x += -lambda*temp->vit_x;
      temp->force_y += -lambda*temp->vit_y;
      temp = temp->next_node;
   }
   
   // ACCELERATION  m*a = somme(forces) => a = somme(forces)/m
   temp = node_list;
   for (i=0; i<nbNode; i++)
   {
      temp->acc_x += temp->force_x*(1/m);
      temp->acc_y += temp->force_y*(1/m);
      temp = temp->next_node;
   }
   
   // Pour calculer la vitesse et la position
   // j'utilise la méthode de Newton
   
   // VITESSE  a = dv/dt => dv = a*dt
   temp = node_list;
   for (i=0; i<nbNode; i++)
   {
      temp->vit_x += temp->acc_x*deltaT;
      //temp->vit_x *= lambda2;
      temp->vit_y += temp->acc_y*deltaT;
      //temp->vit_y *= lambda2;
      temp = temp->next_node;
   }
   
   // POSITION  v = dx/dt => dx = v*dt
   temp = node_list;
   for (i=0; i<nbNode; i++)
   {
      temp->pos_x += temp->vit_x*deltaT;
      temp->pos_y += temp->vit_y*deltaT;
      temp = temp->next_node;
   }
   
   // RECENTRAGE DU GRAPHE
   
   // on calcule la moyenne des positions des points
   float moyenne_x = 0; float moyenne_y = 0;
   temp = node_list;   
   for (i=0; i<nbNode; i++)  // on calcule la moyenne
   {
      moyenne_x += temp->pos_x;
      moyenne_y += temp->pos_y;
      temp = temp->next_node;
   }
   moyenne_x /= nbNode;
   moyenne_y /= nbNode;

   // on calcule l'écart par rapport au centre de la fenêtre
   float ecart_x; float ecart_y;
   ecart_x = 320 - moyenne_x;
   ecart_y = 240 - moyenne_y;
   
   temp = node_list;
   for (i=0; i<nbNode; i++)  // on déplace tous les noeuds
   {
      temp->pos_x += ecart_x;
      temp->pos_y += ecart_y;
      temp = temp->next_node;
   } 
   
   // GESTION DE GRAPHE NON RELIES
   // Méthode 1: on ramène les points dans la fenêtre s'ils dépassent
   // (normalement ça ne devrait pas arriver)
   temp = node_list;
   for (i=0; i<nbNode; i++)
   {
      if (temp->pos_x < 0)
         temp->pos_x = 0;
      if (temp->pos_x > 640)
         temp->pos_x = 640;
      if (temp->pos_y < 0)
         temp->pos_y = 0;
      if (temp->pos_y > 480)
         temp->pos_y = 480;
      temp = temp->next_node;
   }
   // Méthode 2: ne pas faire de forces de répulsion 
   // entre les points de deux graphes non reliés
   
   /*
   // raccourcissement de lrepos si des noeuds sont hors écran
   if (update_lrepos == 0)
   {
      temp = node_list;   
      for (i=0; i<nbNode; i++)  // on cherche si un des noeuds est hors écran
      {
         if (temp->pos_x>640 || temp->pos_x<0 || temp->pos_y<0 || temp->pos_y>480)
         {
            //lrepos -= lrepos_minus;
            lrepos -= 1;
            if (lrepos < lrepos_min)
               lrepos = lrepos_min;
            break;
         }
         temp = temp->next_node;
      }
   }
   //printf("%d \n", lrepos);  */
   
   // reset des forces
   temp = node_list;
   for (i=0; i<nbNode; i++)  // on affiche tous les noeuds
   {
      temp->force_x = 0;
      temp->force_y = 0;
      temp->acc_x = 0;
      temp->acc_y = 0;
      /*temp->vit_x = 0;
      temp->vit_y = 0;*/
      temp = temp->next_node;
   }
   
   return lrepos;
}

void affichage(sfRenderWindow *fenetre, int nbNode, node* node_list, node** node_tab, link* link_list)
{
   // préparation des couleurs
   sfColor noir = {0,0,0,0}, blanc = {255,255,255,255}, gris = {128,128,128,128};
   sfColor color1 = {255, 0, 0, 255}, color2 = {0, 255, 0, 255}, color3 = {0, 0, 255, 255};
   
   /*
   // préparation de la ligne
   sfShape *ligne = sfShape_CreateLine(*x,*y,0,0,1,blanc,1,gris);

   // préparation du texte à afficher
   sfString *texte = sfString_Create();
   char s[10];
   sprintf(s,"%d",12);
   sfString_SetText(texte,s);
   sfString_SetPosition(texte,*x +10, *y+10);
   */
   
   
   node* temp = node_list; link* temp2 = link_list; int i=0;
   sfShape* tab_cercle[nbNode];
   while (temp2 != NULL)
   {
      i++;
      temp2 = temp2->next_link;
   }
   int nbLink = i;
   sfShape* tab_ligne[nbLink];
   
   // création des noeuds à afficher
   temp = node_list;
   for (i=0; i<nbNode; i++)
   {
      if(node_tab[i]->graph_number == 2)
         tab_cercle[i] = sfShape_CreateCircle(temp->pos_x,temp->pos_y,10,color2,1,gris);
      else if(node_tab[i]->graph_number == 3)
         tab_cercle[i] = sfShape_CreateCircle(temp->pos_x,temp->pos_y,10,color3,1,gris);
      else if(node_tab[i]->graph_number == 4)
         tab_cercle[i] = sfShape_CreateCircle(temp->pos_x,temp->pos_y,10,color1,1,gris);
      else
         tab_cercle[i] = sfShape_CreateCircle(temp->pos_x,temp->pos_y,10,blanc,1,gris);
      temp = temp->next_node;
   }
   
   // création des liens à afficher
   temp2 = link_list;
   for (i=0; i<nbLink; i++)
   {
      tab_ligne[i] = sfShape_CreateLine(node_tab[temp2->node_1]->pos_x,node_tab[temp2->node_1]->pos_y,node_tab[temp2->node_2]->pos_x,node_tab[temp2->node_2]->pos_y,1,blanc,1,gris);
      temp2 = temp2->next_link;
   }

   sfRenderWindow_Clear(fenetre,noir);  // effacer l'écran 
   for (i=0; i<nbLink; i++)
      sfRenderWindow_DrawShape(fenetre,tab_ligne[i]);   // tracer les lignes
   for (i=0; i<nbNode; i++)
      sfRenderWindow_DrawShape(fenetre,tab_cercle[i]);  // tracer les cercles
   
   //sfRenderWindow_DrawString(fenetre,texte);  // tracer le texte

   sfRenderWindow_Display(fenetre); // visualiser le contenu de la fenetre    

   // faire le ménage en mémoire
   for (i=0; i<nbNode; i++)
      sfShape_Destroy(tab_cercle[i]);
   for (i=0; i<nbLink; i++)
      sfShape_Destroy(tab_ligne[i]);
   //sfString_Destroy(texte);
}



int main()
{
   
   // lecture du nombre de noeuds
   int nbNode;
   int useless = scanf("%d", &nbNode);
   useless++; // useless sert à éviter les warnings de l'autocorrect
   
   // changement des variables en fonction du nombre de noeuds pour un affichage optimal
   int lrepos; int coef_repulsion; int k;
   if(nbNode<30)
   {
      lrepos = 50;
      coef_repulsion = 1000000;
      k = 500;
   }
   else
   {
      lrepos = 10;
      coef_repulsion = 10000;
      k = 1000;
   }
   
   // création de la liste de noeuds (premier rentré = fin de la liste)
   // je crée également un tableau, pour des raisons de praticité
   int i; node* node_list = NULL; node* node_tab[nbNode];
   for (i=0; i<nbNode; i++)
   {
      node* tmp;
      tmp = malloc(sizeof(node));
      node_tab[nbNode -i -1] = tmp;
      tmp->node_number = nbNode -i -1;
      tmp->graph_number = 0;
      tmp->pos_x = rand() % 640;
      tmp->pos_y = rand() % 480;
      tmp->vit_x = 0; tmp->vit_y = 0;
      tmp->acc_x = 0; tmp->acc_y = 0;
      tmp->force_x = 0; tmp->force_y = 0;
      tmp->next_node = node_list;
      node_list = tmp;
   }
   
   // création des liens entre les noeuds
   int tmp1; int tmp2; link* link_list = NULL; int nb_graph = 0;
   while(scanf("%d", &tmp1))
   {
      if ( getchar() != '-' )
         break;
      useless = scanf("%d", &tmp2);
      if (tmp1 == tmp2)
      {
         printf("WARNING: Lien d'un noeud vers lui-même -> lien ignoré.\n");
         continue;
      }
      if (tmp1 >= nbNode || tmp2 >= nbNode)
      {
         printf("WARNING: Lien avec un numéro de noeud trop grand -> lien ignoré.\n");
         continue;
      }
      
      // gestion des graphes non reliés:
      if (node_tab[tmp1]->graph_number != 0)  // le noeud 1 est déjà dans un graphe
         node_tab[tmp2]->graph_number = node_tab[tmp1]->graph_number;
         // on attribue au noeud 2 le même numéro de graphe 
      else if (node_tab[tmp2]->graph_number != 0)  // le noeud 2 est déjà dans un graphe
         node_tab[tmp1]->graph_number = node_tab[tmp2]->graph_number;
         // on attribue au noeud 1 le même numéro de graphe
      else  // les 2 noeuds n'appartiennent à aucun graphe
      {     // donc on en crée un nouveau
         node_tab[tmp1]->graph_number = nb_graph + 1;
         node_tab[tmp2]->graph_number = nb_graph + 1;
         nb_graph++;
      }
      
      // création du lien
      link* tmp;
      tmp = malloc(sizeof(link));
      tmp->node_1 = tmp1;
      tmp->node_2 = tmp2;
      tmp->next_link = link_list;
      link_list = tmp;
   }
   
    
   // initialisation graphique
   sfRenderWindow *fenetre;
   sfVideoMode mode = {640, 480, 32};
   sfWindowSettings settings = {24, 8, 0};
   fenetre = sfRenderWindow_Create(mode, "MARQUET Vincent",  sfResize | sfClose, settings);
   sfEvent evenement;
   
   // initialisation des données utilisé pour la simulation
   float deltaT; // temps entre deux moments discrets de la simulation
   deltaT = 0.01;// doit etre fixe sinon l'algo dépend de vitesse de la machine
                 // d'exécution !! (PB de correction, surtout avec valgrind!) 
                 // Ne modifiez donc pas cette valeur!
                 
   /*int delay = 2; // delai entre deux raccourcissements de ressorts quand des noeuds sont en dehors de l'écran
   int time0 = time(NULL) + delay;  // temps en seconde depuis 1970 */
   //int update_lrepos = 0;

   // Boucle principale
   while (sfRenderWindow_IsOpened(fenetre)) {
     // gestion des evenements
     sfRenderWindow_GetEvent(fenetre, &evenement);
     if (evenement.Type == sfEvtClosed) { // sortie de boucle d'animation
       sfRenderWindow_Close(fenetre);
     }                  
     
     /*if (time(NULL) < time0)  // delay pas encore passé
        update_lrepos = 1;
     else  // delay atteint, dans le prochain tour de simulation on va pouvoir changer la taille des ressorts
     {
        update_lrepos = 0;
        time0 += delay;
     }*/
     //update_lrepos = 0;  // pour test
     
     lrepos = simulation(deltaT, nbNode, node_list, node_tab, link_list, lrepos, k, coef_repulsion);  // calcul de simulation
     affichage(fenetre, nbNode, node_list, node_tab, link_list);  // affichage
   }
   sfRenderWindow_Destroy(fenetre);
   
    
   // désallocation de node_list
   while (node_list != NULL)
   {
      node* tmp = node_list;
      if (tmp->next_node == NULL)
      {
         free(tmp);
         node_list = NULL;
      }
      else
      {
         while ((tmp->next_node)->next_node != NULL)
            tmp = tmp->next_node;
         free(tmp->next_node);
         tmp->next_node = NULL;
      }
   }
   
   // désallocation de link_list
   while (link_list != NULL)
   {
      link* tmp = link_list;
      if (tmp->next_link == NULL)
      {
         free(tmp);
         link_list = NULL;
      }
      else
      {
         while ((tmp->next_link)->next_link != NULL)
            tmp = tmp->next_link;
         free(tmp->next_link);
         tmp->next_link = NULL;
      }
   }
   
   return EXIT_SUCCESS;
}


