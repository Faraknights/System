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

printf("Serveur actif\n");


int fd, port, err, rec;
//debut initialisation write audio
struct Son{
		int sample_rate;
		int sample_size;
		int channels;
		int bytes_read;
		int bytes_write;
};
struct Son music; 

int  choix=1;
char *name;

switch(choix){
    case 1 : name ="test2.wav";
    break;
    case 2 : name ="test.wav";
    break;
    default : 
	  printf("au revoir\n");
	  //close(socket);
	  return 0;	
}

//a changger test2.wav
int rd = aud_readinit(name,&music.sample_rate, &music.sample_size, &music.channels);
char msgSrv[64] = "Serveur";
char msgClt[64];
int buffer[music.sample_size];


struct sockaddr_in addr;
struct sockaddr_in from;
socklen_t len, flen;


addr.sin_family = AF_INET;
addr.sin_port = htons(1234);
addr.sin_addr.s_addr = htonl(INADDR_ANY);
  
fd = socket(AF_INET,SOCK_DGRAM,0);
if (fd<0) { perror("erreur de fd"); }
 
port = bind(fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
if(port < 0) {perror("Erreur bind"); exit(1); }

flen = sizeof(struct sockaddr_in);

	printf("je suis pret a recevoir\n");
	len = recvfrom(fd, msgClt, sizeof(music), 0,(struct sockaddr*) &from, &flen);
	if(len<0) {perror("erreur de recvfrom");}
	printf("j ai recu\n");



music.bytes_read=read(rd,buffer,music.sample_size);
printf("je lis\n");
while(music.bytes_read!=0){

music.bytes_read=read(rd,buffer,music.sample_size);

err = sendto(fd, &music, sizeof(music)+1, 0, (struct sockaddr*) &from,sizeof(struct sockaddr_in));
if(err<0) {perror("erreur de sendTo");}


printf("bytes_read %d\n", music.bytes_read);



//recvfrom(fd, buffer, sizeof(buffer), 0,(struct sockaddr*) &from, &len);
//~ rec=recvfrom(fd,&music, sizeof(music)+1, 0, (struct sockaddr*) &from, &flen);


if(rec<0) {perror("erreur de recev");}

}

printf("fin");
close(rd);

close(fd);

if (err<0) {perror("erreur 2");}

}
