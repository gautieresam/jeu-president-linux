//include
#include "gestion_jeu.h"


/**
 * \fn void Initalisation_du_jeu_de_carte(int tab[52])
 * \brief Fonction d'initailisation du jeu. 1=> as 2=>2 11=>valet 12=>dame 13=>roi
 *
 * \param tab le tableau à remplir avec les cartes.
 * \return passage par adresse donc pas de return 
 */
void initalisation_du_jeu_de_carte(int tab[TAILLE_JEU_DE_CARTE]){
   // Declaration des variables 
   int emplacement=1; 

   for (int i = 0; i <TAILLE_JEU_DE_CARTE-3; i=i+4 ){
      for (int k = 0; k <=3; k++){
         tab[i+k]=emplacement;
      }
      emplacement++;
   }
}

/**
 * \fn void afficher_le_jeu_de_carte(int tab[52]){
 * \brief Fonction d'affichage du jeu de carte.
 *
 * \param tab le tableau à remplir avec les cartes.
 * \return pas de return 
 */
void afficher_tab(int jeu_de_carte[TAILLE_JEU_DE_CARTE]){

   printf("FONCTION : AFFICHER TAB :\n");
   for (int i = 0; i < TAILLE_JEU_DE_CARTE; i++){
      //printf("%d %d - \n",i,jeu_de_carte[i]);
      printf("%d - ",jeu_de_carte[i]);
   }
}

/**
 * \fn melanger_cartes(jeu_de_carte)
 * \brief Fonction qui va mélanger le jeu de carte (tableau)
 * \
 * \param tab le tableau à melanger.
 * \return pas de return
*/
void melanger_cartes(int jeu_de_carte[TAILLE_JEU_DE_CARTE]){
    int temp, alea;
    //mélanger le jeu
    for (int i = 0; i < TAILLE_JEU_DE_CARTE; i++){
      //echanger la case d'indice i avec une case random du tableau
      temp=jeu_de_carte[i];
      alea=rand() % (TAILLE_JEU_DE_CARTE);
      jeu_de_carte[i]=jeu_de_carte[alea];
      jeu_de_carte[alea]=temp;
   }
}

int give_taille_de_la_main(){
    int taille = TAILLE_JEU_DE_CARTE/NB_JOUEURS;
    return taille;
}

void afficher_carte_joueur(int numJoueur, int jeu_de_carte[TAILLE_JEU_DE_CARTE]){
    int taille_main = give_taille_de_la_main();
    int debut=taille_main*numJoueur;
    int fin=debut+taille_main;

    printf("\nJeu du joueur %d : ",numJoueur);
    for (int i = debut; i < fin; i++){
        printf("%d - ",jeu_de_carte[i]);
    }

}

int indice_partie(int partie[TAILLE_JEU_DE_CARTE]){
	int flag=0, i=0;
	while(flag==0 && i<(TAILLE_JEU_DE_CARTE-1)){
			if(partie[i]==0){
				flag=1;
				return i;
			}
			i++;
		}
	return -1;
}

//return = indice, si -1 ==> carte pas dans le jeu
int jouer_une_carte(int numJoueur, int carte, int jeu_de_carte[(TAILLE_JEU_DE_CARTE)], int partie[(TAILLE_JEU_DE_CARTE)]){
	int indice=-1;
	//vérifier que la carte est dans son jeu + recupérer l'indice
	int taille_main = give_taille_de_la_main();
	int debut=taille_main*numJoueur;
	int fin=debut+taille_main;
	
	int i=debut, flag=0;
	while(flag==0 && i< fin){
		//printf("compare %d avec %d",jeu_de_carte[i], carte);
		if(jeu_de_carte[i]==carte){
			flag=1;
			indice=i;
		}
		i++;
	}
	
	if(flag==1){
		//ajout la carte dans la partie (recupérer indice de jeu)
		int indiceP;
		indiceP=indice_partie(partie);
		if (indiceP >= TAILLE_JEU_DE_CARTE){
			return 0;
		}
		partie[indiceP]=carte;
		//mettre la carte à 0 dans le tab du jeu de carte
		jeu_de_carte[indice]=0;
		return 1;
	}
	return 0;

}
