#!/bin/bash 

gcc -Wall -Wextra -c gestion_jeu.c -lpthread
gcc -Wall -Wextra -c libClient.c -lpthread 

gcc -Wall -Wextra libClient.o client.C -o client -lpthread -fpermissive
gcc -Wall -Wextra gestion_jeu.o  main.c -o server -lpthread

