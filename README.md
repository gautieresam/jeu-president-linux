# president-linux-processus-

Projet LA1 le jeu de président avec des echanges interprocessus

## Comment compiler ? 

Il faut utiliser la commande dans un terminal linux make dans l'arborescence du fichier

Cette commande va produire l'excutable serveur et client

## Comment lancer une partie ? 

Il faut ouvrir 3 terminaux, le premier pour lancer le serveur avec la commande ./server et les 2 autres pour lancer le client avec ./client


# Architecture 

1. Le serveur créer une memoire partagée pour les clients

   1. Lancement du client 
   2. Le client va regarde la memoire partagée si il y a de la place et si la partie n'est pas commencé
   3. Si la partie est pleine ou commencé, le client est viré
   4. Si la partie n'est pas pleine, le client va s'enregistrer dans la memoire partagée avec son PID et prendre un numéro de joueur et attendre les instructions


2. Le thread functionThreadMaitre va attendre X joueurs pour lancer le thread functionThreadPartie
   
    1. Le serveur va utiliser les signaux pour sinifier au client de jouer 
    2. Une alerme de 3 secondes va être mise pour que le joueur puisse poser sa carte
    3. Si il ne répond pas ou ne pause pas de carte son tour passe ! 
    4. Au bout des 3 secondes il va écrire dans la memoire partagée la carte 



3. Dev à faire 
   1. Ajouter un tableau avec le nombre de carte de chaque joueur dans les fichiers pour la structure. 
   Exemple 
