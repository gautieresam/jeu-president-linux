/**
 * Ce programme vise a illustrer le fonctionnement des segments de memoire
 * partagee IPC System V. Ce programme cree un segment de memoire partagee et
 * y place 10 entiers.
 * @author Cyril Rabat
 * @version 14/03/2016
 **/
#define _XOPEN_SOURCE

#include <stdio.h>      /* Pour printf, perror */
#include <stdlib.h>     /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/shm.h>    /* Pour shmget, shmat, shmdt */
#include <errno.h>      /* Pour errno */
#include <sys/stat.h>   /* Pour S_IRUSR, S_IWUSR */

#ifndef _STRUCTURES_
#define _STRUCTURES_

/* Cle utilisee pour le segment de memoire partagee */
#define CLE 1056

#endif

int main() {
    int shmid, i, *debut;
    void *adresse;

    /* Creation d'un segment de 10 entiers */
    if((shmid = shmget((key_t)CLE, sizeof(int) * 10, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        if(errno == EEXIST)
            fprintf(stderr, "Le segment de memoire partagee (cle=%d) existe deja\n", CLE);
        else
            perror("Erreur lors de la creation du segment de memoire ");
        exit(EXIT_FAILURE);
    }
    printf("Serveur : segment cree.\n");

    /* Attachement du segment de memoire partagee */
    if((adresse = shmat(shmid, NULL, 0)) == (void*)-1) {
        perror("Erreur lors de l'attachement du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }

    /* Placement des entiers dans le segment de memoire partagee */
    debut = adresse;
    for(i = 0; i < 10; i++) {
        *debut = i * 2;
        ++debut;
    }
    printf("Serveur : entiers places dans le segment.\n");

    /* Detachement du segment de memoire partagee */
    if(shmdt(adresse) == -1) {
        perror("Erreur lors du detachement ");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}