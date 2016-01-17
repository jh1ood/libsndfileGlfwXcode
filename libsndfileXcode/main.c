#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GLUT/glut.h>
#import "sndfile.h"
#define BUFSIZE 4096

float* buffer;
int numframes;

void display()
{
    static int irep = 0;
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glLineWidth(1.0);
    glColor3d(1.0, 1.0, 1.0);
    glBegin(GL_LINE_STRIP);
    for(int i=0;i<200;i++) {
        glVertex2d((float)i/200.0, buffer[i+irep]);
    }
    glEnd();
    
    glLineWidth(5.0);
    glColor3d(1.0, 1.0, 0.0);
    glBegin(GL_LINE_STRIP);
    for(int i=0;i<200;i+=2) {
        glVertex2d((float)i/200.0, buffer[i+irep]);
    }
    glEnd();

    glLineWidth(5.0);
    glColor3d(0.0, 1.0, 1.0);
    glBegin(GL_LINE_STRIP);
    for(int i=1;i<200;i+=2) {
        glVertex2d((float)i/200.0, buffer[i+irep]);
    }
    glEnd();
    glFlush();
    
    if(irep++>BUFSIZE-200) {
        irep = 0;
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
    
    if( (fp = sf_open("/Users/user1/SinWave.wav", SFM_READ, &sfinfo)) == NULL) {
        printf("error: file not found.\n");
        return 1;
    };
    printf("samplerate = %d \n", sfinfo.samplerate);
    printf("channels   = %d \n", sfinfo.channels);
    printf("frames     = %d \n", (int)sfinfo.frames);

    buffer = malloc(sfinfo.channels*BUFSIZE*sizeof(float));

    int totalnumframes = 0;
    while ((numframes = (int) sf_readf_float(fp, buffer, BUFSIZE)) > 0) {
        for(int i=0;i<numframes;i++) {
            printf("%f \n", buffer[i]);
        }
        printf("numframes = %d \n", numframes);
        totalnumframes += numframes;
        break;
    }
    printf("totalnumframes = %d \n", totalnumframes);
    
    glutInit( &argc, argv );
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(20, 60);
    glutInitWindowSize(1000, 750);
    glutCreateWindow("libsndfileXcode");
    glClearColor(0.5, 0.8, 0.0, 1.0);
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutIdleFunc(idle);
    glutMainLoop();
    
    sf_close(fp);
    
    return 0;
}