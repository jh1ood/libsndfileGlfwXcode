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
    
    if( (fp = sf_open("/Users/user1/SinWave.wav", SFM_READ, &sfinfo)) == NULL) {
        printf("error: file not found.\n");
        return 1;
    };
    printf("samplerate = %d \n", sfinfo.samplerate);
    printf("channels   = %d \n", sfinfo.channels);
    printf("frames     = %d \n", (int)sfinfo.frames);

    float *buffer;
    buffer = malloc(sfinfo.channels*BUFSIZE*sizeof(float));

    int numframes;
    int totalnumframes = 0;
    while ((numframes = (int) sf_readf_float(fp, buffer, BUFSIZE)) > 0) {
        for(int i=0;i<numframes;i++) {
            printf("%f \n", buffer[i]);
        }
        printf("numframes = %d \n", numframes);
        totalnumframes += numframes;
    }
    printf("totalnumframes = %d \n", totalnumframes);
    sf_close(fp);
    
    return 0;
}