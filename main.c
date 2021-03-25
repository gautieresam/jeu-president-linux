#include "gestion_jeu.h"

/**
 * \fn pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;
 * \brief Initialisation d'un thread mutex.
 */
pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;


/**
 * \fn pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
 * \brief Initialisation d'un thread conditionnel.
 */
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


/**
 * \fn Sturure qui contient les informations de la partie.
 * \brief
 *
 * \param tableau jeu_de_carte, contient le jeu de carte mélangé.
 * \param entier start, variable pour les nouveux joueurs si ils peuvent jouer à la partie ou non.
 * \param tableau partie, contient les cartes posés par les joueurs.
 * \param entier nbCurrentUser,variable pour connaitre le nombre de joueur dans la partie.
 */
struct data_t{

    int jeu_de_carte[(TAILLE_JEU_DE_CARTE)];
    int partie[(TAILLE_JEU_DE_CARTE)];
    int joueurs[2][NB_JOUEURS];
    int start;
    int nbCurrentUser;

} ;

/**
 * \fn sem_t *monSemaphore;
 * \brief Initialisation du premier semaphore
 */
sem_t *monSemaphore;


int main(){

    // Structure qui va permettre de stocker les informations de notre partie.
    struct data_t data;

    // Initialiser les variables de data
    data.start=0;
    data.nbCurrentUser=0;
    printf("INFO : La partie est-elle demaré [0/1] : %d\n",data.start);
	initalisation_du_jeu_de_carte(data.jeu_de_carte);
	afficher_tab(data.jeu_de_carte);
	melanger_cartes(data.jeu_de_carte);
	afficher_tab(data.jeu_de_carte);

    monSemaphore=sem_open("/data.SEMAPHORE",O_CREAT | O_RDWR,0600,2); // 2 nb de processus en meme temps


    int taille_main = give_taille_de_la_main();
	printf("Taille de la main : %d\n",taille_main);
	
	afficher_carte_joueur(0,data.jeu_de_carte);

	afficher_carte_joueur(1,data.jeu_de_carte);

	afficher_carte_joueur(2,data.jeu_de_carte);

	afficher_carte_joueur(3,data.jeu_de_carte);


 	remplir_tab_joueurs(data.joueurs);

 	//afficher_tab_joueurs(data.joueurs);




/*
    //TEST des mutex pour partages ressources inter-processus
 	pthread_t j1, j2;
 	//void * ret;
 	pthread_create(&j1, NULL, joueur, (int *) 1);
 	//pthread_create(&j2, NULL, joueur, (int *) 2);

 	pthread_join(j1, NULL);
    	//pthread_join(j2, NULL);
    sleep(1000);


    // Destruction du semaphore
    sem_unlink("ISABELLE.SEMAPHORE");
    sem_close(monSemaphore);

 */
	return 0;
}

//Fonction de jeu du joueur
void *joueur(void *arg){

    printf("test");
    /*


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

     */
}
