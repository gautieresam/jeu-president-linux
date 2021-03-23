#include "gestion_jeu.h"

//declaration variables
pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int jeu_de_carte[(TAILLE_JEU_DE_CARTE)]={0};  
int partie[(TAILLE_JEU_DE_CARTE)]={0};
int joueurs[2][NB_JOUEURS]={0};

int main(){
	
	//int jeu_de_carte[(TAILLE_JEU_DE_CARTE)]={0};  
	
	//TEST des fonctions du jeu de cartes
	initalisation_du_jeu_de_carte(jeu_de_carte);
	//afficher_tab(jeu_de_carte);
	melanger_cartes(jeu_de_carte);
	//afficher_tab(jeu_de_carte);

	int taille_main = give_taille_de_la_main();
	//printf("Taille de la main : %d\n",taille_main);
	
	//afficher_carte_joueur(0,jeu_de_carte);
 	//afficher_carte_joueur(1,jeu_de_carte);

 	remplir_tab_joueurs(joueurs);
 	afficher_tab_joueurs(joueurs);
 	
 	//TEST des mutex pour partages ressources inter-processus
 	pthread_t j1, j2;
 	//void * ret;
 	pthread_create(&j1, NULL, joueur, (int *) 1);
 	//pthread_create(&j2, NULL, joueur, (int *) 2);
 	
 	pthread_join(j1, NULL);
    	//pthread_join(j2, NULL);

	return 0;
}

//Fonction de jeu du joueur
void *joueur(void *arg){

	int numJ = (int) arg;

    for (int i=0; i<10;i++){

        printf("\n\nJe suis le joueur num %d\n",numJ);
	printf("Voici mon jeu : ");
	//mise d'un mutex car utilisation d'une ressource exclusive (le jeu de carte)
	pthread_mutex_lock(&my_mutex);
		afficher_carte_joueur(numJ,jeu_de_carte);
	pthread_mutex_unlock(&my_mutex);
	
	//test jouer une carte : mettre dans un mutex quand utilisation de la ressource
	int carte;
        printf("Veuillez saisir une carte : ");
        scanf("%d", &carte);
        printf("\nessai de jouer la carte %d\n", carte);
        pthread_mutex_lock(&my_mutex);
        //jouer une carte
        int test = jouer_une_carte(numJ, carte, jeu_de_carte, partie);
        printf("resultat (1 si la carte a été joué ou passe son tour, sinon redemander la carte= %d", test);
        pthread_mutex_unlock(&my_mutex);

        pthread_mutex_lock(&my_mutex);
        //afficher le jeu du joueur
        afficher_carte_joueur(numJ, jeu_de_carte);
        //afficher le tableau de la partie
        afficher_tab(partie);
        //la dernière carte jouée
        printf("\nDernière carte jouée : %d\n", get_derniere_carte(partie));
        pthread_mutex_unlock(&my_mutex);
    }

	
	pthread_exit(0);
}
