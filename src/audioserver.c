#include <stdlib.h>
#include <stdio.h>
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

int main(int argc, char *argv[])
{
    int res;
    struct sockaddr_in my_addr;

    int res = socket(AF_INET, SOCK_DGRAM, 0);
    if(res == -1 ){
        perror("erreur de création du socket"); 
        (EXIT_FAILURE);
    } 

    memset(&my_addr, 0, sizeof(struct sockaddr_in));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(1234);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    res = bind(res, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in));
    if (res<0) {
        perror("erreur de création du socket")      ; 
        (EXIT_FAILURE);
    } 
}