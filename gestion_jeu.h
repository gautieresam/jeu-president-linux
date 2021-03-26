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
 * \param tableau jeu_de_carte, contient le jeu de carte mélangé.
 * \param entier start, variable 0 ou 1  pour les nouveux joueurs si ils peuvent jouer à la partie ou non.
 * \param tableau partie, contient les cartes posés par les joueurs.
 * \param entier nbCurrentUser,variable pour connaitre le nombre de joueur dans la partie.
 * \param entier aQuiDeJouer,variable 1,2,3,4 pour connaitre le joueur qui doit jouer.
*/
struct data_t{

    int jeu_de_carte[(TAILLE_JEU_DE_CARTE)];
    int partie[(TAILLE_JEU_DE_CARTE)];
    int joueurs[3][NB_JOUEURS];
    int start;
    int nbCurrentUser;
    int aQuiDeJouer;
    int idProcessus[4];
} ;

/**
 * \brief Prototype des fonctions côté serveur
 **/
void *functionThreadMaitre() ;
void * functionThreadPartie(void *pVoid) ;
void *getSharedMemory(int cle);
void detachSharedMemory(struct data_t *memoryShared);
void createSharedMemory(int cle);



/**
 * \brief Prototype des fonctions côté client
 **/
void initalisation_du_jeu_de_carte(int tab[TAILLE_JEU_DE_CARTE]);
void afficher_tab(int jeu_de_carte[TAILLE_JEU_DE_CARTE]);
void melanger_cartes(int jeu_de_carte[TAILLE_JEU_DE_CARTE]);
int give_taille_de_la_main();
int indice_partie(int partie[TAILLE_JEU_DE_CARTE]);
int get_derniere_carte(int jeu_de_carte[TAILLE_JEU_DE_CARTE]);
void afficher_carte_joueur(int numJoueur, int jeu_de_carte[TAILLE_JEU_DE_CARTE]);
int jouer_une_carte(int numJoueur, int carte, int jeu_de_carte[(TAILLE_JEU_DE_CARTE)], int partie[(TAILLE_JEU_DE_CARTE)]);
void afficher_tab_joueurs(int joueurs[2][NB_JOUEURS]);
void remplir_tab_joueurs(int joueurs[2][NB_JOUEURS]);

/*
void set_joueur_inactif(int numJoueur,int joueurs[2][NB_JOUEURS]);
int get_status_joueurs(int numJoueur,int joueurs[2][NB_JOUEURS]);
void set_all_joueurs_actifs(int joueurs[2][NB_JOUEURS]);
int combien_joueurs_actifs(int joueurs[2][NB_JOUEURS]);
*/