#include "gestion_jeu.h"

/**
 * \fn void * createSharedMemory(int *cle){
 * \brief Cette fonction permet de crééer une memoire partagée.
 * \param int *cle qui est la clé de mon espace de memoire partagée
* **/
void createSharedMemory(int cle){
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
void * getSharedMemory(int cle){

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
void detachSharedMemory(struct data_t *memoryShared){
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
void deleteSharedMemory(int cle){
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


/**
 * \fn void Initalisation_du_jeu_de_carte(int tab[52])
 * \brief Fonction d'initailisation du jeu. 1=> as 2=>2 11=>valet 12=>dame 13=>roi
 *
 * \param tab le tableau à remplir avec les cartes.
 * \return passage par adresse donc pas de return 
 */
void initalisation_du_jeu_de_carte(int tab[TAILLE_JEU_DE_CARTE],int partie[TAILLE_JEU_DE_CARTE]) {
    // Declaration des variables
    int emplacement = 1;

    for (int i = 0; i < TAILLE_JEU_DE_CARTE - 3; i = i + 4) {
        for (int k = 0; k <= 3; k++) {
            tab[i + k] = emplacement;
        }
        emplacement++;
    }

    for (int i = 0; i < TAILLE_JEU_DE_CARTE; i++) {
        partie[i] = -1;
        printf("data %d\n",partie[i]);
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


