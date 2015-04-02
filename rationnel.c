/*
 *   Ce fichier fait partie d'un projet de programmation donné en Licence 3 
 *   à l'Université de Bordeaux.
 *
 *   Copyright (C) 2015 Giuliana Bianchi, Adrien Boussicault, Thomas Place, Marc Zeitoun
 *
 *    This Library is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This Library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this Library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "rationnel.h"
#include "ensemble.h"
#include "automate.h"
#include "parse.h"
#include "scan.h"
#include "outils.h"

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int yyparse(Rationnel **rationnel, yyscan_t scanner);

Rationnel *rationnel(Noeud etiquette, char lettre, int position_min, int position_max, void *data, Rationnel *gauche, Rationnel *droit, Rationnel *pere)
{
   Rationnel *rat;
   rat = (Rationnel *) malloc(sizeof(Rationnel));

   rat->etiquette = etiquette;
   rat->lettre = lettre;
   rat->position_min = position_min;
   rat->position_max = position_max;
   rat->data = data;
   rat->gauche = gauche;
   rat->droit = droit;
   rat->pere = pere;
   return rat;
}

Rationnel *Epsilon()
{
   return rationnel(EPSILON, 0, 0, 0, NULL, NULL, NULL, NULL);
}

Rationnel *Lettre(char l)
{
   return rationnel(LETTRE, l, 0, 0, NULL, NULL, NULL, NULL);
}

Rationnel *Union(Rationnel* rat1, Rationnel* rat2)
{
   // Cas particulier où rat1 est vide
   if (!rat1)
      return rat2;

   // Cas particulier où rat2 est vide
   if (!rat2)
      return rat1;
   
   return rationnel(UNION, 0, 0, 0, NULL, rat1, rat2, NULL);
}

Rationnel *Concat(Rationnel* rat1, Rationnel* rat2)
{
   if (!rat1 || !rat2)
      return NULL;

   if (get_etiquette(rat1) == EPSILON)
      return rat2;

   if (get_etiquette(rat2) == EPSILON)
      return rat1;
   
   return rationnel(CONCAT, 0, 0, 0, NULL, rat1, rat2, NULL);
}

Rationnel *Star(Rationnel* rat)
{
   return rationnel(STAR, 0, 0, 0, NULL, rat, NULL, NULL);
}

bool est_racine(Rationnel* rat)
{
   return (rat->pere == NULL);
}

Noeud get_etiquette(Rationnel* rat)
{
   return rat->etiquette;
}

char get_lettre(Rationnel* rat)
{
   assert (get_etiquette(rat) == LETTRE);
   return rat->lettre;
}

int get_position_min(Rationnel* rat)
{
   assert (get_etiquette(rat) == LETTRE);
   return rat->position_min;
}

int get_position_max(Rationnel* rat)
{
   assert (get_etiquette(rat) == LETTRE);
   return rat->position_max;
}

void set_position_min(Rationnel* rat, int valeur)
{
   assert (get_etiquette(rat) == LETTRE);
   rat->position_min = valeur;
   return;
}

void set_position_max(Rationnel* rat, int valeur)
{
   assert (get_etiquette(rat) == LETTRE);
   rat->position_max = valeur;
   return;
}

Rationnel *fils_gauche(Rationnel* rat)
{
   assert((get_etiquette(rat) == CONCAT) || (get_etiquette(rat) == UNION));
   return rat->gauche;
}

Rationnel *fils_droit(Rationnel* rat)
{
   assert((get_etiquette(rat) == CONCAT) || (get_etiquette(rat) == UNION));
   return rat->droit;
}

Rationnel *fils(Rationnel* rat)
{
   assert(get_etiquette(rat) == STAR);
   return rat->gauche;
}

Rationnel *pere(Rationnel* rat)
{
   assert(!est_racine(rat));
   return rat->pere;
}

void print_rationnel(Rationnel* rat)
{
   if (rat == NULL)
   {
      printf("∅");
      return;
   }
   
   switch(get_etiquette(rat))
   {
      case EPSILON:
         printf("ε");         
         break;
         
      case LETTRE:
         printf("%c", get_lettre(rat));
         break;

      case UNION:
         printf("(");
         print_rationnel(fils_gauche(rat));
         printf(" + ");
         print_rationnel(fils_droit(rat));
         printf(")");         
         break;

      case CONCAT:
         printf("[");
         print_rationnel(fils_gauche(rat));
         printf(" . ");
         print_rationnel(fils_droit(rat));
         printf("]");         
         break;

      case STAR:
         printf("{");
         print_rationnel(fils(rat));
         printf("}*");         
         break;

      default:
         assert(false);
         break;
   }
}

Rationnel *expression_to_rationnel(const char *expr)
{
    Rationnel *rat;
    yyscan_t scanner;
    YY_BUFFER_STATE state;

    // Initialisation du scanner
    if (yylex_init(&scanner))
        return NULL;
 
    state = yy_scan_string(expr, scanner);

    // Test si parsing ok.
    if (yyparse(&rat, scanner)) 
        return NULL;
    
    // Libération mémoire
    yy_delete_buffer(state, scanner);
 
    yylex_destroy(scanner);
 
    return rat;
}

void rationnel_to_dot(Rationnel *rat, char* nom_fichier)
{
   FILE *fp = fopen(nom_fichier, "w+");
   rationnel_to_dot_aux(rat, fp, -1, 1);
}

int rationnel_to_dot_aux(Rationnel *rat, FILE *output, int pere, int noeud_courant)
{   
   int saved_pere = noeud_courant;

   if (pere >= 1)
      fprintf(output, "\tnode%d -> node%d;\n", pere, noeud_courant);
   else
      fprintf(output, "digraph G{\n");
   
   switch(get_etiquette(rat))
   {
      case LETTRE:
         fprintf(output, "\tnode%d [label = \"%c-%d\"];\n", noeud_courant, get_lettre(rat), rat->position_min);
         noeud_courant++;
         break;

      case EPSILON:
         fprintf(output, "\tnode%d [label = \"ε-%d\"];\n", noeud_courant, rat->position_min);
         noeud_courant++;
         break;

      case UNION:
         fprintf(output, "\tnode%d [label = \"+ (%d/%d)\"];\n", noeud_courant, rat->position_min, rat->position_max);
         noeud_courant = rationnel_to_dot_aux(fils_gauche(rat), output, noeud_courant, noeud_courant+1);
         noeud_courant = rationnel_to_dot_aux(fils_droit(rat), output, saved_pere, noeud_courant+1);
         break;

      case CONCAT:
         fprintf(output, "\tnode%d [label = \". (%d/%d)\"];\n", noeud_courant, rat->position_min, rat->position_max);
         noeud_courant = rationnel_to_dot_aux(fils_gauche(rat), output, noeud_courant, noeud_courant+1);
         noeud_courant = rationnel_to_dot_aux(fils_droit(rat), output, saved_pere, noeud_courant+1);
         break;

      case STAR:
         fprintf(output, "\tnode%d [label = \"* (%d/%d)\"];\n", noeud_courant, rat->position_min, rat->position_max);
         noeud_courant = rationnel_to_dot_aux(fils(rat), output, noeud_courant, noeud_courant+1);
         break;
         
      default:
         assert(false);
         break;
   }
   if (pere < 0)
      fprintf(output, "}\n");
   return noeud_courant;
}
/*
static int max(int x, int y)
{
  return x>y ? x : y;
}
static int min(int x, int y)
{
  return x>y ? y : x;
  }*/

