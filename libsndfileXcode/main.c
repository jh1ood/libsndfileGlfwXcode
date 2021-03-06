#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include <fftw3.h>
#include <sndfile.h>
#define PI 3.14159265
#define BUFSIZE 4096
#define NFFT 4096

float* buffer;
float* fftwindow;
int numframes;
int ww, hh;
fftw_complex *in, *out;
fftw_plan p;

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

GLdouble vertex[][3] = {
    { 0.5, 0.5, 0.2 },
    { 1.0, 0.5, 0.2 },
    { 1.0, 1.0, 0.2 },
    { 0.5, 1.0, 0.2 },
    { 0.5, 0.5, 0.7 },
    { 1.0, 0.5, 0.7 },
    { 1.0, 1.0, 0.7 },
    { 0.5, 1.0, 0.7 }
};

int edge[][2] = {
    { 0, 1 },
    { 1, 2 },
    { 2, 3 },
    { 3, 0 },
    { 4, 5 },
    { 5, 6 },
    { 6, 7 },
    { 7, 4 },
    { 0, 4 },
    { 1, 5 },
    { 2, 6 },
    { 3, 7 }
};

int face[][4] = {
    { 0, 1, 2, 3 },
    { 1, 5, 6, 2 },
    { 5, 4, 7, 6 },
    { 4, 0, 3, 7 },
    { 4, 5, 1, 0 },
    { 3, 2, 6, 7 }
};

GLdouble color[][3] = {
    { 1.0, 0.0, 0.0 },
    { 0.0, 1.0, 0.0 },
    { 0.0, 0.0, 1.0 },
    { 1.0, 1.0, 0.0 },
    { 1.0, 0.0, 1.0 },
    { 0.0, 1.0, 1.0 }
};

void display()
{
    static int irep = 44100*0;
    int iskip = 1;
    
    glLineWidth(2.0);
    glColor3d(1.0, 1.0, 0.3);
    glBegin(GL_LINE_STRIP);
    for(int i=0;i<ww;i+=2) {
        glVertex2d(2.0*(float)i/(float)ww - 1.0, buffer[iskip*i+NFFT*irep]);
    }
    glEnd();

    glLineWidth(2.0);
    glColor3d(0.0, 1.0, 1.0);
    glBegin(GL_LINE_STRIP);
    for(int i=1;i<ww;i+=2) {
        glVertex2d(2.0*(float)i/(float)ww - 1.0, buffer[iskip*i+NFFT*irep]);
    }
    glEnd();
    
for(int ich=0;ich<2;ich++) {
    for(int i=0;i<NFFT;i++) {
        in[i][0] = buffer[2*i+ich+NFFT*irep] * fftwindow[i];
        in[i][1] = 0.0;
    }
    fftw_execute(p);

    glColor3d(1.0*ich, 0.0, 1.0*(1-ich));
    glBegin(GL_LINE_STRIP);
    float value, valueb4 = 0.0;
    for(int i=0;i<NFFT/2;i++) {
        value = 10.0*log10(out[i][0]*out[i][0] + out[i][1]*out[i][1]);
        if(i>0) {
        glBegin(GL_QUADS);
        GLdouble vertexwrk[4][3];
        vertexwrk[0][0] = 2.0*(float) i   /(float)(NFFT/2) - 1.0; vertexwrk[0][1] = value  /90; vertexwrk[0][2] = -0.00;
        vertexwrk[1][0] = 2.0*(float) i   /(float)(NFFT/2) - 1.0; vertexwrk[1][1] = value  /90; vertexwrk[1][2] =  0.20;
        vertexwrk[2][0] = 2.0*(float)(i-1)/(float)(NFFT/2) - 1.0; vertexwrk[2][1] = valueb4/90; vertexwrk[2][2] =  0.20;
        vertexwrk[3][0] = 2.0*(float)(i-1)/(float)(NFFT/2) - 1.0; vertexwrk[3][1] = valueb4/90; vertexwrk[3][2] = -0.00;
        glVertex3dv(vertexwrk[0]);
        glVertex3dv(vertexwrk[1]);
        glVertex3dv(vertexwrk[2]);
        glVertex3dv(vertexwrk[3]);
        glEnd();
        }
        valueb4 = value;
    
    }
    glEnd();
}

    glColor3d(0.8, 1.0, 0.8);
    for(int i=0;i<20;i++) {
        float value = 10.0*(i-10);
        glBegin(GL_QUADS);
        GLdouble vertexwrk[4][3];
        vertexwrk[0][0] = -1.0; vertexwrk[0][1] = value/90; vertexwrk[0][2] = -0.00;
        vertexwrk[1][0] = -1.0; vertexwrk[1][1] = value/90; vertexwrk[1][2] =  0.02;
        vertexwrk[2][0] =  1.0; vertexwrk[2][1] = value/90; vertexwrk[2][2] =  0.02;
        vertexwrk[3][0] =  1.0; vertexwrk[3][1] = value/90; vertexwrk[3][2] = -0.00;
        glVertex3dv(vertexwrk[0]);
        glVertex3dv(vertexwrk[1]);
        glVertex3dv(vertexwrk[2]);
        glVertex3dv(vertexwrk[3]);
        glEnd();
    }
    
    glBegin(GL_QUADS);
    for (int j = 0; j < 6; ++j) {
        for (int i = 0; i < 4; ++i) {
            glColor3dv(color[(i+j) % 6]);
            glVertex3dv(vertex[face[j][i]]);
        }
    }
    glEnd();

    if(irep++ > numframes/NFFT - 1) {
        irep = 0;
        printf("irep reset \n");
    }
}

int main(int argc, char *argv[])

{
    SNDFILE *fp;
    SF_INFO sfinfo;
    
    if( (fp = sf_open("/Users/user1/Music/10kHzand7kHz.wav", SFM_READ, &sfinfo)) == NULL) {
        printf("error: file not found.\n");
        return 1;
    };
    printf("samplerate = %d \n", sfinfo.samplerate);
    printf("channels   = %d \n", sfinfo.channels);
    printf("frames     = %d \n", (int)sfinfo.frames);
    
    buffer    = malloc(sfinfo.channels*(int)sfinfo.frames*sizeof(float));
    fftwindow = malloc(NFFT*sizeof(int));
    for(int i=0;i<NFFT;i++) {
        fftwindow[i] = 0.50 - 0.50*cos(2.0*PI*(float)i/(float)(NFFT-1));
    }

    numframes = (int) sf_readf_float(fp, buffer, (int)sfinfo.frames);
    printf("numframes  = %d \n", numframes);
   
    in  = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * NFFT);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * NFFT);
    p = fftw_plan_dft_1d(NFFT, in, out, FFTW_FORWARD, FFTW_ESTIMATE);


    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    window = glfwCreateWindow(640, 640, "libsndfileGlfwXcode", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);
    
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.6f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glOrtho(-1.5, 1.5, -1.5, 1.5, 2.5, -2.5);
        glRotatef((float) glfwGetTime() * 10.f, 1.f, 0.1f, 1.f);
        
        display();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();

    sf_close(fp);
    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
    
    return 0;
}