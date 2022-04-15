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
    printf("Lancement du serveur\n");
    fflush(stdout);
    //vérification des fonctions
    int fileDescriptor, erreurBind, erreurSendTo;
    //addresses
    struct sockaddr_in my_addr, from, activeFrom;
    socklen_t flen = sizeof(struct sockaddr_in);
    //Messages
    firstMessage msgFrom;
    bufferSound bufferMessage;
    int ack;
    //Timeout
    int nb;
    fd_set read_set;
    struct timeval timeout;

    //On initialie le son
    int rd;
    message sound;
    sound.audio.channels = 0;
    sound.audio.sample_size = 0;
    sound.audio.sample_rate = 0;

    //Si le délai d'attente est dépassé
    int cancel = 0;
    
    //création du socket
    fileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if(fileDescriptor == -1){
        perror("erreur de création du socket"); 
        return (EXIT_FAILURE);
    } 

    //on initialie l'addresse du serveur
    memset(&my_addr, 0, sizeof(struct sockaddr_in));
    my_addr.sin_family = AF_INET; //udp
    my_addr.sin_port = htons(1235); //port
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY); //@ip = ip du serveur

    //On fait une liaison entre le serveur (l'adresse) et le socket
    erreurBind = bind(fileDescriptor, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in));
    if (erreurBind<0) {
        perror("erreur de création du socket"); 
        return (EXIT_FAILURE);
    } 
    

    while(true){
        printf("Ecoute...\n");
        fflush(stdout);
        //On attend de recevoir un message d'un client avec le nom d'une music
        int len = recvfrom(fileDescriptor, &msgFrom, sizeof(msgFrom), 0, (struct sockaddr*) &from, &flen);
        if (len<0) {
            perror("Le message reçu est incorrecte"); 
            return (EXIT_FAILURE);
        }

        activeFrom = from;

        while (true)
        {
            if(access(msgFrom.fileName, F_OK) == -1){
                //Si le fichier n'existe pas
                sound.error = 1;
                memset(sound.errorMessage, 0, sizeof sound.errorMessage);
                sprintf(sound.errorMessage, "err: Le fichier n'existe pas");
                erreurSendTo = sendto(fileDescriptor, &bufferMessage.buffer, sizeof(bufferMessage.buffer)+1, 0, (struct sockaddr*) &activeFrom, sizeof(struct sockaddr_in));
                break;
            } else {
                //Si le fichier existe
                rd = aud_readinit(msgFrom.fileName, &sound.audio.sample_rate, &sound.audio.sample_size, &sound.audio.channels);
                sound.error = 0;

                //On envoie la structure du son au client
                erreurSendTo = sendto(fileDescriptor, &sound, sizeof(sound)+1, 0, (struct sockaddr*) &activeFrom, sizeof(struct sockaddr_in));
                if(erreurSendTo<0) {
                    perror("erreur de sendTo"); 
                    return (EXIT_FAILURE);
                }
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
                break;
            }
            if(FD_ISSET(fileDescriptor, &read_set)){
                //On attend la réponse du client
                while (true)
                {
                    int len = recvfrom(fileDescriptor, &ack, sizeof(ack), 0, (struct sockaddr*) &from, &flen);
                    if (len<0) {
                        perror("Le message reçu est incorrecte"); 
                        return (EXIT_FAILURE);
                    }

                    if(activeFrom.sin_addr.s_addr == from.sin_addr.s_addr && activeFrom.sin_port == from.sin_port){
                        //Si c'est le bon client
                        break;
                    } else {
                        //Si c'est pas le bon client
                        message error;
                        error.error = 1;
                        memset(error.errorMessage, 0, sizeof error.errorMessage);
                        sprintf(error.errorMessage, "err: Le serveur est occupé\n");

                        erreurSendTo = sendto(fileDescriptor, &error, sizeof(error)+1, 0, (struct sockaddr*) &from, sizeof(struct sockaddr_in));
                        if(erreurSendTo<0) {
                            perror("erreur de sendTo"); 
                            return (EXIT_FAILURE);
                        }
                    }
                }
            }
            
            do
            {
                //On lit un bout du fichier
                bufferMessage.data = read(rd, bufferMessage.buffer, sizeof(bufferMessage.buffer));

                if(msgFrom.volume == 1){
                    for (size_t i = 0; i < (bufferSize); i++){
                        int16_t tmp = 0;
                        if(sound.audio.sample_size == 16){
                            tmp = bufferMessage.buffer[i++] & 0x00FF;
                            if (i < bufferSize) {
                                tmp |= ((int16_t) bufferMessage.buffer[i] << 8);
                            }
                        } else {
                            tmp = bufferMessage.buffer[i];
                        }
                        tmp *= msgFrom.volumeData;
                        if(sound.audio.sample_size == 16){
                            bufferMessage.buffer[i] = (tmp >> 8) & 0x00FF;
                            bufferMessage.buffer[i - 1] = tmp & 0x00FF;
                        } else {
                            bufferMessage.buffer[i] = tmp;
                        }
                    }
                } 

                if(msgFrom.mono == 1 && sound.audio.channels == 2){ 
                    int iNewBuffer = 0;
                    for (size_t i = 0; i < (bufferSize); i++){
                        int16_t left = 0; int16_t right = 0; int16_t moyenne = 0 ;
                        //On récupère le son gauche
                        if(sound.audio.sample_size == 16){
                            left = bufferMessage.buffer[i++] & 0x00FF;
                            if (i < bufferSize) {
                                left |= ((int16_t) bufferMessage.buffer[i++] << 8);
                            }
                        } else {
                            left = bufferMessage.buffer[i++];
                        }
                        //On récupère le son droit
                        if(sound.audio.sample_size == 16){
                            right = bufferMessage.buffer[i++] & 0x00FF;
                            if (i < bufferSize) {
                                left |= ((int16_t) bufferMessage.buffer[i] << 8);
                            }
                        } else {
                            left = bufferMessage.buffer[i];
                        }
                        //on fait la moyenne
                        moyenne = (left + right) / 2;

                        //On met le resultat dans un nouveau Buffer
                        if(sound.audio.sample_size == 16){
                            bufferMessage.bufferMono[iNewBuffer++] = moyenne & 0x00FF;
                            bufferMessage.bufferMono[iNewBuffer++] = (moyenne >> 8) & 0x00FF;
                        } else {
                            bufferMessage.bufferMono[iNewBuffer++] = moyenne;
                        }
                    }
                } 

                //On l'envoi au client
                erreurSendTo = sendto(fileDescriptor, &bufferMessage, sizeof(bufferMessage)+1, 0, (struct sockaddr*) &activeFrom, sizeof(struct sockaddr_in));
                if(erreurSendTo<0) {
                    perror("erreur de sendTo"); 
                    return (EXIT_FAILURE);
                }


                if(bufferMessage.data != 0){
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
                        cancel = 1;
                        break;
                    }
                    if(FD_ISSET(fileDescriptor, &read_set)){
                        //On attend la fin de la lecture du bout envoyé pour envoyer la suite
                        while (true)
                        {
                            int len = recvfrom(fileDescriptor, &ack, sizeof(ack), 0, (struct sockaddr*) &from, &flen);
                            if (len<0) {
                                perror("Le message reçu est incorrecte"); 
                                return (EXIT_FAILURE);
                            }

                            if(activeFrom.sin_addr.s_addr == from.sin_addr.s_addr && activeFrom.sin_port == from.sin_port){
                                //Si c'est le bon client
                                break;
                            } else {
                                //Si c'est pas le bon client
                                message error;
                                error.error = 1;
                                memset(error.errorMessage, 0, sizeof error.errorMessage);
                                sprintf(error.errorMessage, "err: Le serveur est occupé\n");

                                erreurSendTo = sendto(fileDescriptor, &error, sizeof(error)+1, 0, (struct sockaddr*) &from, sizeof(struct sockaddr_in));
                                if(erreurSendTo<0) {
                                    perror("erreur de sendTo"); 
                                    return (EXIT_FAILURE);
                                }
                            }
                        }
                    }
                }
            } while (bufferMessage.data != 0);
            if(cancel == 1){
                cancel = 0;
                break;
            }
        }
    }  

    close(fileDescriptor);
}