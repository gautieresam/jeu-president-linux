#include "libClient.h"

/**
 * Declaration des prototypes en début du fichier.
 **/
void afficher_carte_joueur(int numJoueur, int jeu_de_carte[TAILLE_JEU_DE_CARTE]);
void * functionThreadClient(void *pVoid1);
void * functionThreadPartie(void *pVoid);
int get_derniere_carte(int jeu_de_carte[TAILLE_JEU_DE_CARTE]);
int jouer_une_carte(int numJoueur, int carte, int jeu_de_carte[(TAILLE_JEU_DE_CARTE)], int partie[(TAILLE_JEU_DE_CARTE)],int tageule[1]);
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
    int retour_jouer_carte;
    switch(num){

        /**
         * \fn reception du signal SIGUSR1 depuis le serveur
         * \brief informe que l'utilisateur doit jouer, affiche ses cartes et demande ce qu'il souhaite jouer.
         * Mise en place d'une alarme à la fin du traitement !
         * \param ouverture et fermeture de la memoire partagée
         */
        case SIGUSR1:
            printf("\n");
            carteQueUtilisateurVeutJouer=-10;
            memoryShared=getSharedMemory(1056); // Demande memoire partagée
            semProtectSharedMemory=sem_open("/TEST.SEMAPHORE",O_CREAT | O_RDWR,0666,1); // Declaration protection
            sem_wait(semProtectSharedMemory); // Début zone critique
            //TODO : enlever le commenatire de clear pour gérer l'affichage pour mieux voir
            //system("clear");
            printf("A ton tour de jouer !! Voici la partie : \n");

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
            printf("\n\nLa carte %d va être jouée\n\n",carteQueUtilisateurVeutJouer);

            memoryShared=getSharedMemory(1056); // Demande memoire partagée
            semProtectSharedMemory=sem_open("/TEST.SEMAPHORE",0,0666,1); // Declaration protection
            sem_wait(semProtectSharedMemory); // Début zone critique

            //LE retour de la fonction jouer_une_carte permet de dire si le joueur a pu poser une carte (0), doit rejouer (1) ou a passé son tour(-1).
            //Si il doit rejouer, on modifier la varibale a qui de Jouer (on la décrémente pour que le joueur suivant soit le joueur actuel)
            //S'il a passé son tour, on incrémente le compteur tageule[1] qui correspond au nb de personnes qui ont passés leur tour d'affilé
            //si la carte est posée normalement (cas 0 et rejouer 1), on rmet le compteur à 0
            retour_jouer_carte=jouer_une_carte(id-1,carteQueUtilisateurVeutJouer,memoryShared->jeu_de_carte,memoryShared->partie,memoryShared->tageule);
            //le joueur doir rejouer si 1
            if(retour_jouer_carte==1){
                if((memoryShared->aQuiDeJouer)==1){
                    memoryShared->aQuiDeJouer=NB_JOUEURS;
                    printf("MODIFICATION DU JOUEUR SUIVANT A PING ! %d",memoryShared->aQuiDeJouer);
                }else{
                    (memoryShared->aQuiDeJouer)--;
                    printf("MODIFICATION DU JOUEUR SUIVANT A PING ! %d",memoryShared->aQuiDeJouer);
                }
                //remise à zéro du compteur de nombre de joueur qui passent leur tour
                memoryShared->tageule[1]=0;
            } else if(retour_jouer_carte==-1){
                //incrémenter le compteur du nb de joueur qui passent leur tour
                (memoryShared->tageule)[1]++;
            } else{
                //remise à zéro du compteur de nombre de joueur qui passent leur tour
                memoryShared->tageule[1]=0;
            }


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
        //printf("---Pas de carte identique car premiere carte ! #0\n");
        nbCartesIdentiques = 1;

    // Si on pose la seconde carte il y a une carte avant !
    }else if(indiceP == 1 ){
        //printf("---Il y a une carte avant ! #1\n");
        nbCartesIdentiques = 1;

    //Si on pose la troisieme carte du jeu il peut y avoir 2 cartes identiques
    } else if(indiceP==2){

        if(partie[indiceP-1]==partie[indiceP-2]){
            //printf("---Il y a 2 carte identiques avant !#2\n");
            nbCartesIdentiques = 2;

        }else {
            //printf("---Il n'y pas de cartes identiques avant ! #2\n");
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
            printf("-------Il y a 1 carte avant ! #3\n");
            nbCartesIdentiques = 1;
        }

    }


    // Retourner le nombre de carte identique
    return nbCartesIdentiques;
}

