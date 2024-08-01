#include <stdlib.h>
#include <stdint.h>

#include "chip8.cpp"

#define PIXEL_SIZE 10       //the x/y length/height of every pixel on the screen
#define GL_SILENCE_DEPRECATION      //used for silencing some compiler warnings

using namespace std;

Chip8 cpu = Chip8(&keys);

void display(){
    //auto start = high_resolution_clock::now();
    glutPostRedisplay();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cpu.run();
    
    glutSwapBuffers();
}

void resize(int w, int h){
    glutReshapeWindow(PIXEL_SIZE * 64, PIXEL_SIZE * 32);
}

void setupOpenGL(){
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(PIXEL_SIZE * 64, PIXEL_SIZE * 32);
    glutInitWindowPosition(glutGet(GLUT_SCREEN_WIDTH)/2-(PIXEL_SIZE * 32) ,glutGet(GLUT_SCREEN_HEIGHT)/2 - (PIXEL_SIZE * 16));
    glutCreateWindow("CHIP-8");
    glClearColor(0, 0, 0, 0);
    gluOrtho2D(0, PIXEL_SIZE * 64, PIXEL_SIZE * 32, 0);
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(buttonDown);
    glutKeyboardUpFunc(buttonUp);
}

int main(int argc, char** argv){
    cpu.loadBinary(argv[1], true);
    glutInit(&argc, argv);
    setupOpenGL();
    glutMainLoop();
    return 0;
}