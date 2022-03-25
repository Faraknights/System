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

int main(int argc, char *argv[]) {
    
    //vérification des fonctions
    int fileDescriptor, erreurBind, erreurSendTo;
    //addresses
    struct sockaddr_in my_addr, from;
    socklen_t flen = sizeof(struct sockaddr_in);
    //Messages
    char msgFrom[64];
    struct bufferSound msgTo;
    int ack;

    //On initialie le son
    int rd, dataMusic;
    struct message sound;
    sound.audio.channels = 0;
    sound.audio.sample_size = 0;
    sound.audio.sample_rate = 0;
    
    //création du socket
    fileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if(fileDescriptor == -1 ){
        perror("erreur de création du socket"); 
        return (EXIT_FAILURE);
    } 

    //on initialie l'addresse du serveur
    memset(&my_addr, 0, sizeof(struct sockaddr_in));
    my_addr.sin_family = AF_INET; //udp
    my_addr.sin_port = htons(1234); //port
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY); //@ip = ip du serveur

    //On fait une liaison entre le serveur (l'adresse) et le socket
    erreurBind = bind(fileDescriptor, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in));
    if (erreurBind<0) {
        perror("erreur de création du socket"); 
        return (EXIT_FAILURE);
    } 
    

    printf("Ecoute...");
    while(true){
        //On attend de recevoir un message d'un client avec le nom d'une music
        int len = recvfrom(fileDescriptor, msgFrom, sizeof(msgFrom), 0, (struct sockaddr*) &from, &flen);
        if (len<0) {
            perror("Le message reçu est incorrecte"); 
            return (EXIT_FAILURE);
        }

        while (true)
        {
            if(access(msgFrom, F_OK) == -1){
                //Si le fichier n'existe pas
                sound.error = 1;
                memset(sound.errorMessage, 0, sizeof sound.errorMessage);
                sprintf(sound.errorMessage, "err: Le fichier n'existe pas");
                erreurSendTo = sendto(fileDescriptor, &msgTo, sizeof(msgTo)+1, 0, (struct sockaddr*) &from, sizeof(struct sockaddr_in));
                break;
            } else {
                //Si le fichier existe
                rd = aud_readinit(msgFrom, &sound.audio.sample_rate, &sound.audio.sample_size, &sound.audio.channels);
                sound.error = 0;

                //On envoie la structure du son au client
                erreurSendTo = sendto(fileDescriptor, &sound, sizeof(sound)+1, 0, (struct sockaddr*) &from, sizeof(struct sockaddr_in));
                if(erreurSendTo<0) {
                    perror("erreur de sendTo"); 
                    return (EXIT_FAILURE);
                }
            }

            //On attend la réponse du client
            int len = recvfrom(fileDescriptor, &ack, sizeof(ack), 0, (struct sockaddr*) &from, &flen);
            if (len<0) {
                perror("Le message reçu est incorrecte"); 
                return (EXIT_FAILURE);
            }
            
            do
            {
                //On lit un bout du fichier
                dataMusic = read(rd, msgTo.buffer, sizeof(msgTo));
                msgTo.data = dataMusic;

                //On l'envoi au client
                erreurSendTo = sendto(fileDescriptor, &msgTo, sizeof(msgTo)+1, 0, (struct sockaddr*) &from, sizeof(struct sockaddr_in));
                if(erreurSendTo<0) {
                    perror("erreur de sendTo"); 
                    return (EXIT_FAILURE);
                }

                if(dataMusic != 0){
                    //On attend la fin de la lecture du bout envoyé pour envoyer la suite
                    int len = recvfrom(fileDescriptor, &ack, sizeof(ack), 0, (struct sockaddr*) &from, &flen);
                    if (len<0) {
                        perror("Le message reçu est incorrecte"); 
                        return (EXIT_FAILURE);
                    }
                }
            } while (dataMusic != 0);
        }
    }  

    close(fileDescriptor);
}