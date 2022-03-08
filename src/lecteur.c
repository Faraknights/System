
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

int main(int argc, char *argv[])
{
    char* filename = argv[1];
    int sample_rate, sample_size, channels;
    
    int desc_read = aud_readinit(filename, &sample_rate, &sample_size, &channels);

    printf("sample rate : %d\n", sample_rate);
    printf("sample size : %d\n", sample_size);
    printf("channels : %d\n", channels);

    int desc_write = aud_writeinit(sample_rate, sample_size, 2);

    char buffer[1024];
    int data = read(desc_read, buffer, sizeof(buffer));

    do{
        write(desc_write, buffer, sizeof(buffer));
        data = read(desc_read, buffer, sizeof(buffer));
    } while (data != 0);

    return 0;
}