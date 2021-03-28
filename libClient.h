#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <errno.h>

#define TAILLE_JEU_DE_CARTE 52 // 32 ou 52
#define NB_JOUEURS 2
#ifndef _STRUCTURES_
#define _STRUCTURES_
#define CLE 1056
#endif

struct data_t{
    int jeu_de_carte[(TAILLE_JEU_DE_CARTE)];
    int partie[(TAILLE_JEU_DE_CARTE)];
    int joueurs[3][NB_JOUEURS];
    int start;
    int nbCurrentUser;
    int aQuiDeJouer;
    int idProcessus[4];
} ;

void *getSharedMemory(int cle);
void detachSharedMemory(struct data_t *memoryShared);
void createSharedMemory(int cle);
int give_taille_de_la_main();
int indice_partie(int partie[TAILLE_JEU_DE_CARTE]);