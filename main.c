#include <sys/wait.h>
#include "gestion_jeu.h"

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

void MONSIG(int num);

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


/**
 * \fn void sleep_perso(double seconde)
 * \brief Fonction d'attente maison !
 * @param seconde
 */
void sleep_perso(double seconde){
    //printf("function");
    time_t time1, time2;
    time(&time1);
    time(&time2);

    while (difftime(time2,time1)<seconde){
        time(&time2);
    }
}

/**
 * \fn int kill(pid_t pid, int sig);
 * @param pid_t pid, le processus destination
 * @param int sig, le numéro du signal
 * @return
 */
int kill(pid_t pid, int sig);


/**
 *
 * @param num
 */
void MONSIG(int num){
    struct data_t *memoryShared;
    switch(num){

        case SIGUSR1:
            printf("INFO : reception SIGUSR1\n");

            break;
            /**
             * \fn reception du signal SIGALRM
             * \brief alrm pour le thread pour ping l'utilisateur voulu !
             * \param ouverture de la memoire partagée
             */
        case SIGALRM:
            printf("INFO : Reception du signal alarme");

            break;

        default:
            break;
    }
}


int main(){

    /**
    * Redéfinition des actions pour la reception d'un signal !
    */
    struct sigaction newact;
    newact.sa_handler=MONSIG;
    sigemptyset(&newact.sa_mask);
    sigaction(SIGUSR1,&newact,NULL);
    sigaction(SIGALRM,&newact,NULL);

    /**
    * \brief Creation de la memoire partagé qui est une structure data_t
    * \ Attention il faut jouer ./clean si la memoire partagé n'a pas été supprimé !
    * \param la clé utilisé est 1056
    */
    struct data_t *memoryShared;
    createSharedMemory(1056);
    memoryShared=getSharedMemory(1056);

    /**
     * \brief Mise en place d'un semaphore nommé TEST.SEMAPHORE
     * Attendre que le sémaphore soit disponible pour faire mon action !
     * \param 0666 pour écrire et lire depuis le sémaphore
     */
    semProtectSharedMemory=sem_open("/TEST.SEMAPHORE",O_CREAT | O_RDWR,0666,1);
    sem_wait(semProtectSharedMemory); // Débit zone critique

    /**
     * \brief Affecter le PID de mon processus serveur dans la structure de la memoire partagée
     */
    memoryShared->PPID=getpid();
    printf("INFO : mon PID %d",getpid());

    /**
     * \brief Initialiser le joueur qui va commencer le premier
     */
    memoryShared->aQuiDeJouer=1;
    memoryShared->tageule[0]=0;
    memoryShared->tageule[1]=0;//nb de joueurs qui ont passé leur tour


    initalisation_du_jeu_de_carte(memoryShared->jeu_de_carte,memoryShared->partie);
    afficher_tab(memoryShared->jeu_de_carte);
    melanger_cartes(memoryShared->jeu_de_carte);
    afficher_tab(memoryShared->jeu_de_carte);
    //afficher_carte_joueur(0,memoryShared->jeu_de_carte);
    //afficher_carte_joueur(1,memoryShared->jeu_de_carte);
    //remplir_tab_joueurs(memoryShared->joueurs);
    //afficher_tab_joueurs(memoryShared->joueurs);

    /**
     * Fin de la zone critique avec la fermeture du semaphore !
     */
    sem_post(semProtectSharedMemory);

    /**
     * \fn Creation d'un thread conditionnel partie
     * \brief Ce thread est déclenché lorsque 4 joueurs sont connectés à une partie
     * \param La condition utilisé est cond
    */
    pthread_t threadPartie;
    pthread_create(&threadPartie, NULL,functionThreadPartie, NULL);

    /**
     * \fn Creation d'un thread maitre, il s'agit du premier thread du serveur
     * \brief Ce thread va declencher une condition cond si il detecte 4 joueurs qui sont connectés à une partie
     * \param La condition utilisé est cond
    */
    void *ret;
    pthread_t threadMaitre;
    pthread_create(&threadMaitre, NULL,functionThreadMaitre, NULL);
    pthread_join(threadMaitre, &ret);

    /**
     * Supprimer la memoire partagée
     * **/
    //detachSharedMemory(memoryShared);

    /**
     * Suspend l'exécution du thread
     */
    pthread_join(threadPartie, &ret);

    return 0;
}

