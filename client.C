#include "libClient.h"

/**
 * Declaration des prototypes en début du fichier.
 **/
void afficher_carte_joueur(int numJoueur, int jeu_de_carte[TAILLE_JEU_DE_CARTE]);
int give_taille_de_la_main();
void * functionThreadClient(void *pVoid1);
void * functionThreadPartie(void *pVoid);


/**
 * \fn void *getSharedMemory(int *entier){
 * \brief Cette fonction est déclenché par un thread conditionnel.
 * \param int *cle qui est la clé de mon espace de memoire partagée
* **/
void *getSharedMemory(int cle){

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


/***
 * Declaration des variables globales pour un client
 * */
int id=-1;

/**
 * \fn sem_t *semProtectSharedMemory;
 * \brief Declaration du semaphore pour l'espace de memoire partagée
 */
sem_t *semProtectSharedMemory;

/**
 * \fn pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
 * \brief Initialisation d'un thread conditionnel.
 */
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

/**
 * \fn pthread_mutex_t unMutex = PTHREAD_MUTEX_INITIALIZER;
 * \brief Declaration d'un mutex pour proteger les ressources
 */
pthread_mutex_t unMutex = PTHREAD_MUTEX_INITIALIZER;
void MONSIG(int num);

void MONSIG(int num){
    struct data_t *memoryShared;

    printf("Bienvenu dans MONSIG\n");
    /* code */
    switch(num){
        case SIGUSR1:
            printf("INFO SIGNAL : reception SIGUSR1\n");
            memoryShared=getSharedMemory(1056);
            detachSharedMemory(memoryShared);


            break;
        case SIGUSR2:   printf("INFO SIGNAL : reception SIGUSR2\n");
            break;
        case SIGALRM: printf("INFO SIGNAL : reception SIGUSR2\n");
            break;
        default:
            break;
    }
}

int main() {

    struct sigaction newact;
    newact.sa_handler=MONSIG;
    sigemptyset(&newact.sa_mask);
    sigaction(SIGUSR1,&newact,NULL);
    sigaction(SIGALRM,&newact,NULL);


    void *ret;
    /**
     * Identifier le processus du client
     * **/
    printf("Processus client : %d \n",getpid());

    /**
    * \fn Creation d'un thread conditionnel partie
    * \brief Ce thread est déclenché lorsque j'ai reussi à m'integrer à une partie
    * \param La condition utilisé est cond
    * **/
    pthread_t threadPartie;
    pthread_create(&threadPartie, NULL,functionThreadPartie, NULL);

    /**
    * \fn Creation d'un thread client, il s'agit du premier thread du client
    * \brief Ce thread va déclenchr une condition cond si il arrive a se connecter à une partie
    * \param La condition utilisé est cond
    * **/
    pthread_t threadClient;
    pthread_create(&threadClient, NULL,functionThreadClient, NULL);
    pthread_join(threadClient, &ret);
    pthread_join(threadPartie, &ret);

    return EXIT_SUCCESS;
}


/**
 * \fn void *functionThreadPartie(void *arg)
 * \brief Cette fonction est déclenché par un thread conditionnel.
 * Ce thread permet la communication avec le serveur.
 * \param void *arg les paramatres du thread
* **/
void * functionThreadPartie(void *pVoid){
    pthread_cond_wait(&cond,&unMutex);
    printf("Attente des informations du serveur ;\n");

    alarm(10);
    pause();

    // Faire une boucle qui va regarder si il y a un pipe nommé avec les instructions
    pthread_exit(0);

}


/**
 * \fn void *functionThreadClient(void *arg){
 * \brief Premier thread client pour definir si je vais pouvoir jouer ! Pour cela on va regarder si il y a de la place dans la memoire partagé !
 *
 * \param
 * \return
 */
void * functionThreadClient(void *pVoid){

    int shmid;
    struct data_t *memoryShared;

    printf("INFO : THREAD CLIENT \n");
    /** Recuperation du segment de memoire partagee **/
    if((shmid = shmget((key_t)CLE, sizeof(struct data_t) * 1, 0)) == -1) {
        perror("Erreur lors de la recuperation du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }
    printf("INFO : creation du segment de memoire.\n");

    /** Attachement du segment de memoire partagee **/
    if((memoryShared = shmat(shmid, NULL, 0)) == (void*)-1) {
        perror("Erreur lors de l'attachement du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }

    printf("INFO : la partie est-elle commencé 0 => non / 1 => oui : %d\n",memoryShared->start);
    printf("INFO : nombre d'utilisateur si je m'ajoute à la partiee : %d\n",(memoryShared->nbCurrentUser+1));


    //semProtectSharedMemory=sem_open("/TEST.SEMAPHORE",O_CREAT | O_RDWR,0666,1);
    //sem_wait(semProtectSharedMemory); // Débit zone critique

    if( memoryShared->start == 0 && (memoryShared->nbCurrentUser+1) <= NB_JOUEURS){

        printf("INFO : il y a de la place pour integrer la partie\n");
        memoryShared->nbCurrentUser++;
        id=memoryShared->nbCurrentUser;
        memoryShared->idProcessus[id]=getpid();
        printf("INFO : identifiant %d\n",id);
        pthread_cond_signal(&cond);

    }else{
        printf("INFO : pas de place pour integrer la partie, ou alors elle a déjà commencé !");
        exit(3);

    }

    /** Detachement du segment de memoire partagee **/
    if(shmdt(memoryShared) == -1) {
        perror("Erreur lors du detachement du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }

    //sem_post(semProtectSharedMemory);// Fin de zone critique

    pthread_exit(0);
}



/**
 * \fn
 * \brief
 *
 * \param
 * \return
 */
int give_taille_de_la_main(){
    int taille = TAILLE_JEU_DE_CARTE/NB_JOUEURS;
    return taille;
}

/**
 * \fn
 * \brief
 *
 * \param
 * \return
 */
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

