#ifndef INPUTS_CPP
#define INPUTS_CPP

#include <GLUT/glut.h>

using namespace std;

typedef struct {
    bool w, a, s, d, q, e, y, x, c, r, f, v, one, two, three, four;
} ButtonKeys;

ButtonKeys keys;

void buttonDown(unsigned char key, int x, int y){
    if(key == 'a'){keys.a = true;}
    if(key == 's'){keys.s = true;}
    if(key == 'd'){keys.d = true;}
    if(key == 'w'){keys.w = true;}
    if(key == 'q'){keys.q = true;}
    if(key == 'y'){keys.y = true;}
    if(key == 'x'){keys.x = true;}
    if(key == 'c'){keys.c = true;}
    if(key == 'e'){keys.e = true;}
    if(key == 'r'){keys.r = true;}
    if(key == 'f'){keys.f = true;}
    if(key == 'v'){keys.v = true;}
    if(key == '1'){keys.one = true;}
    if(key == '2'){keys.two = true;}
    if(key == '3'){keys.three = true;}
    if(key == '4'){keys.four = true;}
    glutPostRedisplay();
}

void buttonUp(unsigned char key, int x, int y){
    if(key == 'a'){keys.a = false;}
    if(key == 's'){keys.s = false;}
    if(key == 'd'){keys.d = false;}
    if(key == 'w'){keys.w = false;}
    if(key == 'q'){keys.q = false;}
    if(key == 'y'){keys.y = false;}
    if(key == 'x'){keys.x = false;}
    if(key == 'c'){keys.c = false;}
    if(key == 'e'){keys.e = false;}
    if(key == 'r'){keys.r = false;}
    if(key == 'f'){keys.f = false;}
    if(key == 'v'){keys.v = false;}
    if(key == '1'){keys.one = false;}
    if(key == '2'){keys.two = false;}
    if(key == '3'){keys.three = false;}
    if(key == '4'){keys.four = false;}
    glutPostRedisplay();
}

#endif