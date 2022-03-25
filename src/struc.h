#ifndef __include_structures_h__
#define __include_structures_h__

//format de son
typedef struct audio{
		int sample_rate;
		int sample_size;
		int channels;
};

typedef struct message{
		struct audio audio;
		int error;
		char errorMessage[128];
};

typedef struct bufferSound{
	char buffer[1024];
	int data;
};

#endif /* #ifndef __include_fichier_h__ */