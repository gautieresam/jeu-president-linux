
#define _XOPEN_SOURCE
#include <stdio.h>      /* Pour printf, perror */
#include <stdlib.h>     /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/shm.h>    /* Pour shmget, shmat, shmdt */
#include <errno.h>      /* Pour errno */
#include "gestion_jeu.h"



#ifndef _STRUCTURES_
#define _STRUCTURES_


/* Cle utilisee pour le segment de memoire partagee */
#define CLE 1056

#endif

int main() {
    int shmid, i, *debut;
    void *adresse;
    data_t data;

    /** Recuperation du segment de memoire partagee **/
    if((shmid = shmget((key_t)CLE, 0, 0)) == -1) {
        perror("Erreur lors de la recuperation du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }
    printf("Client : recuperation du segment de memoire.\n");

    /* Attachement du segment de memoire partagee */
    if((adresse = shmat(shmid, NULL, 0)) == (void*)-1) {
        perror("Erreur lors de l'attachement du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }

    /* Lecture des 10 entiers du segment de memoire partagee */
    debut = adresse;
    printf("Client : entiers lus = [");





    /* Detachement du segment de memoire partagee */
    if(shmdt(adresse) == -1) {
        perror("Erreur lors du detachement du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}