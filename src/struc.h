#ifndef __include_structures_h__
#define __include_structures_h__

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
	char buffer[1024];
	int data;
} bufferSound;

#endif /* #ifndef __include_fichier_h__ */