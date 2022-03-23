//Erkahn Moussa Boyeau Indy Groupe 3.1
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "audio.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>


int main(char argc, char* argv[]) {

printf("Bonjour je suis un client audio\n");



int fd,err;



//debut initialisation write audio
struct Son{
		int sample_rate;
		int sample_size;
		int channels;
		int bytes_read;
		int bytes_write;
};

struct Son music;

struct sockaddr_in from;
struct sockaddr_in dest;
dest.sin_family = AF_INET;
dest.sin_port = htons(1234);
dest.sin_addr.s_addr = inet_addr("127.0.0.1");
char msgSrv[64];
char msgClt[64] = "Client";
socklen_t len;
fd = socket(AF_INET, SOCK_DGRAM, 0);
/*
 * msgClt[0] = sample_rate
 * msgClt[1] = sample_size
 * 
 * 
 * */

err = sendto(fd, msgClt, sizeof(msgClt)+1, 0, (struct sockaddr*) &dest,sizeof(struct sockaddr_in));
if(err<0) {perror("erreur de sendTo");}


len = recvfrom(fd, &music, sizeof(music), 0,(struct sockaddr*) &from, &len);
if (len<0) { perror("erreur de len"); }

int wr = aud_writeinit( music.sample_rate, music.sample_size, music.channels);
int buffer[music.sample_size];




while(music.bytes_write!=0){

len = recvfrom(fd, &music, sizeof(music), 0,(struct sockaddr*) &from, &len);

if (len<0) { perror("erreur de len"); }

printf("Received struct music with sample %d  sample_size %d and channel %d, bytes_writes %d\n", music.sample_rate, music.sample_size, music.channels,music.bytes_write);

music.bytes_write=write(wr,buffer,sizeof(music));




//err=sendto(fd, buffer, sizeof(buffer), 0, (struct sockaddr*) &dest,sizeof(struct sockaddr_in));
if (err<0) { perror("erreur de len"); }

}


printf("Received struct music with sample %d  sample_size %d and channel %d, bytes_writes %d\n", music.sample_rate, music.sample_size, music.channels,music.bytes_write);
close(wr);
close(fd);

return 0;
}