int numeroter_rationnel_aux(Rationnel *rat,int i)
{
  /*on utilise i pour connaitre le nombre de lettres visitées
    on propage les min et max recursivement*/
   switch(rat->etiquette)
   {
      case LETTRE:
	i++;
	rat->position_min=i;
	rat->position_max=i;
	return i;
         break;

      case EPSILON:
	rat->position_min=0;
	rat->position_max=0;
	return 0;
         break;

      case UNION:
	i=numeroter_rationnel_aux(rat->gauche,i);
	i=numeroter_rationnel_aux(rat->droit,i);
	rat->position_min=rat->gauche->position_min;
	rat->position_max=rat->droit->position_max;
	return i;
	break;

      case CONCAT:
        i=numeroter_rationnel_aux(rat->gauche,i);
	i=numeroter_rationnel_aux(rat->droit,i);
	rat->position_min=rat->gauche->position_min;
	rat->position_max=rat->droit->position_max;
	return i;
         break;

      case STAR:
	i=numeroter_rationnel_aux(rat->gauche,i);
	rat->position_min=rat->gauche->position_min;
	rat->position_max=rat->gauche->position_max;
	return i;
        break;
         
      default:
         assert(false);
         break;
   }
   return 0;
}

void numeroter_rationnel(Rationnel *rat)
{
  /* On parcours le rationnel de manière préfixe en numérotant
     les états qui sont des lettres */
  int i=0;
  numeroter_rationnel_aux(rat,i);
}

