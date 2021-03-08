/**
 * @file
 * @author  Gautier Masse <gautieresam@gmail.com> <gautier.masse@ig2i.centralelille.fr>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Exercice creation des joueurs qui sont des processus  
 * 
 * compile gcc test_creation_joueurs.c -o exe -lpthread
 * 
 *
 */


#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>


// Declaration des macros : 
#define TAILLE_JEU_DE_CARTE 52 // 32 ou 52 
#define NB_JOUEURS 4
#define CHECK(sts,msg) if ((sts) == -1 )  { perror(msg);exit(-1);}
int kill(pid_t pid, int sig);


/**
 * \fn void Initalisation_du_jeu_de_carte(int tab[52])
 * \brief Fonction d'initailisation du jeu. 1=> as 2=>2 11=>valet 12=>dame 13=>roi
 *
 * \param tab le tableau à remplir avec les cartes.
 * \return passage par adresse donc pas de return 
 */
void Initalisation_du_jeu_de_carte(int tab[TAILLE_JEU_DE_CARTE]){
 
   printf("FONCTION : INITIALISATION DU JEU DE CARTE :\n");

   // Declaration des variables 
   int emplacement=1; 

   for (int i = 0; i <TAILLE_JEU_DE_CARTE-3; i=i+4 )
   {
      for (int k = 0; k <=3; k++)
      {
         tab[i+k]=emplacement;
      }
      emplacement++;
   }
}




/**
 * \fn void afficher_le_jeu_de_carte(int tab[52]){
 * \brief Fonction d'affichage du jeu de carte.
 *
 * \param tab le tableau à remplir avec les cartes.
 * \return pas de return 
 */
void afficher_le_jeu_de_carte(int jeu_de_carte[TAILLE_JEU_DE_CARTE]){

   printf("FONCTION : AFFICHER LE JEU DE CARTE :\n");
   int i=0; 
   for (int i = 0; i < TAILLE_JEU_DE_CARTE; i++){
      printf("%d %d - \n",i,jeu_de_carte[i]);
   }
}




/**
 * \fn void distribution_paire(){
 * \brief Fonction de repartition des cartes si le nombre de joueurs
 *  est paire 
 * \ 
 * \param jeu_de_carte.
 * \return pas de return 
*/
void distribution_paire(int jeu_de_carte[TAILLE_JEU_DE_CARTE]){
   
   printf("DEBUG paire\n");

   int taille_de_la_main=TAILLE_JEU_DE_CARTE/NB_JOUEURS;

   printf("La main est de %d\n",taille_de_la_main);
}




void distribution_impaire(int jeu_de_carte[TAILLE_JEU_DE_CARTE]){
   printf("impaire\n");
}


/**
 * \fn choix_repartition_des_cartes(jeu_de_carte)
 * \brief Fonction de repartition des cartes cette fonction va 
 * \ appeler distribution_paire ou bien distibution_impaire en 
 * \ fonction du nombre de joueurs !
 * \ 
 * \param tab le tableau à melanger.
 * \return pas de return 
*/
void choix_repartition_des_cartes(int jeu_de_carte[TAILLE_JEU_DE_CARTE]){

   printf("FONCTION : REPARTITION DU JEU DE CARTE :\n");

   int choix=0;
   NB_JOUEURS % 2 == 0 ? distribution_paire(jeu_de_carte) : distribution_impaire(jeu_de_carte);

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
    //mélanger le jeu
    for (int i = 0; i < TAILLE_JEU_DE_CARTE; i++){
      //echanger la case d'indice i avec une case random du tableau
      temp=jeu_de_carte[i];
      alea=rand() % (TAILLE_JEU_DE_CARTE);
      jeu_de_carte[i]=jeu_de_carte[alea];
      jeu_de_carte[alea]=temp;
   }
}

int taille_de_la_main(){
    int taille = TAILLE_JEU_DE_CARTE/NB_JOUEURS;
    return taille;
}


// Main 
int main(int argc, char const *argv[])
{

   // Declaration des variables 
   int status;
   int joueur_1,joueur_2;
   int jeu_de_carte[(TAILLE_JEU_DE_CARTE)]={0};  

   //afficher_le_jeu_de_carte(jeu_de_carte);
   Initalisation_du_jeu_de_carte(jeu_de_carte);
   afficher_le_jeu_de_carte(jeu_de_carte);
   choix_repartition_des_cartes(jeu_de_carte);
   melanger_cartes(jeu_de_carte);
   afficher_le_jeu_de_carte(jeu_de_carte);

   int main = taille_de_la_main();
  printf("%d",main);



   // Creation ID partie 
   


   // Creéation des joueurs : 
   // To do : faire une fonction pour la creation des joueurs. 
   //joueur_1 = fork();
   //joueur_2 = fork();




/*
   switch(joueur_1)

   {
      case -1:
         CHECK(fork(),"Ne peut pas créer un noouveau processus");
         break;
      
      case 0:
         
         printf("Je suis le processus fils %d, mon pere est le %d \n",getpid(),getppid());
         
   
         break;
   
   default:
      
      printf("Je suis le processus pere %d, mon pere est le %d \n",getpid(),getppid());

      // Attendre que le processus fils à fini son taf ! 
      waitpid(joueur_1,0,0); 

      int exit_status = WEXITSTATUS(status);  

      if (WIFEXITED(status)) {
         printf("terminé, code=%d\n", WEXITSTATUS(status));
      
      } else if (WIFSIGNALED(status)) {
         printf("tué par le signal %d\n", WTERMSIG(status));
      
      } else if (WIFSTOPPED(status)) {
         printf("arrêté par le signal %d\n", WSTOPSIG(status));
      
      } else if (WIFCONTINUED(status)) {
         printf("relancé\n");
      }


   } 
*/

   return 0;
}
