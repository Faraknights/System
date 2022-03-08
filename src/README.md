Programme fonctionnel.

__arborescence__
> project
    > bin
        > lecteur
    > obj
        > audio.o
        > lecteur.o
    > src
        > audio.c
        > audio.h
        > lecteur.c
        > test.wav
    > Makefile

__Executer__
aller sur project/
make lecteur
padsp ./bin/lecteur ./src/test.wav

__Questions__

1. *Que se passe-t-il si on déclare une fausse fréquence d'échantillonage à la sortie audio ? Vous pouvez essayer de passer fréquence deux fois trop importante, ou deux fois trop faible. Expliquez le phénomène obtenu.*

Ceci accélère ou ralentit la vitesse de lecture du fichier audio. 
Si l'on augmente la fréquence d'échantillonage, la piste audio se lira plus vite,
tandis que si on la diminue. La piste audio se lira moins vite.

---------------------

2. *Que se passe-t-il si vous déclarez à la sortieaudio que le fichier est mono, alors qu'il est en réalité en stéréo ? Expliquez le phénomène obtenu*

En passant de stéréo à mono. Le lecteur lit les paquets gauche droite l'un après l'autre,
au lieu de lire en même temps gauche et droite. Doublant de ce fait le temps de lecture de la piste audio.

---------------------

3. *Que se passe-t-il si vous déclarez à la sortie audio une mauvaise taile d'échantillons (8 bits à la place de 16 bits)? Expliquez le phénomène obtenu.*

Le résultat obtenu n'est pas celui escompté, car le lecteur s'attend a recevoir des informations sur 16 bits.
Or il recoit deux informations sur 8 bits en même temps.
Il fusionne alors les deux paquets afin de former d'avoir des mots de 16 bits, ce qui provoque ce grésillement.