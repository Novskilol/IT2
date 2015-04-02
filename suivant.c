Rationnel *getRatRecursif(Rationnel *rat,position p){
  retour=NULL;
  if (rat->position == p)
    return rat;
  if(rat->droit != NULL)
    if((retour=getRatRecursif(rat->droit,p))!=NULL)
      return retour;
  if(rat->gauche != NULL)
    if((retour=getRatRecursif(rat->gauche,p))!=NULL)
      return retour;  
  if (rat->etiquette == LETTRE && rat->position ==p)
    return rat;
}
static void suivantRecursif(Rationnel *rat,Ensemble **e,Rat *init){
  if(rat->position_min == init->position_min)
    if (rat->pere != NULL)
      return suivantRecursif(rat->pere,e,p);
  switch(rat->etiquette)
    {
      
    case STAR:
      /**
       * premier du sous arbre de l'étoile est compris 
       * dans suivant(A) 
       */
      if(rat->gauche != NULL)
	*e=creer_union_ensemble(*e,premier(rat->gauche));
      break;
      
    case CONCAT:
      /**
       * Si A appartient à dernier(gauche)
       * alors fils droit est compris dans suivant(A)
       */
      if(est_dans_l_ensemble(*e,init->position_min) && rat->droit != NULL)
	*e=creer_union_ensemble(*e,premier(rat->droit));
      break;
      
    default :
      break;
     
  
    }
  if(rat->pere != NULL)
    return suivantRecursif(rat->pere,e,p);
  
      
    
  


}
