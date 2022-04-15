#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define AUDIODEV	"/dev/dsp"
#define swap_short(x)		(x)è-
#define swap_long(x)		(x)

#include <stdint.h>
#include <sys/soundcard.h>
#include "./../src/audio.h"

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "./struc.h"
#include "ctype.h"

int main(int argc, char *argv[])
{
    //vérification des fonctions
    int fileDescriptor, erreurSendTo;
    //addresses
    struct sockaddr_in dest, from;
    socklen_t flen = sizeof(struct sockaddr_in);
    //Messages
    firstMessage msgTo;
	bufferSound bufferMessage;
    int ack = 1;
    //Timeout
    int nb;
    fd_set read_set;
    struct timeval timeout;

    //On initialie le son
    int wd;
    message sound;

    int counterParam = 3;
    msgTo.volume = 0; msgTo.volumeData = 1.0;
    double speed = 1.0;

    //création du socket
    fileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if(fileDescriptor == -1 ){
        perror("erreur de création du socket"); 
        return (EXIT_FAILURE);
    } 

    //on initialie l'addresse du serveur
    memset(&dest, 0, sizeof(struct sockaddr_in));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(1235);
    dest.sin_addr.s_addr = inet_addr(argv[1]);

    while (counterParam < argc){
        if(strcmp(argv[counterParam], "volume") == 0){
            if(counterParam + 1 == argc){
                perror("Le parametre du volume est manquant | format: [xx](\%)"); 
                return (EXIT_FAILURE);
            } else if(!isdigit(*argv[counterParam + 1])){
                perror("Le format attendu du parametre volume est un nombre décimal positif"); 
                return (EXIT_FAILURE);
            }
            printf("volume : %s%%\n", argv[counterParam + 1]);
            fflush(stdout);
            msgTo.volume = 1;
            msgTo.volumeData = atof(argv[counterParam + 1]) / 100.0f;
            counterParam+=2;
        } else if(strcmp(argv[counterParam], "speed") == 0){
            if(counterParam + 1 == argc){
                perror("Le parametre du volume est manquant | format: [xx](\%)"); 
                return (EXIT_FAILURE);
            } else if(!isdigit(*argv[counterParam + 1])){
                perror("Le format attendu du parametre volume est un nombre décimal positif"); 
                return (EXIT_FAILURE);
            }
            printf("Vitesse : %d%%\n", (int)atof(argv[counterParam + 1]));
            fflush(stdout);
            speed = atof(argv[counterParam + 1]) / 100.0;
            counterParam+=2;
        } else if(strcmp(argv[counterParam], "mono") == 0){
            msgTo.mono = 1;
            counterParam++;
        } else {
            perror("Parametre non pris en charge"); 
            return (EXIT_FAILURE);
        }
    }

    //On envoi le nom de la musique
    memset(msgTo.fileName, 0, sizeof msgTo.fileName);
    strcpy(msgTo.fileName, argv[2]);
    erreurSendTo = sendto(fileDescriptor, &msgTo, sizeof(msgTo), 0, (struct sockaddr*) &dest, flen);
    if(erreurSendTo<0) {
        perror("erreur de sendTo - 1"); 
        return (EXIT_FAILURE);
    }

    FD_ZERO(&read_set);
    FD_SET(fileDescriptor, &read_set);
    timeout.tv_sec  = 0;
    timeout.tv_usec = 2000000;

    nb = select(fileDescriptor + 1, &read_set, NULL, NULL, &timeout);
    if(nb<0){
        perror("err: select"); 
        return (EXIT_FAILURE);
    }
    if(nb==0){
        perror("Délai d'attente dépassé"); 
        return (EXIT_FAILURE);
    }
    if(FD_ISSET(fileDescriptor, &read_set)){           
        //On attend la réponse du serveur: le son ou une erreur
        int len = recvfrom(fileDescriptor, &sound, sizeof(sound), 0, (struct sockaddr*) &from, &flen);
        if (len<0) {
            perror("Le message reçu est incorrecte"); 
            return (EXIT_FAILURE);
        } 
    }

    if(sound.error == 1){
        printf("%s", sound.errorMessage);
        return EXIT_FAILURE;
    } 

    if(msgTo.mono == 1){
        sound.audio.channels = 1;
    }

    wd = aud_writeinit(sound.audio.sample_rate * speed, sound.audio.sample_size, sound.audio.channels);


    erreurSendTo = sendto(fileDescriptor, &ack, sizeof(ack)+1, 0, (struct sockaddr*) &from, sizeof(struct sockaddr_in));
    if(erreurSendTo<0) {
        perror("erreur de sendTo - 2"); 
        return (EXIT_FAILURE);
    }
                  
    while (true){
        FD_ZERO(&read_set);
        FD_SET(fileDescriptor, &read_set);
        timeout.tv_sec  = 0;
        timeout.tv_usec = 2000000;

        nb = select(fileDescriptor + 1, &read_set, NULL, NULL, &timeout);
        if(nb<0){
            perror("err: select"); 
            return (EXIT_FAILURE);
        }
        if(nb==0){
            perror("Délai d'attente dépassé"); 
            return (EXIT_FAILURE);
        }

        if(FD_ISSET(fileDescriptor, &read_set)){   
            //On attend la fin de la lecture du bout envoyé pour envoyer la suite
            int len = recvfrom(fileDescriptor, &bufferMessage, sizeof(bufferMessage), 0, (struct sockaddr*) &from, &flen);
            if (len<0) {
                perror("Le message reçu est incorrecte"); 
                return (EXIT_FAILURE);
            }
        }

        if(msgTo.mono == 1){
            write(wd, bufferMessage.bufferMono, sizeof(bufferMessage.bufferMono));
        } else {
            write(wd, bufferMessage.buffer, sizeof(bufferMessage.buffer));
        }

        erreurSendTo = sendto(fileDescriptor, &ack, sizeof(ack)+1, 0, (struct sockaddr*) &from, sizeof(struct sockaddr_in));
        if(erreurSendTo<0) {
            perror("erreur de sendTo - 3"); 
            return (EXIT_FAILURE);
        }

        if(bufferMessage.data == 0){
            break;
        } 
    };

    close(fileDescriptor);
}