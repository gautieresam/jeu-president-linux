# president-linux-processus-

Projet LA1 le jeu de président avec des echanges interprocessus, le but du projet est de mettre en evidence les echanges inter-process

## Comment compiler ? 
Il faut utiliser la commande dans un terminal linux make dans l'arborescence du fichier
Cette commande va produire l'excutable serveur et client

## Comment lancer une partie ? 
Il faut ouvrir 3 terminaux, le premier pour lancer le serveur avec la commande ./server et les 2 autres pour lancer le client avec ./client

## Comprendre les rêgles du président ? 

Nous avons déposé sur votre drive un diaporama qui explique les rêgles
https://docs.google.com/presentation/d/1O3rvtm5myppeVxDMuuivvZER_8mwOIdeVDPbRXCet0U/edit?usp=sharing 

## Si vous souhaitez voir une partie nous avons réalisé une vidéo dispo sur votre drive dans notre répertoire

# Erreur mémoire 
Il faut supprimer le segment de mémoire partagée avec ./clean à pour clé 1056 

# Rapport de bug 
Comme évoqué sur discord nous avons essayé de corriger le problème si un utilisateur ne rentre pas sa carte dans un scanf il y a des problèmes d'affichage. Nous avons adapté le morceau de code que vous nous avez donné. Il permet de garder un affichage presque propre dans le terminal. 

