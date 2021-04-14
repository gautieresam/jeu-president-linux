#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/shm.h>


#define TAILLE_JEU_DE_CARTE 52 // 32 ou 52
#define NB_JOUEURS 2

#ifndef _STRUCTURES_
#define _STRUCTURES_

/**
 * \fn #define CLE 1056
 * \brief Declaration d'une clé pour la memoire partagée de la structure
 */
#define CLE 1056
#endif


/**
 * \fn sem_t *semProtectSharedMemory;
 * \brief Declaration du semaphore pour l'espace de memoire partagée
 */
sem_t *semProtectSharedMemory;


/**
 * \fn struct data_t
 * \brief sturure qui contient les informations de la partie.
 *
 * \param tableau jeu_de_carte, contient le jeu de carte mélangé (chaque joueur a un secteur de ce tableau)
 * \param tableau partie, contient les cartes posés par les joueurs.
 * \param entier start, variable 0 ou 1  pour les nouveux joueurs si ils peuvent jouer à la partie ou non (partie commencé ou non)
 * \param entier nbCurrentUser,variable pour connaitre le nombre de joueur dans la partie.
 * \param entier aQuiDeJouer,variable 1,2,3,4 pour connaitre le joueur qui doit jouer.
 * \param int idProcessus[4], tableau contenant le pid des processus
 * \param int PPID, pid du processus père
 * \param tageule[1], tableau pour la gestion du TG (case 0 du tab) et la gestion du nombres de joueurs couchés
*/
struct data_t{
    int jeu_de_carte[(TAILLE_JEU_DE_CARTE)];
    int partie[(TAILLE_JEU_DE_CARTE)];
    int start;
    int nbCurrentUser;
    int aQuiDeJouer;
    int idProcessus[4];
    int PPID;
    int tageule[1];
} ;


void sleep_perso(double seconde);
void *functionThreadMaitre() ;
void * functionThreadPartie(void *pVoid) ;
void *getSharedMemory(int cle);
void detachSharedMemory(struct data_t *memoryShared);
void createSharedMemory(int cle);
void initalisation_du_jeu_de_carte(int tab[TAILLE_JEU_DE_CARTE],int partie[TAILLE_JEU_DE_CARTE]);
void afficher_tab(int jeu_de_carte[TAILLE_JEU_DE_CARTE]);
void melanger_cartes(int jeu_de_carte[TAILLE_JEU_DE_CARTE]);
int give_taille_de_la_main();
int compterNombreDeCartesdUnJoueur(int numJoueur,int jeu_de_carte[TAILLE_JEU_DE_CARTE]);
