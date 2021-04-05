#include "libClient.h"

/**
 * Declaration des prototypes en début du fichier.
 **/
void afficher_carte_joueur(int numJoueur, int jeu_de_carte[TAILLE_JEU_DE_CARTE]);
void * functionThreadClient(void *pVoid1);
void * functionThreadPartie(void *pVoid);
int get_derniere_carte(int jeu_de_carte[TAILLE_JEU_DE_CARTE]);
int jouer_une_carte(int numJoueur, int carte, int jeu_de_carte[(TAILLE_JEU_DE_CARTE)], int partie[(TAILLE_JEU_DE_CARTE)]);
int compterNombreDeCartesdUnJoueur(int numJoueur,int jeu_de_carte[TAILLE_JEU_DE_CARTE]);
void * functionThreadTest(void *pVoid);
int getNombreDeCarteIdentiqueAlaSuite(int partie [(TAILLE_JEU_DE_CARTE)]);


/***
 * Declaration des variables globales pour un client
 * */
int id=0;
int carteQueUtilisateurVeutJouer=-10;

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
         * \fn reception du signal SIGUSR1 depuis le serveur
         * \brief informe que l'utilisateur doit jouer, affiche ses cartes et demande ce qu'il souhaite jouer.
         * Mise en place d'une alarme à la fin du traitement !
         * \param ouverture et fermeture de la memoire partagée
         */
        case SIGUSR1:
            carteQueUtilisateurVeutJouer=-10;
            memoryShared=getSharedMemory(1056); // Demande memoire partagée
            semProtectSharedMemory=sem_open("/TEST.SEMAPHORE",O_CREAT | O_RDWR,0666,1); // Declaration protection
            sem_wait(semProtectSharedMemory); // Début zone critique
            afficherLaGame(memoryShared->partie);// Afficher le jeu
            printf("INFO : afficher les cartes du joueur");


            afficher_carte_joueur(id-1,memoryShared->jeu_de_carte); // Afficher les cartes du joueur

            int nbCartesJoueurs;
            nbCartesJoueurs=compterNombreDeCartesdUnJoueur(id-1,memoryShared->jeu_de_carte);


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

            /**
             * \fn reception du signal SIGALRM
             * \brief l'utilisateur a saisi sa valeur nous allons la verifier avec la fonction jouer_une_carte()
             * \param ouverture et fermeture de la memoire partagée
             */
            //printf("INFO SIGNAL : reception alarme\n");
            //printf("INFO : la carte à tester est %d\n",carteQueUtilisateurVeutJouer);

            memoryShared=getSharedMemory(1056); // Demande memoire partagée
            semProtectSharedMemory=sem_open("/TEST.SEMAPHORE",0,0666,1); // Declaration protection
            sem_wait(semProtectSharedMemory); // Début zone critique

            jouer_une_carte(id-1,carteQueUtilisateurVeutJouer,memoryShared->jeu_de_carte,memoryShared->partie);

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

    struct sigaction newact;
    newact.sa_handler=MONSIG;
    sigemptyset(&newact.sa_mask);
    sigaction(SIGUSR1,&newact,NULL);
    sigaction(SIGALRM,&newact,NULL);

    pthread_cond_wait(&cond,&unMutex);
    printf("INFO : Attente des informations du serveur ;\n");

    while (1){
        sleep(20000);
    }

    pthread_exit(0);
}





/**
 * \fn void *functionThreadClient(void *arg){
 * \brief Premier thread client pour definir si je vais pouvoir jouer ! Pour cela on va regarder si il y a de la place dans la memoire partagé !
 * \param void *pVoid
 */
void * functionThreadClient(void *pVoid){

    int shmid;
    struct data_t *memoryShared;

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

    pthread_exit(0);
}

/**
 * \fn void afficher_carte_joueur(int numJoueur, int jeu_de_carte[TAILLE_JEU_DE_CARTE]);
 * \brief Afficher les carte d'un joueur suivant son identifiant !
 * Pour rappel, le tableau est divisé en NB_JOUEUR
 * Le joueur 1 posséde les cartes du tableau de 0 à TAILLE_JEU_DE_CARTE/NB_JOUEUR
 * Le joueur 2 posséde les cartes du tableau de TAILLE_JEU_DE_CARTE/NB_JOUEUR à 2 fois TAILLE_JEU_DE_CARTE/NB_JOUEUR
 * \param int numJoueur
 * \param int jeu_de_carte[TAILLE_JEU_DE_CARTE]
 * \return
 */
