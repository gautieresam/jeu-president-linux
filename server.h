#include <sys/shm.h>    /* Pour shmget, shmat, shmdt */
#include <errno.h>      /* Pour errno */
#include <sys/stat.h>   /* Pour S_IRUSR, S_IWUSR */

#ifndef _STRUCTURES_
#define _STRUCTURES_

/* Cle utilisee pour le segment de memoire partagee */
#define CLE 1056
#endif



void *functionThreadMaitre(void *arg);

void *functionThreadPartie(void *arg);