/*
 * RETURN :
 * 0 -> carte posée correctement
 * 1 -> carte posée mais le jouer rejoue (carré, 2 ou nouvelle série)
 * -1 -> Joueur passe son tour
 */
int jouer_une_carte(int numJoueur, int carte, int jeu_de_carte[(TAILLE_JEU_DE_CARTE)], int partie[(TAILLE_JEU_DE_CARTE)],int tageule[1]) {

    int indice = -1, taille_main = give_taille_de_la_main(), debut = taille_main * numJoueur, fin =
            debut + taille_main, i = debut, flag = 0, last_card = -9, carte_ok = 0, indiceP = indice_partie(partie);

    printf("INFO : indice=%d carteAJouer=%d tageule=%d\n", indiceP, carteQueUtilisateurVeutJouer,tageule[0]);

    afficherLaGame(partie);

    int nbCArtesIdentiques = getNombreDeCarteIdentiqueAlaSuite(partie);
    printf("DEBUG : Nombre de carte identique %d\n",nbCArtesIdentiques);

    /**
     * Si la carte est égal à 0, le joueur choisit de passer son tour
     * Mettre -2 dans le tableau partie pour indiquer que le joueur passe son tour !
     */
    if (carte == -1) {
        printf("CASE 0.1 : INFO : joueur %d passe son tour \n", numJoueur);
        //si le joueur passe son tour, ça signifie que dans tout les cas, le tageule est baissé
        tageule[0]=0;
        return -1;
    } else {

        //Verifier que la  carte existe bien dans le jeu
        while (flag == 0 && i < fin) {
            //printf("INFO : compare %d avec %d\n",jeu_de_carte[i], carte);
            if (jeu_de_carte[i] == carte) {
                flag = 1;
                indice = i;
                //printf("INFO : la carte joué est dans le jeu du joueur\n");
            }
            i++;
        }
        /**
        * Si la carte n'est pas dans le jeu, procedure passer son tour
        * Mettre -2 dans le tableau partie pour indiquer que le joueur passe son tour !
        */
        if (flag == 0) {
            printf("CASE 0.2 : INFO : la carte à jouer %d n'est pas dans le jeu du joueur\n", carte);
            //Pas dans le jeu du joueur, il passe son tour
            // si le joueur passe son tour, ça signifie que dans tout les cas, le tageule est baissé
            tageule[0]=0;
            return -1;
        } else {//la carte est dans le jeu

            //Recuperer la derniere carte du jeu !
            last_card = get_derniere_carte(partie);

            //CAS où tout les joueurs ont déjà passé leur tour <=> joue ce que l'on veut tant que la carte est dans le jeu
            //>= car si la carte n'existe pas dans le jeu, le compteur aura dépassé le NBJOUEUR
            if (tageule[1]>=NB_JOUEURS){
                printf("===> TOUS LES JOUEURS ONT DEJA PASSE LEUR TOUR, JOUER CARTE DU JEU QUE L'ON VEUT");
                partie[indiceP] = carte;
                jeu_de_carte[indice] = 0;
                return 0;
            }else if (nbCArtesIdentiques == 1) {
                // Si le nombre de carte égal à 1 on doit poser une carte équivalente ou supérieur

                // Si on pose une carte au dessus d'une autre sachant qu'on ne joue pas un 2 ou un 1
                // 1 3 4 5 6 7 8 9 10 11 12 13
                if (last_card<carte && carte!=2 && carte!=1) {
                    printf("CASE 1.1 : INFO : bonne carte, carte à jouer %d, la carte d'avant %d #0\n", carte, last_card);
                    partie[indiceP] = carte;
                    jeu_de_carte[indice] = 0;
                    return 0;

                // Si on pose une carte identique à la precedente est différente de 2
                } else if(last_card==carte && carte!=2 && carte!=1){

                    // Il faut lever le tageule ! pour le prochain joueur
                    tageule[0]=1;
                    printf("Voici la nouvelle valeur du tageule : %d",tageule[0]);

                    printf("CASE 1.2 : INFO : bonne carte, carte à jouer %d, la carte d'avant %d on leve le tageule=%d #1\n", carte, last_card,tageule[0]);
                    partie[indiceP] = carte;
                    jeu_de_carte[indice] = 0;
                    return 0;


                // Si l'utilisateur pose un 2
                }else if(carte==2){
                    printf("CASE 1.3 INFO : bonne carte, carte à jouer %d, la carte d'avant %d #2\n", carte, last_card);
                    partie[indiceP] = carte;
                    jeu_de_carte[indice] = 0;

                    printf("2 posé, série fermée");
                    //renvoi 1 pour dire de faire rejouer le joueur
                    return 1;

                }else if(carte==1 && last_card!=1){
                    //jouer un AS, toujours possible, juste si carte égale lever le TG
                    printf("CASE 1.1 : INFO : bonne carte, carte à jouer %d, la carte d'avant %d #0\n", carte, last_card);
                    partie[indiceP] = carte;
                    jeu_de_carte[indice] = 0;
                    return 0;
                }else if(carte==1 && last_card==1){
                    //jouer un as au dessu d'un as = lever le TG
                    tageule[0]=1;
                    printf("Voici la nouvelle valeur du tageule : %d",tageule[0]);

                    printf("CASE 1.2 : INFO : bonne carte, carte à jouer %d, la carte d'avant %d on leve le tageule=%d #1\n", carte, last_card,tageule[0]);
                    partie[indiceP] = carte;
                    jeu_de_carte[indice] = 0;
                    return 0;
                }else{
                    printf("CASE 1.4 : La carte n'est pas bonne, passe ton tour #3");
                    // si le joueur passe son tour, ça signifie que dans tout les cas, le tageule est baissé
                    tageule[0]=0;
                    return -1;
                }

            } else if (nbCArtesIdentiques == 2) {

                // Il y a 2 cartes identiques l'utilisateur doit obligatoirement poser la même carte si le tageule est à 1 !
                if (last_card==carte && tageule[0]==1) {
                    printf("CASE 2.1 : INFO : bonne carte, carte à jouer %d, la carte d'avant %d #4\n", carte, last_card);
                    partie[indiceP] = carte;
                    jeu_de_carte[indice] = 0;
                    return 0;

                // Il y a 2 cartes identiques mais une personne a passé son tour tu peux donc poser ce que tu veux
                // Le joueur suivant va pouvoir jouer ce qu'il veut
                } else if  ( tageule[0]==0) {
                    //QUand nous sommes ici, c'est qu'un joueur a déjà passé son tour, donc on joue normalement (même scénario de jeu que quand carte_identique ==1)

                    // Si on pose une carte au dessus d'une autre sachant qu'on ne joue pas un 2 ou un 1
                    // 1 3 4 5 6 7 8 9 10 11 12 13
                    if (last_card<carte && carte!=2 && carte!=1) {
                        printf("CASE 2.2 : INFO : bonne carte, carte à jouer %d, la carte d'avant %d #0\n", carte, last_card);
                        partie[indiceP] = carte;
                        jeu_de_carte[indice] = 0;
                        return 0;

                        // Si on pose une carte identique à la precedente est différente de 2
                    } else if(last_card==carte && carte!=2 && carte!=1){

                        // Il faut lever le tageule ! pour le prochain joueur
                        tageule[0]=1;
                        printf("Voici la nouvelle valeur du tageule : %d",tageule[0]);

                        printf("CASE 2.3 : INFO : bonne carte, carte à jouer %d, la carte d'avant %d on leve le tageule=%d #1\n", carte, last_card,tageule[0]);
                        partie[indiceP] = carte;
                        jeu_de_carte[indice] = 0;
                        return 0;


                    }else if (carte==1 && last_card!=1){
                        printf("CASE 2.2 : INFO : bonne carte, carte à jouer %d, la carte d'avant %d #0\n", carte, last_card);
                        partie[indiceP] = carte;
                        jeu_de_carte[indice] = 0;
                        return 0;
                    }else if (carte==1 && last_card==1){
                        // Il faut lever le tageule ! pour le prochain joueur
                        tageule[0]=1;
                        printf("Voici la nouvelle valeur du tageule : %d",tageule[0]);

                        printf("CASE 2.3 : INFO : bonne carte, carte à jouer %d, la carte d'avant %d on leve le tageule=%d #1\n", carte, last_card,tageule[0]);
                        partie[indiceP] = carte;
                        jeu_de_carte[indice] = 0;
                        return 0;
                    }else if(carte==2 ){
                        printf("CASE 2.4 : INFO : bonne carte, carte à jouer %d, la carte d'avant %d #2\n", carte, last_card);
                        partie[indiceP] = carte;
                        jeu_de_carte[indice] = 0;

                        printf("2 posé, série fermée");
                        //renvoi 1 pour dire de faire rejouer le joueur
                        return 1;

                    }else{
                        printf("2.5 : INFO :  mauvaise carte ,carte à jouer %d, la carte d'avant %d #6\n", carte, last_card);
                        return -1;
                    }

                // Sinon, ce que la carte ne peut pas être jouée (carte != derneir carte alors que le tageule est levé)
                }else {
                    printf("CASE 2.6 : INFO :  mauvaise carte ,carte à jouer %d, la carte d'avant %d #6\n", carte, last_card);
                    tageule[0]=0;//baisser le tageule
                    printf("Voici la nouvelle valeur du tageule : %d",tageule[0]);
                    return -1;
                }
            // Il y a 3 cartes identiques l'utilisateur
            } else if (nbCArtesIdentiques == 3) {
                // Il y a 2 cartes identiques l'utilisateur doit obligatoirement poser la même carte si le tageule est à 1 !
                // Il va recuperer la main !
                if (last_card == carte && tageule[0]==1) {
                    printf("CASE 3.1 : INFO : bonne carte, carte à jouer %d, la carte d'avant %d #7\n", carte, last_card);
                    partie[indiceP] = carte;
                    jeu_de_carte[indice] = 0;

                    printf("FERMETURE DU JEU : 2 , rejouer\n");
                    printf("MODIFICATION JOUEUR SUIVANT\n");
                    //renvoi 1 pour dire de faire rejouer le joueur
                    return 1;

                // Faire jouer le joueur
                } else if(tageule[0]==0){
                    //QUand nous sommes ici, c'est qu'un joueur a déjà passé son tour, donc on joue normalement (même scénario de jeu que quand carte_identique ==1)

                    // Si on pose une carte au dessus d'une autre sachant qu'on ne joue pas un 2 ou un 1
                    // 1 3 4 5 6 7 8 9 10 11 12 13
                    if (last_card<carte && carte!=2 && carte!=1) {
                        printf("3.2 : INFO : bonne carte, carte à jouer %d, la carte d'avant %d #0\n", carte, last_card);
                        partie[indiceP] = carte;
                        jeu_de_carte[indice] = 0;
                        return 0;

                        // Si on pose une carte identique à la precedente et différente de 2
                    } else if(last_card==carte && carte!=2 && carte !=1){
                        printf("3.3 : INFO : bonne carte, carte à jouer %d, la carte d'avant %d on leve le tageule=%d #1\n", carte, last_card,tageule[0]);
                        partie[indiceP] = carte;
                        jeu_de_carte[indice] = 0;

                        tageule[0]=0;
                        printf("Voici la nouvelle valeur du tageule : %d",tageule[0]);

                        printf("FERMETURE CARRE, rejouer\n");
                        //renvoi 1 pour dire de faire rejouer le joueur
                        return 1;
                    }else if (carte==1 && last_card!=1){
                        printf("3.2 : INFO : bonne carte, carte à jouer %d, la carte d'avant %d #0\n", carte, last_card);
                        partie[indiceP] = carte;
                        jeu_de_carte[indice] = 0;
                        return 0;
                    }else if (carte==1 && last_card==1){
                        printf("3.3 : INFO : bonne carte, carte à jouer %d, la carte d'avant %d on leve le tageule=%d #1\n", carte, last_card,tageule[0]);
                        partie[indiceP] = carte;
                        jeu_de_carte[indice] = 0;

                        tageule[0]=0;
                        printf("Voici la nouvelle valeur du tageule : %d",tageule[0]);

                        printf("FERMETURE CARRE, rejouer\n");
                        //renvoi 1 pour dire de faire rejouer le joueur
                        return 1;
                    }else if(carte==2 ){
                        printf("3.4 : INFO : bonne carte, carte à jouer %d, la carte d'avant %d #2\n", carte, last_card);
                        partie[indiceP] = carte;
                        jeu_de_carte[indice] = 0;

                        //renvoi 1 pour dire de faire rejouer le joueur
                        return 1;

                    }else{
                        printf("3.5 : INFO : bonne carte, carte à jouer %d, la carte d'avant %d #2\n", carte, last_card);
                        return 0;
                    }
                } else{
                    printf("3.6 : INFO :  mauvaise carte ,carte à jouer %d, la carte d'avant %d #6\n", carte, last_card);
                    tageule[0]=0;//baisser le tageule
                    printf("Voici la nouvelle valeur du tageule : %d",tageule[0]);
                    return -1;
                }

            } else {
                printf("==>Il y a un pb mdr ! \n");
            }
        }
    }
}