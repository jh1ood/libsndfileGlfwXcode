#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#import "sndfile.h"
#define BUFSIZE 4096

int main(void)

{
    SNDFILE *fp;
    SF_INFO sfinfo;
    
    float *buffer;
    buffer = malloc(2*BUFSIZE*sizeof(float));
    
    if( (fp = sf_open("/Users/user1/SinWave.wav", SFM_READ, &sfinfo)) == NULL) {
        printf("error: file not found.\n");
        return 1;
    };
    printf("samplerate = %d \n", sfinfo.samplerate);
    printf("channels   = %d \n", sfinfo.channels);
    printf("frames     = %d \n", (int)sfinfo.frames);
    
    int numframes = (int) sf_readf_float(fp, buffer, sfinfo.frames);
    for(int i=0;i<numframes;i++) {
        printf("%f \n", buffer[i]);
    }
    
    sf_close(fp);
    
    return 0;
}