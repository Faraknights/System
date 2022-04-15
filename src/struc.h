#ifndef __include_structures_h__
#define bufferSize 1024
#define __include_structures_h__

//Premier message
typedef struct firstMessage{
	char fileName[64];
	int mono;
	int volume;
	double volumeData;
} firstMessage;

//format de son
typedef struct audio{
	int sample_rate;
	int sample_size;
	int channels;
} audio;

typedef struct message{
	audio audio;
	int error;
	char errorMessage[128];
} message;

typedef struct bufferSound{
	char buffer[bufferSize];
	char bufferMono[bufferSize/2];
	int data;
} bufferSound;

#endif /* #ifndef __include_fichier_h__ */