void afficher_carte_joueur(int numJoueur,int jeu_de_carte[TAILLE_JEU_DE_CARTE]){
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





/**
 * \fn void compterNombreDeCartesdUnJoueur(int numJoueur,int jeu_de_carte[TAILLE_JEU_DE_CARTE]);
 * \brief Compte le nombre de cartes du joueur en fonction de son identifiant !
 * \param int <numJoueur
 * \param int jeu_de_carte[TAILLE_JEU_DE_CARTE]
 * \return
 */
int compterNombreDeCartesdUnJoueur(int numJoueur,int jeu_de_carte[TAILLE_JEU_DE_CARTE]){
    int taille_main = give_taille_de_la_main();
    //printf("DEBUG : compterNombreDeCartesdUnJoueur, taille de la main %d\n",taille_main);
    int debut=taille_main*numJoueur;
    int fin=debut+taille_main;
    int compterNbZero=0;
    for (int i = debut; i < fin; i++){

        if (jeu_de_carte[i]==0){
            //Compte le nombre de 0
            compterNbZero++;
        }
    }
    //printf("Il reste %d cartes dans la main du joueur ! \n",taille_main-compterNbZero);
    return taille_main-compterNbZero;
}



/**
 * \fn int give_taille_de_la_main();
 * \brief Donne la taille de la main en divisant la taille du jeu de carte par le nombre de joueur
 * @return la taille de la main d'un joueur
 */
int give_taille_de_la_main(){
    int taille = TAILLE_JEU_DE_CARTE/NB_JOUEURS;
    return taille;
}


/**
 * \fn int get_derniere_carte(int jeu_de_carte[TAILLE_JEU_DE_CARTE]);
 * \brief Retourne la derniere carte qui a été posé durant la partie !
 * @param jeu_de_carte
 * @return
 */
int get_derniere_carte(int jeu_de_carte[TAILLE_JEU_DE_CARTE]){
    int i=0;
    if(jeu_de_carte[i]==-1){
        printf("INFO : premiere carte de la partie\n");
        i=1;
    }else{
        while (jeu_de_carte[i]!=-1){
            i++;
        }
    }
    return jeu_de_carte[i-1];
}

/**
 * \fn int indice_partie(int partie[TAILLE_JEU_DE_CARTE])
 * \brief Retourne l'indice du tableau ou le joueur va poser sa carte !
 * @param partie, le tableau d'entier de la partie
 * @return indice, un entier
 */
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

/**
 * \fn int getNombreDeCarteIdentiqueAlaSuite(int partie [(TAILLE_JEU_DE_CARTE)])
 * \brief Retourne le nombre de carte identique à un instant T de la partie
 * @param partie, le tableau d'entier de la partie
 * @return nbCartesIdentiques , un entier
 */
int getNombreDeCarteIdentiqueAlaSuite(int partie [(TAILLE_JEU_DE_CARTE)]){

    int nbCartesIdentiques=-1;
    int indice=-1, flag=0,last_card=-9,carte_ok=0;
    int indiceP=indice_partie(partie);

    //printf("--INFO getNombreDeCarteIdentiqueAlaSuite  \n");
    //printf("--INFO : indiceP %d\n",indiceP);


    // Si on pose la premiere carte il n'est pas possible d'avoir une carte identique avant..
    if(indiceP == 0 ) {
        printf("---Pas de carte identique car premiere carte ! #0\n");
        nbCartesIdentiques = 0;

    // Si on pose la seconde carte il y a une carte avant !
    }else if(indiceP == 1 ){
        printf("---Il y a une carte avant ! #1\n");
        nbCartesIdentiques = 1;

    //Si on pose la troisieme carte du jeu il peut y avoir 2 cartes identiques
    } else if(indiceP==2){

        if(partie[indiceP-1]==partie[indiceP-2]){
            printf("---Il y a 2 carte identiques avant !#2\n");
            nbCartesIdentiques = 2;

        }else {
            printf("---Il n'y pas de cartes identiques avant ! #2\n");
            nbCartesIdentiques = 1;
        }

    //Si on pose la troisieme carte du jeu il peut y avoir 3 cartes identiques !
    }else{

        // Si il y a 3 cartes identiques avant !
        if(partie[indiceP-1]==partie[indiceP-2] && partie[indiceP-2]==partie[indiceP-3] && partie[indiceP-1]==partie[indiceP-3]){
            printf("---Il y a 3 cartes identiques avant ! #3\n");
            nbCartesIdentiques = 3;

        // Si il y a 2 cartes identiques avant !
        }else if(partie[indiceP-1]==partie[indiceP-2] && partie[indiceP-2]!=partie[indiceP-3] ){
            printf("---Il y a 2 cartes identiques avant ! #3\n");
            nbCartesIdentiques = 2;

        // Il y a donc 1 carte
        }else{
            printf("---Il y a 1 carte avant ! #3\n");
            nbCartesIdentiques = 1;
        }

    }


    // Retourner le nombre de carte identique
    return nbCartesIdentiques;
}


int jouer_une_carte(int numJoueur, int carte, int jeu_de_carte[(TAILLE_JEU_DE_CARTE)], int partie[(TAILLE_JEU_DE_CARTE)]){

    int indice=-1,taille_main = give_taille_de_la_main(),debut=taille_main*numJoueur,fin=debut+taille_main,i=debut, flag=0,last_card=-9,carte_ok=0,indiceP=indice_partie(partie);

    printf("\nFONCTION : jouer une carte \n");
    //printf("INFO : indice de la carte à jouer %d \n",indiceP);
    printf("INFO : indice=%d carteAJouer=%d\n",indiceP,carteQueUtilisateurVeutJouer);

    afficherLaGame(partie);

    int nbCArtesIdentiques = getNombreDeCarteIdentiqueAlaSuite(partie);
    //printf("DEBUG : Nombre de carte identique %d\n",nbCArtesIdentiques);

    /**
     * Si la carte est égal à 0, le joueur choisit de passer son tour
     * Mettre -2 dans le tableau partie pour indiquer que le joueur passe son tour !
     */
    if ( carte == -1 ){
        printf("INFO : joueur %d passe son tour \n",numJoueur);

    }else{

        //Verifier que la  carte existe bien dans le jeu
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
        */
        if( flag == 0 ){
            printf("INFO : la carte à jouer %d n'est pas dans le jeu du joueur\n",carte);
            carte_ok=-3;
        }

        /**
        * Recuperer la derniere carte du jeu !
        */
        last_card=get_derniere_carte(partie);
        //printf("INFO : la derniere carte %d\n",last_card);

        /**
        * Analyse de la partie
        */
        if( last_card == -1){ // Cas l'utilisateur est couché ou bien premiere carte
            printf("INFO : bonne carte, carte à jouer %d, la carte d'avant %d\n",carte,last_card);
            carte_ok=1; // la carte
        }else if (last_card > 2 && carte > 2){
            if (carte >= last_card){ //cas classique, mettre une carte suppérieur ou egale (numériquement parlant)
                printf("INFO : bonne carte, carte à jouer %d, la carte d'avant %d\n",carte,last_card);
                carte_ok=1;
            } else{
                printf("INFO : mauvaise carte ,carte à jouer %d, la carte d'avant %d\n",carte,last_card);
            }
        }else if (last_card > 2 && carte <= 2){ //le joueur veut jouer un as ou un 2 au dessus d'une carte entre le 3 et le roi OK

            printf("INFO : bonne carte, carte à jouer %d, la carte d'avant %d\n",carte,last_card);
            carte_ok=1;
        }else if (last_card <= 2 && carte > 2){ //le joueur veut jouer une carte au dessus d'un as ou un 2 PAS POSSIBLE
            printf("INFO : mauvaise carte ,carte à jouer %d, la carte d'avant %d\n",carte,last_card);
        }else if (last_card <= 2 && carte <= 2){  //le joueur veut jouer un as ou un 2 au dessus d'un as ou un 2
            if (last_card<=carte){
                printf("INFO : bonne carte, carte à jouer %d, la carte d'avant %d\n",carte,last_card);
                carte_ok=1;
                // Mettre if et dire que c est l utilisatuer qui doit rejouer
            }else{
                printf("INFO : mauvaise carte ,carte à jouer %d, la carte d'avant %d\n",carte,last_card);
            }
        }else{

            // Il faut traiter le cas ou la carte avant est -1 et -2
            carte_ok==1;
        }


        //jouer la carte
        if(flag==1 && carte_ok==1){
            //ajout la carte dans la partie (recupérer indice de jeu)
            if (indiceP >= TAILLE_JEU_DE_CARTE){
                printf("\n==>La carte saisie n'existe pas dans votre jeu\n");
                partie[indiceP]=-2;
            }
            partie[indiceP]=carte;
            jeu_de_carte[indice]=0;
        }

    }




/*


    // SI tous les joueurs passe que se passee til
    // Si la carte d'avnt est couché il faut regarder la carte encore d avant

    //2. poser obligatoirement la meme carte
    //analyser les 3 dernières cartes
    // 0 1 2 3 4
    // 2 2 2 X
    if (indiceP>2 ){ // && partie[indiceP-1]==last_card

        if(partie[indiceP-1]==partie[indiceP-2] && partie[indiceP-1]==partie[indiceP-3]  ){

            // L'utilisateur va finir la plie et rejouer 3 consecutif
            carte_ok=1;
        }else if(partie[indiceP-1]==partie[indiceP-2] && partie[indiceP-2]==carte){
            // Si 2 cartes à la suite identique le joueur peut jouer
            carte_ok=1;
        }else{
            printf("toto\n");
        }
    }


 */
    printf("------------------------------------------------------\n\n");
    return 0;

}