bool contient_mot_vide(Rationnel *rat)
{
  bool contientEpsilonFilsGauche=false;
  bool contientEpsilonFilsDroit=false;
  /**
   * On test d'abord pour le noeud courant .
   * Le mot vide peut être présent directement ou bien il y a une étoile
   */
  if (rat->etiquette ==EPSILON || rat->etiquette  == STAR)
    return true;
  /**
  * Si le noeud courant ne contient pas epsilon , on le test récursivement
  * sur les fils gauche et droit .
  */
  if(rat->gauche!=NULL)
    contientEpsilonFilsGauche=contient_mot_vide(rat->gauche);
   
  if(rat->droit!=NULL)
    contientEpsilonFilsDroit=contient_mot_vide(rat->droit);
  
  return contientEpsilonFilsDroit||contientEpsilonFilsGauche;
}

/**
 * Cette fonction cherche récursivement les premier 
 * ATTENTION , la valeur renvoyée par la fonction ne correspond
 * pas au résultat mais sert juste à la récursion.
 * Le résultat se retrouve dans Ensemble *e 
 */
static bool premierRecursif(Rationnel *rat,Ensemble *e)
{
  bool estTrouvePremier=false;
  /**
   * Si le premier terme est une lettre on l'ajoute à l'ensemble et
   * l'on renvoie que le premier a été trouvé
   */
  if(rat->etiquette == LETTRE){
    ajouter_element(e,rat->position_min);
    return true;
  }
  /**
   * Autrement on cherche récursivement sur le fils gauche le premier.
   * Si il n'est pas trouvé ou bien que l'étiquette père est une étoile alors
   * on cherche récursivement sur le fils droit . 
   */
  else 
    {
      /**
       * estTrouverPremier peut être faux si l'expression gauche contient une 
       * étoile avant chaque feuille ou bien si l'expression n'est pas correcte
       */
      if(rat->gauche!=NULL)
	estTrouvePremier=premierRecursif(rat->gauche,e);
      
      /**
       * Dans le cas ou l'expression gauche contient une étoile ou bien 
       * que l'étiquette courante est une étoile on regarde le fils droit .
       */
      if(!estTrouvePremier || rat->etiquette == STAR || rat->etiquette == UNION)
	if (rat->droit != NULL)
	  estTrouvePremier=premierRecursif(rat->droit,e);
      
      /**
       * Si l'étiquette courante est une étoile , on renvoie 
       * que l'élément est encore à chercher . 
       */
      if (rat->etiquette == STAR)
	estTrouvePremier=false;
    }
  return estTrouvePremier;

}
Ensemble *premier(Rationnel *rat)
{
  /**
   * Attention l'ensemble renvoyé par cette fonction 
   * devra être désalloué (vider_ensemble(Ensemble*))
   */
  Ensemble *e=creer_ensemble(NULL,NULL,NULL);
  premierRecursif(rat,e);
  return e;

}

