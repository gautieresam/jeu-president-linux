
int jouer_une_carte(int numJoueur, int carte, int jeu_de_carte[(TAILLE_JEU_DE_CARTE)], int partie[(TAILLE_JEU_DE_CARTE)]){

    int indice=-1,taille_main = give_taille_de_la_main(),debut=taille_main*numJoueur,fin=debut+taille_main,i=debut, flag=0,last_card=-9,carte_ok=0,indiceP=indice_partie(partie);

    printf("INFO : indice de la carte à jouer %d \n",indiceP);
    printf("INFO : function jouer_carte %d\n",carteQueUtilisateurVeutJouer);

    afficherLaGame(partie);

    /**
     * Si la carte est égal à 0, le joueur choisit de passer son tour
     * Mettre -2 dans le tableau partie pour indiquer que le joueur passe son tour !
     */
    if (carte==0 || carte == -1 ){
        printf("INFO : joueur %d passe son tour",numJoueur);
        carte_ok=-1;
    }else{

        //Verifier que la  carte existe bien dans le jeu
        while(flag==0 && i< fin){
            //printf("INFO : compare %d avec %d\n",jeu_de_carte[i], carte);
            if(jeu_de_carte[i]==carte){
                flag=1;
                indice=i;
                printf("INFO : la carte joué est dans le jeu du joueur\n");
            }
            i++;
        }
        /**
        * Si la carte n'est pas dans le jeu, procedure passer son tour
        * Mettre -2 dans le tableau partie pour indiquer que le joueur passe son tour !
        */
        if( flag == 0 ){
            printf("INFO : la carte à jouer %d n'est pas dans le jeu du joueur\n",carte);
            carte_ok=-3;
        }

        /**
        * Recuperer la derniere carte du jeu !
        */
        last_card=get_derniere_carte(partie);
        printf("INFO : la derniere carte %d\n",last_card);

        /**
        * Analyse de la partie
        */
        if(last_card == 0 || last_card == -1){ // Cas l'utilisateur est couché ou bien premiere carte
            printf("INFO : bonne carte, carte à jouer %d, la carte d'avant %d\n",carte,last_card);
            carte_ok=1; // la carte
        }else if (last_card > 2 && carte > 2){
            if (carte >= last_card){ //cas classique, mettre une carte suppérieur ou egale (numériquement parlant)
                printf("INFO : bonne carte, carte à jouer %d, la carte d'avant %d\n",carte,last_card);
                carte_ok=1;
            } else{
                printf("INFO : mauvaise carte ,carte à jouer %d, la carte d'avant %d\n",carte,last_card);
            }
        }else if (last_card > 2 && carte <= 2){ //le joueur veut jouer un as ou un 2 au dessus d'une carte entre le 3 et le roi OK

            printf("INFO : bonne carte, carte à jouer %d, la carte d'avant %d\n",carte,last_card);
            carte_ok=1;
        }else if (last_card <= 2 && carte > 2){ //le joueur veut jouer une carte au dessus d'un as ou un 2 PAS POSSIBLE
            printf("INFO : mauvaise carte ,carte à jouer %d, la carte d'avant %d\n",carte,last_card);
        }else if (last_card <= 2 && carte <= 2){  //le joueur veut jouer un as ou un 2 au dessus d'un as ou un 2
            if (last_card<=carte){
                printf("INFO : bonne carte, carte à jouer %d, la carte d'avant %d\n",carte,last_card);
                carte_ok=1;
                // Mettre if et dire que c est l utilisatuer qui doit rejouer
            }else{
                printf("INFO : mauvaise carte ,carte à jouer %d, la carte d'avant %d\n",carte,last_card);
            }
        }

        //jouer la carte
        if(flag==1 && carte_ok==1){
            //ajout la carte dans la partie (recupérer indice de jeu)
            if (indiceP >= TAILLE_JEU_DE_CARTE){
                printf("\n==>La carte saisie n'existe pas dans votre jeu\n");
                partie[indiceP]=-2;
            }
            partie[indiceP]=carte;
            jeu_de_carte[indice]=0;
        }else{
            printf("FINAL: on va jouer la carte -2 !!")
            partie[indiceP]=-2;
        }

    }




/*


    // SI tous les joueurs passe que se passee til
    // Si la carte d'avnt est couché il faut regarder la carte encore d avant

    //2. poser obligatoirement la meme carte
    //analyser les 3 dernières cartes
    // 0 1 2 3 4
    // 2 2 2 X
    if (indiceP>2 ){ // && partie[indiceP-1]==last_card

        if(partie[indiceP-1]==partie[indiceP-2] && partie[indiceP-1]==partie[indiceP-3]  ){

            // L'utilisateur va finir la plie et rejouer 3 consecutif
            carte_ok=1;
        }else if(partie[indiceP-1]==partie[indiceP-2] && partie[indiceP-2]==carte){
            // Si 2 cartes à la suite identique le joueur peut jouer
            carte_ok=1;
        }else{
            printf("toto\n");
        }
    }


 */

    return 0;

}
