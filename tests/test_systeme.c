#include <stdlib.h>
#include <stdio.h>
#include <automate.h>
#include <rationnel.h>
#include <ensemble.h>
#include <outils.h>
#include <parse.h>
#include <scan.h>

int test_systeme(){
	int result = 1;
    {
      Automate* automate = Glushkov(expression_to_rationnel("a.b"));
      print_automate(automate);
      Systeme s=systeme(automate);
      print_systeme(s,taille_ensemble(get_etats(automate)));
      
    }

    return result;
}

int main(int argc, char *argv[])
{
   if( ! test_systeme() )
    return 1; 
   
   return 0;
}

