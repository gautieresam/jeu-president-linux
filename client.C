#include "libClient.h"

/**
 * Declaration des prototypes en début du fichier.
 **/
void afficher_carte_joueur(int numJoueur, int jeu_de_carte[TAILLE_JEU_DE_CARTE]);
void * functionThreadClient(void *pVoid1);
void * functionThreadPartie(void *pVoid);
int get_derniere_carte(int jeu_de_carte[TAILLE_JEU_DE_CARTE]);
int jouer_une_carte(int numJoueur, int carte, int jeu_de_carte[(TAILLE_JEU_DE_CARTE)], int partie[(TAILLE_JEU_DE_CARTE)]);

/***
 * Declaration des variables globales pour un client
 * */
int id=0;
int carteQueUtilisateurVeutJouer=-1;

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

/**
 * \fn void afficherLaGame(int tab[52]){
 * \brief Fonction d'affichage du jeu de carte.
 *
 * \param tab le tableau à remplir avec les cartes.
 * \return pas de return
 */
void afficherLaGame(int jeu_de_carte[TAILLE_JEU_DE_CARTE]){

    printf("INFO : afficher la partie\n");
    for (int i = 0; i < TAILLE_JEU_DE_CARTE; i++){
        if (jeu_de_carte[i]==11){
            printf("Valet ");
        } else if (jeu_de_carte[i]== 12){
            printf("Dame ");
        }else if (jeu_de_carte[i]== 13){
            printf("Roi ");
        }else{
            printf("%d ",jeu_de_carte[i]);
        }
    }
    printf("\n");
}

/**
 * \fn pthread_mutex_t unMutex = PTHREAD_MUTEX_INITIALIZER;
 * \brief Declaration d'un mutex pour proteger les ressources
 */
pthread_mutex_t unMutex = PTHREAD_MUTEX_INITIALIZER;
void MONSIG(int num);

void MONSIG(int num){

    struct data_t *memoryShared;
    switch(num){

        /**
         * \fn reception du signal SIGUSR1
         * \brief informe que l'utilisateur doit jouer, affiche ses cartes et demande ce qu'il souhaite jouer.
         * \param ouverture de la memoire partagée
         * **/
        case SIGUSR1:
            //printf("INFO SIGNAL : reception SIGUSR1\n");
            memoryShared=getSharedMemory(1056); // Demande memoire partagée
            semProtectSharedMemory=sem_open("/TEST.SEMAPHORE",O_CREAT | O_RDWR,0666,1); // Declaration protection
            sem_wait(semProtectSharedMemory); // Début zone critique
            afficherLaGame(memoryShared->partie);// Afficher le jeu
            printf("INFO : afficher les cartes du joueur");
            afficher_carte_joueur(id-1,memoryShared->jeu_de_carte); // Afficher les cartes du joueur
            sem_post(semProtectSharedMemory);// Fin de zone critique
            detachSharedMemory(memoryShared);

            printf("USER : entre ta carte :");
            scanf("%d",&carteQueUtilisateurVeutJouer);
            alarm(3);


            break;
        case SIGUSR2:
            printf("INFO SIGNAL : reception SIGUSR2\n");
            break;

        case SIGALRM:

            printf("INFO SIGNAL : reception alarme\n");
            printf("INFO : la carte à tester est %d\n",carteQueUtilisateurVeutJouer);

            memoryShared=getSharedMemory(1056); // Demande memoire partagée
            semProtectSharedMemory=sem_open("/TEST.SEMAPHORE",0,0666,1); // Declaration protection
            sem_wait(semProtectSharedMemory); // Début zone critique

            jouer_une_carte(id-1,carteQueUtilisateurVeutJouer,memoryShared->jeu_de_carte,memoryShared->partie);

            // Prevenir le pere que j'ai fini !
            //kill(memoryShared->PPID,SIGUSR1);

            detachSharedMemory(memoryShared);
            sem_post(semProtectSharedMemory);// Fin de zone critique
            break;

        default:
            break;
    }
}

int main() {

    /**
     * Reféfinition des signaux !
     */
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
    printf("INFO : Attente des informations du serveur ;\n");
    pause();
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
    printf("\n");
    for (int i = debut; i < fin; i++){
        if (jeu_de_carte[i]==11){
            printf("Valet(11) ");
        } else if (jeu_de_carte[i]== 12){
            printf("Dame(12) ");
        }else if (jeu_de_carte[i]== 13){
            printf("Roi(13) ");
        }else if (jeu_de_carte[i]!=0){
            //n'affiche pas les 0 (car signifie qu'il n'y a pas de carte
            printf("%d ",jeu_de_carte[i]);
        }
    }
    printf("Passe son tour(0) ");

    printf("\n");
}

int give_taille_de_la_main(){
    int taille = TAILLE_JEU_DE_CARTE/NB_JOUEURS;
    return taille;
}



