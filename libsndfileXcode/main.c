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
    static int irep = 44100*0;
    int iskip = 10;
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glLineWidth(5.0);
    glColor3d(1.0, 1.0, 0.0);
    glBegin(GL_LINE_STRIP);
    for(int i=0;i<200;i+=2) {
        glVertex2d((float)i/200.0, buffer[i*iskip+irep]);
    }
    glEnd();

    glLineWidth(5.0);
    glColor3d(0.0, 1.0, 1.0);
    glBegin(GL_LINE_STRIP);
    for(int i=1;i<200;i+=2) {
        glVertex2d((float)i/200.0, buffer[i*iskip+irep]);
    }
    glEnd();
    glFlush();
    
    irep += 4096;
    if(irep > numframes-200) {
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
    
    if( (fp = sf_open("/Users/user1/test.wav", SFM_READ, &sfinfo)) == NULL) {
        printf("error: file not found.\n");
        return 1;
    };
    printf("samplerate = %d \n", sfinfo.samplerate);
    printf("channels   = %d \n", sfinfo.channels);
    printf("frames     = %d \n", (int)sfinfo.frames);

    buffer = malloc(sfinfo.channels*(int)sfinfo.frames*sizeof(float));

    numframes = (int) sf_readf_float(fp, buffer, (int)sfinfo.frames);
    printf("numframes  = %d \n", numframes);
    
    glutInit( &argc, argv );
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(20, 60);
    glutInitWindowSize(1000, 600);
    glutCreateWindow("libsndfileXcode");
    glClearColor(0.5, 0.8, 0.0, 1.0);
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutIdleFunc(idle);
    glutMainLoop();
    
    sf_close(fp);
    
    return 0;
}