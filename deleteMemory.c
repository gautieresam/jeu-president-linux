/**
 * Ce programme permet de supprimer le segment de memoire partagee cree par
 * le programme 'memoireServeur'.
 * @author Cyril Rabat
 * @version 23/02/2016
 **/
#define _XOPEN_SOURCE

#include <stdio.h>      /* Pour printf, perror */
#include <stdlib.h>     /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/shm.h>    /* Pour shmget, shmctl */


#ifndef _STRUCTURES_
#define _STRUCTURES_

/* Cle utilisee pour le segment de memoire partagee */
#define CLE 1056

#endif

int main() {
    int shmid;

    /* Recuperation du segment de memoire partagee */
    if((shmid = shmget((key_t)CLE, 0, 0)) == -1) {
        perror("Erreur lors de la recuperation du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }

    /* Suppression du segment de memoire partagee */
    if(shmctl(shmid, IPC_RMID, 0) == -1) {
        perror("Erreur lors de la suppression du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }

    printf("Segment de memoire partagee supprime.\n");

    return EXIT_SUCCESS;
}