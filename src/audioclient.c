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

int main(int argc, char *argv[])
{
    //vérification des fonctions
    int fileDescriptor, erreurSendTo;
    //addresses
    struct sockaddr_in dest, from;
    socklen_t flen;
    //Messages
    char msgTo[64];
    struct bufferSound msgFrom;
    int ack = 1;

    //On initialie le son
    int wd, dataMusic;
    struct message sound;

    //création du socket
    fileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if(fileDescriptor == -1 ){
        perror("erreur de création du socket"); 
        return (EXIT_FAILURE);
    } 

    //on initialie l'addresse du serveur
    memset(&dest, 0, sizeof(struct sockaddr_in));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(1234);
    dest.sin_addr.s_addr = inet_addr("127.0.0.1");

    //On envoi le nom de la musique
    *msgTo = argv[2];
    erreurSendTo = sendto(fileDescriptor, msgTo, strlen(msgTo)+1, 0, (struct sockaddr*) &dest, flen);
    if(erreurSendTo<0) {
        perror("erreur de sendTo"); 
        return (EXIT_FAILURE);
    }
                
    //On attend la réponse du serveur: le son ou une erreur
    int len = recvfrom(fileDescriptor, &sound, sizeof(sound), 0, (struct sockaddr*) &from, &flen);
    if (len<0) {
        perror("Le message reçu est incorrecte"); 
        return (EXIT_FAILURE);
    } 

    if(sound.error == 1){
        printf(sound.errorMessage);
        return EXIT_FAILURE;
    } 

    int wd = aud_writeinit(sound.audio.sample_rate, sound.audio.sample_size, sound.audio.channels);


    erreurSendTo = sendto(fileDescriptor, &ack, sizeof(ack)+1, 0, (struct sockaddr*) &from, sizeof(struct sockaddr_in));
    if(erreurSendTo<0) {
        perror("erreur de sendTo"); 
        return (EXIT_FAILURE);
    }
                  
    do
    {
        //On attend la fin de la lecture du bout envoyé pour envoyer la suite
        int len = recvfrom(fileDescriptor, &msgFrom, sizeof(msgFrom), 0, (struct sockaddr*) &from, &flen);
        if (len<0) {
            perror("Le message reçu est incorrecte"); 
            return (EXIT_FAILURE);
        }

        write(wd, msgFrom.buffer, sizeof(msgFrom.buffer));

        erreurSendTo = sendto(fileDescriptor, &ack, sizeof(ack)+1, 0, (struct sockaddr*) &from, sizeof(struct sockaddr_in));
        if(erreurSendTo<0) {
            perror("erreur de sendTo"); 
            return (EXIT_FAILURE);
        }
                
    } while (msgFrom.data != 0);

    close(fileDescriptor);
}