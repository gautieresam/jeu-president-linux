
#include "libClient.h"


/***************************************************************************************
 * Partie Serveur Gautier
 ***************************************************************************************/

/**
 * \fn void * createSharedMemory(int *cle){
 * \brief Cette fonction permet de crééer une memoire partagée.
 * \param int *cle qui est la clé de mon espace de memoire partagée
* **/
void createSharedMemory(int cle){
    int shmid;
    if((shmid = shmget((key_t)cle,sizeof(struct data_t) * 1, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        if(errno == EEXIST)
            fprintf(stderr, "Le segment de memoire partagee (cle=%d) existe deja\n", CLE);
        else
            perror("Erreur lors de la creation du segment de memoire ");
        exit(EXIT_FAILURE);
    }
    printf("Serveur : segment cree.\n");
}


/**
 * \fn void *getSharedMemory(int *entier){
 * \brief Cette fonction est déclenché par un thread conditionnel.
 * \param int *cle qui est la clé de mon espace de memoire partagée
* **/
void * getSharedMemory(int cle){

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
 * \fn void *deleteSharedMemory(int *cle)
 * \brief Cette fonction permet de supprimer une memoire partagée.
 * \param int *cle la clé de la memoire partagée
* **/
void deleteSharedMemory(int cle){
    int shmid;
    /* Recuperation du segment de memoire partagee */
    if((shmid = shmget((key_t)cle, 0, 0)) == -1) {
        perror("Erreur lors de la recuperation du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }
    /* Suppression du segment de memoire partagee */
    if(shmctl(shmid, IPC_RMID, 0) == -1) {
        perror("Erreur lors de la suppression du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }
}


