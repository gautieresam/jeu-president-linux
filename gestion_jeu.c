#include "gestion_jeu.h"


/***************************************************************************************
 * Partie Serveur Gautier
 ***************************************************************************************/

/**
 * \fn void * createSharedMemory(int *cle){
 * \brief Cette fonction permet de crééer une memoire partagée.
 * \param int *cle qui est la clé de mon espace de memoire partagée
* **/
void * createSharedMemory(int cle){

    int shmid;

    if((shmid = shmget((key_t)cle,sizeof(struct data_t) * 1, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        if(errno == EEXIST)
            fprintf(stderr, "Le segment de memoire partagee (cle=%d) existe deja\n", CLE);
        else
            perror("Erreur lors de la creation du segment de memoire ");
        exit(EXIT_FAILURE);
    }
    printf("Serveur : segment cree.\n");
}


/**
 * \fn void *getSharedMemory(int *entier){
 * \brief Cette fonction est déclenché par un thread conditionnel.
 * \param int *cle qui est la clé de mon espace de memoire partagée
* **/
void * getSharedMemory(int *cle){

    int shmid;
    struct data_t *memoryShared;

    if((shmid = shmget((key_t)cle, sizeof(struct data_t) * 1, 0)) == -1) {
        perror("Erreur lors de la recuperation du segment de memoire partagee\n");
        exit(EXIT_FAILURE);
    }
    if((memoryShared = shmat(shmid, NULL, 0)) == (void*)-1) {
        perror("Erreur lors de l'attachement du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }
    return memoryShared;
}

/**
 * \fn void * detachSharedMemory(struct data_t *memoryShared)
 * \brief Cette fonction est permet de detacher une memoire partagée.
 * \param struct data_t *memoryShared la structure à détacher
* **/
void * detachSharedMemory(struct data_t *memoryShared){
    if(shmdt(memoryShared) == -1) {
        perror("Erreur lors du detachement du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }
}

/**
 * \fn void *deleteSharedMemory(int *cle)
 * \brief Cette fonction permet de supprimer une memoire partagée.
 * \param int *cle la clé de la memoire partagée
* **/
void *deleteSharedMemory(int *cle){
    int shmid;
    /* Recuperation du segment de memoire partagee */
    if((shmid = shmget((key_t)cle, 0, 0)) == -1) {
        perror("Erreur lors de la recuperation du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }
    /* Suppression du segment de memoire partagee */
    if(shmctl(shmid, IPC_RMID, 0) == -1) {
        perror("Erreur lors de la suppression du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }
}








/***************************************************************************************
 * Partie Client Mariette
 ***************************************************************************************/

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

   printf("\nFONCTION : AFFICHER TAB du jeu :\n");
   for (int i = 0; i < TAILLE_JEU_DE_CARTE; i++){
       if (jeu_de_carte[i]==11){
           printf("Valet - ");
       } else if (jeu_de_carte[i]== 12){
           printf("Dame - ");
       }else if (jeu_de_carte[i]== 13){
           printf("Roi - ");
       }else{
           printf("%d - ",jeu_de_carte[i]);
       }
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
    srand(time(NULL));
    //mélanger le jeu
    for (int i = 0; i < TAILLE_JEU_DE_CARTE; i++){
      //echanger la case d'indice i avec une case random du tableau
      temp=jeu_de_carte[i];
      alea=rand() % (TAILLE_JEU_DE_CARTE);
      jeu_de_carte[i]=jeu_de_carte[alea];
      jeu_de_carte[alea]=temp;
   }
}

void test(){
    printf("function");
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
        if (jeu_de_carte[i]==11){
            printf("Valet - ");
        } else if (jeu_de_carte[i]== 12){
            printf("Dame - ");
        }else if (jeu_de_carte[i]== 13){
            printf("Roi - ");
        }else if (jeu_de_carte[i]!=0){
            //n'affiche pas les 0 (car signifie qu'il n'y a pas de carte
            printf("%d - ",jeu_de_carte[i]);
        }
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

/*return = indice, si 0 ==> carte pas dans le jeu
si 1 ==> carte jouée ou passe son tour
 si 2 ==> carte pas possible detre jouée (ordre),
 si 3==> il fautnjouer une carte pareille
 */
int jouer_une_carte(int numJoueur, int carte, int jeu_de_carte[(TAILLE_JEU_DE_CARTE)], int partie[(TAILLE_JEU_DE_CARTE)]){

    //si la carte est 0 <=> le joueur choisit de passer son tour
    if (carte==0){
        //le joueur passe son tour
        printf("==> Joueur %d passe son tour",numJoueur);
        //appeler la fonction qui permet de passer son tour
        return 1;
    }

    int indice=-1;
	//vérifier que la carte est dans son jeu + recupérer l'indice
	int taille_main = give_taille_de_la_main();
	int debut=taille_main*numJoueur;
	int fin=debut+taille_main;
    int i=debut, flag=0;
    int last_card;
    int carte_ok=0;
    int indiceP=indice_partie(partie);

    //Verifier que la  carte existe bien dans le jeu
	//printf("DÉBUT : %d, FIN : %d",debut,fin);
	while(flag==0 && i< fin){
		//printf("compare %d avec %d",jeu_de_carte[i], carte);
		if(jeu_de_carte[i]==carte){
			flag=1;
			indice=i;
		}
		i++;
	}

	//verifier si la carte est bien jouée :
	//1. supérieur ou égal à la carte d'avant (attention à l'as et au 2)
    last_card=get_derniere_carte(partie);
    if (last_card==0){
        //première carte à poser
        carte_ok=1;
    }else if (last_card > 2 && carte > 2){
        //cas classique, mettre une carte suppérieur ou egale (numériquement parlant)
        if (carte >= last_card){
            carte_ok=1;
        } else{
            printf("\n==>Veuillez jouer une carte supérieure ou égale à la dernière du jeu\n");
            return 2;//pas odre du jeu
        }
    }else if (last_card > 2 && carte <= 2){
        //le joueur veut jouer un as ou un 2 au dessus d'une carte entre le 3 et le roi OK
        carte_ok=1;
    }else if (last_card <= 2 && carte > 2){
        //le joueur veut jouer une carte au dessus d'un as ou un 2 PAS POSSIBLE
        printf("\n==>Veuillez jouer une carte supérieure ou égale à la dernière du jeu\n");
        return 2;//pas odre du jeu
    }else if (last_card <= 2 && carte <= 2){
        //le joueur veut jouer un as ou un 2 au dessus d'un as ou un 2
        if (last_card<=carte){
            carte_ok=1;
        }else{
            printf("\n==>Veuillez jouer une carte supérieure ou égale à la dernière du jeu\n");
            return 2;//pas odre du jeu
        }
    }

	//2. poser obligatoirement la meme carte
	//analyser les 3 dernières cartes
    if (indiceP>1 && partie[indiceP-1]==last_card){
        //2 dernière carte pareille = jouer une carte pareille
        if (carte==last_card){
            carte_ok=1;
        }else{
            printf("\n==>Veuillez poser obligatoirement une carte égale à la dernière carte posée\n");
            return 3;//il faut jouer une carte pareil ou passer son tour
        }
    }


	//jouer la carte
	if(flag==1 && carte_ok==1){
		//ajout la carte dans la partie (recupérer indice de jeu)
		if (indiceP >= TAILLE_JEU_DE_CARTE){
            printf("\n==>La carte saisie n'existe pas dans votre jeu\n");
            return 0;
		}
		partie[indiceP]=carte;
		//mettre la carte à 0 dans le tab du jeu de carte
		jeu_de_carte[indice]=0;
		return 1;
	}
    printf("\n==>La carte saisie n'existe pas dans votre jeu\n");
    return 0;

}


int get_derniere_carte(int jeu_de_carte[TAILLE_JEU_DE_CARTE]){
    int i=0;

    while (jeu_de_carte[i]!=0){
        i++;
    }
        return jeu_de_carte[i-1];
}

void remplir_tab_joueurs(int joueurs[][NB_JOUEURS]){
    for (int i = 0; i < NB_JOUEURS; i++) {
        joueurs[1][i]=i+1;
        joueurs[2][i]=1;//actif par défaut
    }
}

void afficher_tab_joueurs(int joueurs[2][NB_JOUEURS]){
    printf("\nJOUEURS :\n");
    for (int i = 0; i < NB_JOUEURS; i++){
            printf("ordre : %d | NumJ : %d | Actif : %d \n",i,joueurs[1][i],joueurs[2][i]);
        }
}

void set_joueur_inactif(int numJoueur,int joueurs[2][NB_JOUEURS]){
   int i=0;
    while (joueurs[1][i]!=numJoueur){
        i++;
    }
    joueurs[2][i]=0;
}

//actif ou "couché"
int get_status_joueurs(int numJoueur,int joueurs[2][NB_JOUEURS]){
    int i=0;
    while (joueurs[1][i]!=numJoueur){
        i++;
    }
    return joueurs[2][i];
}

void set_all_joueurs_actifs(int joueurs[2][NB_JOUEURS]){
    for (int i = 0; i < NB_JOUEURS; i++) {
        joueurs[2][i]=1;//actif par défaut
    }
}

int combien_joueurs_actifs(int joueurs[2][NB_JOUEURS]){
    int count=0;
    for (int i = 0; i < NB_JOUEURS; i++) {
        if (joueurs[2][i]==1){
            count++;
        }
    }
    return count;
}









//TODO : gérer les série : quand un "2" est posé, quand carré ou quand tout le monde est couché => dernièrec arte posé commence