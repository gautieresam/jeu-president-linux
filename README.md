# president-linux-processus-




# Architecture 


# Fonctionnement de la relation client serveur 

1. Le serveur créer une memoire partagée pour les clients

   1. Lancement du client 
   2. Le client va regarde la memoire partagée si il y a de la place et si la partie n'est pas commencé
   3. Si la partie est pleine ou commencé, le client est viré
   4. Si la partie n'est pas pleine, le client va s'enregistrer dans la memoire partagée et attendre les instructions


2. Le thread functionThreadMaitre va attendre X joueurs pour lancer le thread functionThreadPartie
   
    1. Le serveur va créer les tubes nommés pour communiquer avec les clients
    2. Le client va créer l'écouteur pour recevoir les messages.   
