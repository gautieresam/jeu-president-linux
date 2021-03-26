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
 * \fn sem_t *semProtectSharedMemory;
 * \brief Declaration du semaphore pour l'espace de memoire partagée
 */
sem_t *semProtectSharedMemory;


/**
 * \fn pthread_mutex_t unMutex = PTHREAD_MUTEX_INITIALIZER;
 * \brief Declaration d'un mutex pour proteger les ressources
 */
pthread_mutex_t unMutex = PTHREAD_MUTEX_INITIALIZER;


int main(){

    /**
    * \fn Creation de la memoire partagé qui est une structure data_t
    * \brief Attention il faut jouer ./clean si la memoire partagé n'a pas été supprimé !
    * \param la clé utilisé est 1056
    * **/
    int shmid;
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

    // EXEMPLE POUR MARIETTE POUR ACCEDER AUX INFORMATIONS, NOTATION POINTEE
    printf("%d",memoryShared->start);

    // Mise en place d'un semaphore
    semProtectSharedMemory=sem_open("/TEST.SEMAPHORE",O_CREAT | O_RDWR,0666,1);
    //sem_wait(semProtectSharedMemory); // Débit zone critique

    initalisation_du_jeu_de_carte(memoryShared->jeu_de_carte);
    afficher_tab(memoryShared->jeu_de_carte);
    melanger_cartes(memoryShared->jeu_de_carte);
    afficher_tab(memoryShared->jeu_de_carte);
    afficher_carte_joueur(0,memoryShared->jeu_de_carte);
    afficher_carte_joueur(1,memoryShared->jeu_de_carte);
    remplir_tab_joueurs(memoryShared->joueurs);
    afficher_tab_joueurs(memoryShared->joueurs);


    //sem_post(semProtectSharedMemory);// Fin de zone critique

    /**
     * \fn Creation d'un thread conditionnel partie
     * \brief Ce thread est déclenché lorsque 4 joueurs sont connectés à une partie
     * \param La condition utilisé est cond
    * **/
    pthread_t threadPartie;
    pthread_create(&threadPartie, NULL,functionThreadPartie, NULL);



    /**
     * \fn Creation d'un thread maitre, il s'agit du premier thread du serveur
     * \brief Ce thread va declencher une condition cond si il detecte 4 joueurs qui sont connectés à une partie
     * \param La condition utilisé est cond
    * **/
    void *ret;
    pthread_t threadMaitre;
    pthread_create(&threadMaitre, NULL,functionThreadMaitre, NULL);
    pthread_join(threadMaitre, &ret);



    return 0;
}



/**
 * \fn void *functionThreadPartie(void *arg) {
 * \brief Cette fonction est déclenché par un thread conditionnel.
 * Ce thread permet la communication avec les clients.
 * \param void *arg les paramatres du thread
* **/
void *functionThreadPartie(void *arg) {

    pthread_cond_wait(&cond,&unMutex);
    printf("Il y a eu un declenchement de la partie ;\n");


    // Creation d'un tibe nommé pour communiquer avec un client dans un sens.
    mkfifo("serverToJ1", 0666);




}


/**
 * \fn vvoid *functionThreadMaitre(void *arg){
 * \brief Cette fonction est déclenché par le premier thread du programme. Il permet d'attendre que 4 personnes se connectent à la partie.
 * \param void *arg les paramatres du thread
* **/
void *functionThreadMaitre(void *arg){

    int shmid;
    struct data_t *memoryShared;

    printf("THREAD Maitre \n");
    if((shmid = shmget((key_t)CLE, sizeof(struct data_t) * 1, 0)) == -1) {
        perror("Erreur lors de la recuperation du segment de memoire partagee\n");
        exit(EXIT_FAILURE);
    }
    printf("Client : recuperation du segment de memoire.\n");

    int flag = -1 ;
    while (flag == -1 ){

        if((memoryShared = shmat(shmid, NULL, 0)) == (void*)-1) {
            perror("Erreur lors de l'attachement du segment de memoire partagee ");
            exit(EXIT_FAILURE);
        }

        if( memoryShared->nbCurrentUser == NB_JOUEURS){
            flag=0;
            memoryShared->start=1;
            pthread_cond_signal(&cond);

        }else{
            printf("En attente des joueurs\n");
        }

        if(shmdt(memoryShared) == -1) {
            perror("Erreur lors du detachement du segment de memoire partagee ");
            exit(EXIT_FAILURE);
        }
        sleep(3);
    }

    pthread_exit(0);
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
