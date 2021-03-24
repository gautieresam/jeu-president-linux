/**

 * HOW TO COMPILE SERVER/CLIENT ?
 * gcc server.c -o server -lpthread
 * RUN with ./server and ./cleint
 *
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




    /* Suppression du segment de memoire partagee */
    /* Demander à l'utilisateur */
    /*
    printf("Taper sur 0 pour supprimer le segment de memoire ! ");
    int deleteMemoryShared=-1;
    scanf("%d", &deleteMemoryShared);
    if(deleteMemoryShared == 0){
        if(shmctl(shmid, IPC_RMID, 0) == -1) {
            perror("Erreur lors de la suppression du segment de memoire partagee ");
            exit(EXIT_FAILURE);
        }
    }
    printf("Segment de memoire partagee supprime.\n");
*/





    return EXIT_SUCCESS;
}