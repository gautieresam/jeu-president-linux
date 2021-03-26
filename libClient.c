
#include "libClient.h"

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



