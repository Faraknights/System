Programme fonctionnel.
Le client arrive à recevoir les informations de la musique, et la lire en temps réel.
Le serveur peut informer un autre client qu'il est occupé.
La gestion des timeout a été mise en place.

Filtres :
- volume en rajoutant 2 paramètres, le premier "volume", le deuxième un entier positif ex :
padsp ./bin/audioclient 127.0.0.1 ./src/test.wav volume 50
- mono en rajoutant 1 paramètre "mono"
(Le son est parasité, je n'arrive pas à trouver la source du problème)
padsp ./bin/audioclient 127.0.0.1 ./src/test.wav mono

__Executer__

- Aller sur project/
- exécuter: make client_server
- exécuter: ./bin/server
- aller sur un autre terminal et aller sur projet
- exécuter: padsp ./bin/client 127.0.0.1 ./src/test.wav

__arborescence__
> project
    > bin
        > client
        > server
    > obj
        > audio.o
        > lecteur.o
        > server.o
    > src
        > audio.c
        > audio.h
        > audioclient.c
        > audioserver.C
        > struc.h
        > test.wav
    > Makefile
    > README.md