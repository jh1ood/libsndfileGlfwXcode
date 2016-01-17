#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GLUT/glut.h>
#include "fftw3.h"
#import "sndfile.h"
#define PI 3.14159265
#define BUFSIZE 4096
#define NFFT 4096

float* buffer;
float* fftwindow;
int numframes;
fftw_complex *in, *out;
fftw_plan p;

void display()
{
    static int irep = 44100*0;
    int iskip = 5;
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glLineWidth(3.0);
    glColor3d(1.0, 1.0, 0.0);
    glBegin(GL_LINE_STRIP);
    for(int i=0;i<200;i+=2) {
        glVertex2d((float)i/200.0, buffer[iskip*i+NFFT*irep]);
    }
    glEnd();

    glLineWidth(3.0);
    glColor3d(0.0, 1.0, 1.0);
    glBegin(GL_LINE_STRIP);
    for(int i=1;i<200;i+=2) {
        glVertex2d((float)i/200.0, buffer[iskip*i+NFFT*irep]);
    }
    glEnd();
    
    for(int ich=0;ich<2;ich++) {
    for(int i=0;i<NFFT;i++) {
        in[i][0] = buffer[2*i+ich+NFFT*irep] * fftwindow[i];
        in[i][1] = 0.0;
    }
    fftw_execute(p);
    
    glLineWidth(1.0);
    glColor3d(1.0*ich, 0.0, 1.0*(1-ich));
    glBegin(GL_LINE_STRIP);
    for(int i=0;i<NFFT/2;i++) {
        float value = log10(out[i][0]*out[i][0] + out[i][1]*out[i][1]);
        glVertex2d((float)i/(float)(NFFT/2), value/8.0);
    }
    glEnd();
    }

    glFlush();
    
    if(irep++ > numframes/NFFT - 1) {
        irep = 0;
        printf("irep reset \n");
    }
}

void resize(int w, int h)
{
    glViewport(0, 0, w, h);
    glLoadIdentity();
    glOrtho(0.0, 1.0, -1.1, 1.1, -1.0, 1.0);
}

void idle(void)
{
    glutPostRedisplay();
}

int main(int argc, char *argv[])

{
    SNDFILE *fp;
    SF_INFO sfinfo;
    
    if( (fp = sf_open("/Users/user1/chirp.wav", SFM_READ, &sfinfo)) == NULL) {
        printf("error: file not found.\n");
        return 1;
    };
    printf("samplerate = %d \n", sfinfo.samplerate);
    printf("channels   = %d \n", sfinfo.channels);
    printf("frames     = %d \n", (int)sfinfo.frames);
    
    buffer    = malloc(sfinfo.channels*(int)sfinfo.frames*sizeof(float));
    fftwindow = malloc(NFFT*sizeof(int));
    for(int i=0;i<NFFT;i++) {
        fftwindow[i] = 0.54 - 0.46*cos(2.0*PI*(float)i/(float)NFFT);
    }

    numframes = (int) sf_readf_float(fp, buffer, (int)sfinfo.frames);
    printf("numframes  = %d \n", numframes);
   
    in  = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * NFFT);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * NFFT);
    p = fftw_plan_dft_1d(NFFT, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    glutInit( &argc, argv );
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(20, 60);
    glutInitWindowSize(1000, 600);
    glutCreateWindow("libsndfileXcode");
    glClearColor(0.7, 0.8, 0.7, 1.0);
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutIdleFunc(idle);
    glutMainLoop();
    
    sf_close(fp);
    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
    
    return 0;
}