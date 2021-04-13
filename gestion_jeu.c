#include "gestion_jeu.h"

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
 * \fn void *getSharedMemory(int *entier){
 * \brief Cette fonction est déclenché par un thread conditionnel.
 * \param int *cle qui est la clé de mon espace de memoire partagée
* **/
void * getSharedMemory(int cle){

    int shmid;
    struct data_t *memoryShared;

    if((shmid = shmget((key_t)cle, sizeof(struct data_t) * 1, 0)) == -1) {
        perror("Erreur lors de la recuperation du segment de memoire partagee\n");
        exit(EXIT_FAILURE);
    }
    if((memoryShared = shmat(shmid, NULL, 0)) == (void*)-1) {
        perror("Erreur lors de l'attachement du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }
    return memoryShared;
}

/**
 * \fn void * detachSharedMemory(struct data_t *memoryShared)
 * \brief Cette fonction est permet de detacher une memoire partagée.
 * \param struct data_t *memoryShared la structure à détacher
* **/
void detachSharedMemory(struct data_t *memoryShared){
    if(shmdt(memoryShared) == -1) {
        perror("Erreur lors du detachement du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }
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

/**
 * \fn void sleep_perso(double seconde)
 * \brief Fonction d'attente maison !
 * @param seconde
 */
void sleep_perso(double seconde){
    //printf("function");
    time_t time1, time2;
    time(&time1);
    time(&time2);

    while (difftime(time2,time1)<seconde){
        time(&time2);
    }
}


/**
 * \fn void Initalisation_du_jeu_de_carte(int tab[52])
 * \brief Fonction d'initailisation du jeu. 1=> as 2=>2 11=>valet 12=>dame 13=>roi
 *
 * \param tab le tableau à remplir avec les cartes.
 * \return passage par adresse donc pas de return 
 */
void initalisation_du_jeu_de_carte(int tab[TAILLE_JEU_DE_CARTE],int partie[TAILLE_JEU_DE_CARTE]) {
    // Declaration des variables
    int emplacement = 1;

    for (int i = 0; i < TAILLE_JEU_DE_CARTE - 3; i = i + 4) {
        for (int k = 0; k <= 3; k++) {
            tab[i + k] = emplacement;
        }
        emplacement++;
    }

    for (int i = 0; i < TAILLE_JEU_DE_CARTE; i++) {
        partie[i] = -1;
        printf("data %d\n",partie[i]);
    }
}

/**
 * \fn void afficher_le_jeu_de_carte(int tab[52]){
 * \brief Fonction d'affichage du jeu de carte.
 *
 * \param tab le tableau à remplir avec les cartes.
 * \return pas de return 
 */
void afficher_tab(int jeu_de_carte[TAILLE_JEU_DE_CARTE]){

   printf("\nFONCTION : AFFICHER TAB du jeu :\n");
   for (int i = 0; i < TAILLE_JEU_DE_CARTE; i++){
       if (jeu_de_carte[i]==11){
           printf("Valet - ");
       } else if (jeu_de_carte[i]== 12){
           printf("Dame - ");
       }else if (jeu_de_carte[i]== 13){
           printf("Roi - ");
       }else{
           printf("%d - ",jeu_de_carte[i]);
       }
   }
}

/**
 * \fn melanger_cartes(jeu_de_carte)
 * \brief Fonction qui va mélanger le jeu de carte (tableau)
 * \
 * \param tab le tableau à melanger.
 * \return pas de return
*/
void melanger_cartes(int jeu_de_carte[TAILLE_JEU_DE_CARTE]){
    int temp, alea;
    srand(time(NULL));
    //mélanger le jeu
    for (int i = 0; i < TAILLE_JEU_DE_CARTE; i++){
      //echanger la case d'indice i avec une case random du tableau
      temp=jeu_de_carte[i];
      alea=rand() % (TAILLE_JEU_DE_CARTE);
      jeu_de_carte[i]=jeu_de_carte[alea];
      jeu_de_carte[alea]=temp;
   }
}

int give_taille_de_la_main(){
    int taille = TAILLE_JEU_DE_CARTE/NB_JOUEURS;
    return taille;
}

/**
 * \fn void compterNombreDeCartesdUnJoueur(int numJoueur,int jeu_de_carte[TAILLE_JEU_DE_CARTE]);
 * \brief Compte le nombre de cartes du joueur en fonction de son identifiant !
 * \param int <numJoueur
 * \param int jeu_de_carte[TAILLE_JEU_DE_CARTE]
 * \return
 */
int compterNombreDeCartesdUnJoueur(int numJoueur,int jeu_de_carte[TAILLE_JEU_DE_CARTE]){
    int taille_main = give_taille_de_la_main();
    //printf("DEBUG : compterNombreDeCartesdUnJoueur, taille de la main %d\n",taille_main);
    int debut=taille_main*numJoueur;
    int fin=debut+taille_main;
    int compterNbZero=0;
    for (int i = debut; i < fin; i++){

        if (jeu_de_carte[i]==0){
            //Compte le nombre de 0
            compterNbZero++;
        }
    }
    //printf("Il reste %d cartes dans la main du joueur ! \n",taille_main-compterNbZero);
    return taille_main-compterNbZero;
}
