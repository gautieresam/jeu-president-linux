#include "gestion_jeu.h"
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


int kill(pid_t pid, int sig);



int main(){

    /**
    * \brief Creation de la memoire partagé qui est une structure data_t
    * \ Attention il faut jouer ./clean si la memoire partagé n'a pas été supprimé !
    * \param la clé utilisé est 1056
    * **/
    struct data_t *memoryShared;

    createSharedMemory(1056);
    memoryShared=getSharedMemory(1056);

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

    /**
     * Supprimer la memoire partagée
     * **/
    detachSharedMemory(memoryShared);

    return 0;
}

/**
 * \fn void *functionThreadPartie(void *arg) {
 * \brief Cette fonction est déclenché par un thread conditionnel.
 * Ce thread permet la communication avec les clients.
 * \param void *arg les paramatres du thread
* **/
void * functionThreadPartie(void *pVoid) {

    pthread_cond_wait(&cond,&unMutex);
    printf("INFO : declenchement de la partie\n");

    // Creation d'un tube nommé pour communiquer avec un client dans un sens.
    if ( mkfifo("serverToJ1", 0666) == -1  ) { // Retourne 0 en cas de succès, -1 en cas d’échec
        printf("INFO: echec de la création du tube serverToJ1");
    }
    //open("serverToJ1", O_WRONLY); // ouverture en écriture seule

    struct data_t *memoryShared;
    memoryShared=getSharedMemory(1056);

    for (int i = 1; i <= NB_JOUEURS ; i++) {
        printf("INFO : joueur %d - PID %d \n",i,memoryShared->idProcessus[i]);

        // Envoyer le signal SIGUSR2 au processus
        kill(memoryShared->idProcessus[i],SIGUSR1);

    }

    detachSharedMemory(memoryShared);
    unlink("serverToJ1"); // Suppression du tube nommé serverToJ1

    pthread_exit(0);
}


/**
 * \fn vvoid *functionThreadMaitre(void *arg){
 * \brief Cette fonction est déclenché par le premier thread du programme. Il permet d'attendre que 4 personnes se connectent à la partie.
 * \param void *arg les paramatres du thread
* **/
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
            printf("En attente des joueurs\n");
        }
        detachSharedMemory(memoryShared);
        timeout++;
        sleep(3);
    }
    pthread_exit(0);
}
