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

int main(int argc, char *argv[])
{
    int fileDescriptor, erreur;
    struct sockaddr_in my_addr, from;

    fileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if(fileDescriptor == -1 ){
        perror("erreur de création du socket"); 
        return (EXIT_FAILURE);
    } 

    memset(&my_addr, 0, sizeof(struct sockaddr_in));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(1234);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    erreur = bind(fileDescriptor, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in));
    if (erreur<0) {
        perror("erreur de création du socket"); 
        return (EXIT_FAILURE);
    } 

    char msg[64];

    printf("Lancement du serveur 👍");
    printf("Ecoute...");
    while(true){
        socklen_t flen = sizeof(struct sockaddr_in);
        int len = recvfrom(fileDescriptor, msg, sizeof(msg), 0, (struct sockaddr*) &from, &flen);
        if (len<0) {
            perror("Le message reçu est incorrecte"); 
            return (EXIT_FAILURE);
        } else {
            printf("Received %d bytes from host %s port %d: %s", len, inet_ntoa(from.sin_addr), ntohs(from.sin_port), msg);

            msg = "test";
            erreur = sendto(fileDescriptor, msg, strlen(msg)+1, 0, (struct sockaddr*) &from, flen);
            if (erreur<0) { 
                perror("Erreur d'envoi"); 
                return (EXIT_FAILURE);
            }

            break;
        }
    }  

    close(fileDescriptor);
}