/*   à l'Université de Bordeaux
 *
 *   Copyright (C) 2015 Adrien Boussicault
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
#include "outils.h"
#include "fifo.h"

#include <signal.h>
#include <errno.h>

int test_arden(){

	int result = 1;
	
	{
	        Rationnel * expression2 = expression_to_rationnel( "(a*+b)" );

		Automate * automate2 = Glushkov( expression2 );
		print_automate(automate2);

		const Automate *m2=creer_automate_minimal(automate2);
		printf("automate m2: ");
		print_automate(m2);

		Systeme systemed=systeme(automate2);
		int nbLigne=taille_ensemble(get_etats(automate2));
		print_systeme(systemed,nbLigne);
		
		/*resoudre_variable_arden(systemed[1],1,nbLigne);
		substituer_variable(systemed[0], 1,systemed[1] , nbLigne);
		substituer_variable(systemed[0], 2,systemed[2] , nbLigne);
		resoudre_systeme(systemed,nbLigne);*/
		
		resoudre_systeme(systemed,nbLigne);

		
		print_systeme(systemed,nbLigne);

		Rationnel *ret=systemed[0][nbLigne];

		Automate *retaut=Glushkov(ret);
		const Automate *m1=creer_automate_minimal(retaut);
		printf("automate m1: ");
		print_automate(m1);
		TEST(
		     1
		     , result
		     );

		liberer_automate( automate2 );
	}	
return result;
}


int main(){

	if( ! test_arden() ){ return 1; }

	return 0;
}