// Retourne la derneire carte
int get_derniere_carte(int jeu_de_carte[TAILLE_JEU_DE_CARTE]){

    int i=0;
    /**
     * Gestion du cas ou il s'agit de la premiere carte posée
     */
    if(jeu_de_carte[i]==-1){
        printf("INFO : premiere carte de la partie");
        i=1;
    }else{

        while (jeu_de_carte[i]!=-1){
            i++;
        }
    }

    return jeu_de_carte[i-1];
}


int indice_partie(int partie[TAILLE_JEU_DE_CARTE]){
    int flag=0, i=0,indice=0;
    while(flag==0 && i<(TAILLE_JEU_DE_CARTE-1)){
        if(partie[i]==-1){
            flag=1;
             indice=i;
        }
        i++;
    }
    return indice;
}



int jouer_une_carte(int numJoueur, int carte, int jeu_de_carte[(TAILLE_JEU_DE_CARTE)], int partie[(TAILLE_JEU_DE_CARTE)]){

    int indice=-1,taille_main = give_taille_de_la_main(),debut=taille_main*numJoueur,fin=debut+taille_main,i=debut, flag=0,last_card=-9,carte_ok=0,indiceP=indice_partie(partie);

    printf("INFO : indice de la carte à jouer %d\n",indiceP);
    printf("INFO : function jouer_carte %d\n",carteQueUtilisateurVeutJouer);

    printf("DEBUG :");
    afficherLaGame(partie);


    /**
     * si la carte est égal à 0, le joueur choisit de passer son tour
     * Mettre -2 dans le tableau partie pour indiquer que le joueur passe son tour !
     **/
    if (carte==0){
        printf("INFO : joueur %d passe son tour",numJoueur);
        partie[indiceP]=-2;
        return 1;
    }

    /**
     * Verifier que la  carte existe bien dans le jeu
     **/
    while(flag==0 && i< fin){
        //printf("INFO : compare %d avec %d\n",jeu_de_carte[i], carte);
        if(jeu_de_carte[i]==carte){
            flag=1;
            indice=i;
            printf("INFO : la carte joué est dans le jeu du joueur\n");
        }
        i++;
    }

    /**
     * Si la carte n'est pas dans le jeu, procedure passer son tour
     * Mettre -2 dans le tableau partie pour indiquer que le joueur passe son tour !
     **/
    if( flag == 0 ){
        printf("INFO : la carte à jouer %d n'est pas dans le jeu du joueur\n",carte);
        partie[indiceP]=-2;
    }

    /**
     * Recuperer la derniere carte du jeu !
     **/
    last_card=get_derniere_carte(partie);

    /**
     * Carte=0 => couché , Carte=-1 => 1ere carte, carte=-2 => l'utilisateur a passé son tour
     **/
    printf("INFO : la derniere carte %d\n",last_card);


    if(last_card == 0 || last_card == -1){
        printf("INFO : bonne carte, carte à jouer %d, la carte d'avant %d\n",carte,last_card);
        carte_ok=1; // la carte
    }else if (last_card > 2 && carte > 2){
        //cas classique, mettre une carte suppérieur ou egale (numériquement parlant)
        if (carte >= last_card){
            printf("INFO : bonne carte, carte à jouer %d, la carte d'avant %d\n",carte,last_card);
            carte_ok=1;
        } else{
            printf("INFO : mauvaise carte ,carte à jouer %d, la carte d'avant %d\n",carte,last_card);
        }
    }else if (last_card > 2 && carte <= 2){
        //le joueur veut jouer un as ou un 2 au dessus d'une carte entre le 3 et le roi OK
        printf("INFO : bonne carte, carte à jouer %d, la carte d'avant %d\n",carte,last_card);
        carte_ok=1;
    }else if (last_card <= 2 && carte > 2){
        //le joueur veut jouer une carte au dessus d'un as ou un 2 PAS POSSIBLE
        printf("INFO : mauvaise carte ,carte à jouer %d, la carte d'avant %d\n",carte,last_card);
    }else if (last_card <= 2 && carte <= 2){
        //le joueur veut jouer un as ou un 2 au dessus d'un as ou un 2
        if (last_card<=carte){
            printf("INFO : bonne carte, carte à jouer %d, la carte d'avant %d\n",carte,last_card);
            carte_ok=1;
        }else{
            printf("INFO : mauvaise carte ,carte à jouer %d, la carte d'avant %d\n",carte,last_card);
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
        }
    }

    //jouer la carte
    if(flag==1 && carte_ok==1){
        //ajout la carte dans la partie (recupérer indice de jeu)
        if (indiceP >= TAILLE_JEU_DE_CARTE){
            printf("\n==>La carte saisie n'existe pas dans votre jeu\n");
            partie[indiceP]=-2;
            return 0;
        }
        printf("DEGUG : %d",carte);
        partie[indiceP]=carte;
        //mettre la carte à 0 dans le tab du jeu de carte
        jeu_de_carte[indice]=0;
        return 1;
    }
    return 0;

}
