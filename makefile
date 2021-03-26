all : gestion_jeu.o main.c
	gcc -Wall -Wextra gestion_jeu.o  main.c -o server -lpthread
gestion_jeu.o : gestion_jeu.c gestion_jeu.h
	gcc -Wall -Wextra -c gestion_jeu.c -lpthread