/**
 * \fn void *functionThreadPartie(void *arg) {
 * \brief Cette fonction est déclenché par un thread conditionnel.
 * Ce thread permet la communication avec les clients.
 * \param void *arg les paramatres du thread
*/
void * functionThreadPartie(void *pVoid) {

    pthread_cond_wait(&cond,&unMutex);
    printf("INFO : declenchement de la partie\n");
    struct data_t *memoryShared;

    while (1){

        memoryShared=getSharedMemory(1056); // Demande memoire partagée
        semProtectSharedMemory=sem_open("/TEST.SEMAPHORE",0,0666,1); // Declaration protection
        sem_wait(semProtectSharedMemory); // Début zone critique
        printf("DEBUG : a qui de jouer ? %d\n",memoryShared->aQuiDeJouer);
        int aQuiDeJouer=memoryShared->aQuiDeJouer;

        if (compterNombreDeCartesdUnJoueur(aQuiDeJouer-1,memoryShared->jeu_de_carte)==24){
            printf("Jeu finis, joueur %d a gagné (test à 22 cartes)",aQuiDeJouer);
            //envoyer un signal de fin de jeu aux autres processus
            for (int i = 1; i < NB_JOUEURS+1; i++) {
                kill(memoryShared->idProcessus[i],SIGUSR2);
                printf("SIGUSR2 envoyé à joueur %d",i);
            }
            //TODO : sortir du thread + TUER MEMOIRE PARTAGEE ET/OU SEMAPHORE
        } else{
            //sinon jouer en envoyant un signal au prochain joueur
            if(aQuiDeJouer==NB_JOUEURS){
                //envoit un signal au joueur pour lui dire de demander la carte et jouer
                kill(memoryShared->idProcessus[aQuiDeJouer],SIGUSR1);
                memoryShared->aQuiDeJouer=1; // Remise en place du joeur
                printf("DEBUG : joueur suivant  %d\n",memoryShared->aQuiDeJouer);
            }else{
                kill(memoryShared->idProcessus[aQuiDeJouer],SIGUSR1);

                memoryShared->aQuiDeJouer++; // Remise en place du joeur
                printf("DEBUG : joueur suivant  %d\n",memoryShared->aQuiDeJouer);
            }
        }


        detachSharedMemory(memoryShared);
        sem_post(semProtectSharedMemory);// Fin de zone critique

        printf("INFO : ping 10 seconde\n");
        sleep_perso(7);

    }

    pthread_exit(0);
}


/**
 * \fn vvoid *functionThreadMaitre(void *arg){
 * \brief Cette fonction est déclenché par le premier thread du programme. Il permet d'attendre que NB_JOUEURS se connectent à la partie.
 * Le temps
 * \param void *arg les paramatres du thread
*/
void *functionThreadMaitre(){
    int flag = -1, timeout = 0 ;
    struct data_t *memoryShared;
    printf("INFO : recuperation du segment de memoire.\n");
    while (flag == -1 && timeout <= 10 ){
        memoryShared=getSharedMemory(1056);
        if( memoryShared->nbCurrentUser == NB_JOUEURS){
            flag=0;
            memoryShared->start=1;
            pthread_cond_signal(&cond);
        }else{
            printf("INFO : en attente des joueurs\n");
        }
        detachSharedMemory(memoryShared);
        timeout++;
        sleep(3);
    }
    pthread_exit(0);
}