/* static bool dernierRecursif(Rationnel *rat,Ensemble *e) */
/* { */
/*   bool estTrouvePremier=false; */
/*   /\** */
/*    * Si le premier terme est une lettre on l'ajoute à l'ensemble et */
/*    * l'on renvoie que le premier a été trouvé */
/*    *\/ */
/*   if(rat->etiquette == LETTRE){ */
/*     ajouter_element(e,rat->position_min); // XXX rat->lettre ou &rat->lettre */
/*     return true; */
/*   } */
/*   /\** */
/*    * Autrement on cherche récursivement sur le fils gauche le premier. */
/*    * Si il n'est pas trouvé ou bien que l'étiquette père est une étoile alors */
/*    * on cherche récursivement sur le fils droit .  */
/*    *\/ */
/*   else  */
/*     { */
/*       /\** */
/*        * estTrouverPremier peut être faux si l'expression gauche contient une  */
/*        * étoile avant chaque feuille ou bien si l'expression n'est pas correcte */
/*        *\/ */
/*       if(rat->droit!=NULL) */
/* 	estTrouvePremier=premierRecursif(rat->gauche,e); */
      
/*       /\** */
/*        * Dans le cas ou l'expression gauche contient une étoile ou bien  */
/*        * que l'étiquette courante est une étoile on regarde le fils droit . */
/*        *\/ */
/*       if(!estTrouvePremier || rat->etiquette == STAR || rat->etiquette == UNION) */
/* 	if (rat->droit != NULL) */
/* 	  estTrouvePremier=premierRecursif(rat->droit,e); */
      
/*       /\** */
/*        * Si l'étiquette courante est une étoile , on renvoie  */
/*        * que l'élément est encore à chercher .  */
/*        *\/ */
/*       if (rat->etiquette == STAR) */
/* 	estTrouvePremier=false; */
/*     } */
/*   return estTrouvePremier; */
/* } */

Ensemble *dernier(Rationnel *rat)
{
  /*Ensemble *e=creer_ensemble(NULL,NULL,NULL);
  dernierRecursif(rat,e);
  return e;*/
  A_FAIRE_RETURN(NULL);
}

Ensemble *suivant(Rationnel *rat, int position)
{
   A_FAIRE_RETURN(NULL);
}

Automate *Glushkov(Rationnel *rat)
{
  /* on numérote le rationnel puis on le transforme 
     en automate de glushkov*/
  numeroter_rationnel(rat);
  Automate *ret=creer_automate();
  if (rat->lettre!='\0')
    ajouter_etat(ret,rat->position_min);
  A_FAIRE_RETURN(NULL);
}

bool meme_langage (const char *expr1, const char* expr2)
{
  //
  //rationnel *r1=expression_to_rationnel(expr1);
  //rationnel *r2=expression_to_rationnel(expr2);
   A_FAIRE_RETURN(true);
}

Systeme systeme(Automate *automate)
{
   A_FAIRE_RETURN(NULL);
}

void print_ligne(Rationnel **ligne, int n)
{
   for (int j = 0; j <=n; j++)
      {
         print_rationnel(ligne[j]);
         if (j<n)
            printf("X%d\t+\t", j);
      }
   printf("\n");
}

void print_systeme(Systeme systeme, int n)
{
   for (int i = 0; i <= n-1; i++)
   {
      printf("X%d\t= ", i);
      print_ligne(systeme[i], n);
   }
}

Rationnel **resoudre_variable_arden(Rationnel **ligne, int numero_variable, int n)
{
   A_FAIRE_RETURN(NULL);
}

Rationnel **substituer_variable(Rationnel **ligne, int numero_variable, Rationnel **valeur_variable, int n)
{
   A_FAIRE_RETURN(NULL);
}

Systeme resoudre_systeme(Systeme systeme, int n)
{
   A_FAIRE_RETURN(NULL);
}

Rationnel *Arden(Automate *automate)
{
   A_FAIRE_RETURN(NULL);
}

