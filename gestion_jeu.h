#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>



#define TAILLE_JEU_DE_CARTE 52 // 32 ou 52
#define NB_JOUEURS 4


void *joueur(void *arg);

//fonctions du jeu général
void initalisation_du_jeu_de_carte(int tab[TAILLE_JEU_DE_CARTE]);

/*
void afficher_tab(int jeu_de_carte[TAILLE_JEU_DE_CARTE]);
void melanger_cartes(int jeu_de_carte[TAILLE_JEU_DE_CARTE]);
int give_taille_de_la_main();
int indice_partie(int partie[TAILLE_JEU_DE_CARTE]);
int get_derniere_carte(int jeu_de_carte[TAILLE_JEU_DE_CARTE]);


//fonctions de jeu d'un joueur
void afficher_carte_joueur(int numJoueur, int jeu_de_carte[TAILLE_JEU_DE_CARTE]);
int jouer_une_carte(int numJoueur, int carte, int jeu_de_carte[(TAILLE_JEU_DE_CARTE)], int partie[(TAILLE_JEU_DE_CARTE)]);


//gestion joueurs
 */
void afficher_tab_joueurs(int joueurs[2][NB_JOUEURS]);


//void remplir_tab_joueurs(int joueurs[2][NB_JOUEURS]);

/*
void set_joueur_inactif(int numJoueur,int joueurs[2][NB_JOUEURS]);
int get_status_joueurs(int numJoueur,int joueurs[2][NB_JOUEURS]);
void set_all_joueurs_actifs(int joueurs[2][NB_JOUEURS]);
int combien_joueurs_actifs(int joueurs[2][NB_JOUEURS]);
*/