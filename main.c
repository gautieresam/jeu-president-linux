#include "gestion_jeu.h"
#include "server.h"

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
 * \fn sem_t *monSemaphore;
 * \brief Initialisation du premier semaphore
 */
sem_t *semProtectSharedMemory;

int main(){

    struct data_t *data;

    /**
    * \fn Creation de la memoire partagé qui est une structure data_t
    * \brief Attention il faut jouer ./clean si la memoire partagé n'a pas été supprimé !
    * \param la clé utilisé est 1056
    * **/
    int shmid, i, *debut;
    struct data_t *memoryShared;

    if((shmid = shmget((key_t)CLE, sizeof(struct data_t) * 1, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        if(errno == EEXIST)
            fprintf(stderr, "Le segment de memoire partagee (cle=%d) existe deja\n", CLE);
        else
            perror("Erreur lors de la creation du segment de memoire ");
        exit(EXIT_FAILURE);
    }
    printf("Serveur : segment cree.\n");

    if((memoryShared = shmat(shmid, NULL, 0)) == (void*)-1) {
        perror("Erreur lors de l'attachement du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }

    // EXEMPLE POUR MARIETTE

    printf("%d",memoryShared->start);

    initalisation_du_jeu_de_carte(memoryShared->jeu_de_carte);
    afficher_tab(memoryShared->jeu_de_carte);
    melanger_cartes(memoryShared->jeu_de_carte);
    afficher_tab(memoryShared->jeu_de_carte);
    afficher_carte_joueur(0,memoryShared->jeu_de_carte);
    afficher_carte_joueur(1,memoryShared->jeu_de_carte);
    afficher_carte_joueur(2,memoryShared->jeu_de_carte);
    afficher_carte_joueur(3,memoryShared->jeu_de_carte);

    remplir_tab_joueurs(memoryShared->joueurs);
    afficher_tab_joueurs(memoryShared->joueurs);


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
