Programme fonctionnel.
Le client arrive à recevoir les informations de la musique, et la lire en temps réel.
Le serveur peut informer un autre client qu'il est occupé.
La gestion des timeout a été mise en place.

Filtres :
- volume en rajoutant 2 paramètres, le premier "volume", le deuxième un entier positif ex :
> padsp ./bin/audioclient 127.0.0.1 ./src/test.wav volume 50
- mono en rajoutant 1 paramètre "mono"
(Le son est parasité/saturé, je n'arrive pas à trouver la source du problème)
> padsp ./bin/audioclient 127.0.0.1 ./src/test.wav mono
- Echo : J'ai essayé un bon moment de faire ce filtre avec différent processus. Le processus père jouait, le processus fils attendait le délai indiqué en paramètre et jouait à son tour avant de s'arrêter. Or je tombais sur des erreures de ce type : Assertion 'pa_atomic_load(&(b)->_ref) > 0' failed at pulsecore/memblock.c:682, function pa_memblock_unref(). Aborting.
J'ai donc abandonné ce filtre et choisi malheureusement la facilité pour la vitesse de lecture...
- speed en rajoutant 2 paramètres, le premier "speed", le deuxxième un entier positif (pourcentage de la vitesse initiale)  ex: 
> padsp ./bin/audioclient 127.0.0.1 ./src/test.wav speed 200

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