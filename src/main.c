/*
   TROPISM
   Copyright 2015 Callum Watkinson and Rob Sanders

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

// Uncomment on Windows
//#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Non-Standard Libraries */
#include <libguile.h>
#include <GL/freeglut.h>

int stats=0;
int start=0;

int food[100*2];

typedef struct CREATURE {
    int posx;
    int posy;
    int width;
    int height;
    int energy;
    int collided[4]; /* {top, right, bottom, left} */
} Creature;

Creature creature = {
    .posx = 100,
    .posy = 100,
    .width = 40,
    .height = 40,
    .energy = 1000,
};

/* internal functions */
void collision();
void eat();
void keyboard(unsigned char, int, int);
void move();
void respire();
void setPerspective();

void
keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 27:
            exit(0);
            break;
        case 32:
            if (stats) stats=0;
            else stats=1;
            break;
        default:
            break;
    }
}

void
setPerspective()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glViewport(0, 0, 640, 480);
    glOrtho(0.0, 640, 480, 0.0, 1.0, -1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void
draw()
{
    if (glutGet(GLUT_ELAPSED_TIME)-start>1000/20.0) {
        start = glutGet(GLUT_ELAPSED_TIME);
        eat();
        move();
        respire();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glColor3f(1.0, 0.0, 0.0);
        int i;
        for (i=0; i<100; i++) {
            glBegin(GL_QUADS);
                glVertex3f(food[2*i+0]+10.0, food[2*i+1]+0.0, -1.0);
                glVertex3f(food[2*i+0]+10.0, food[2*i+1]+10.0, -1.0);
                glVertex3f(food[2*i+0]+0.0, food[2*i+1]+10.0, -1.0);
                glVertex3f(food[2*i+0]+0.0, food[2*i+1]+0.0, -1.0);
            glEnd();
        }
        glColor3f(0.0, 0.2, 1.0);
        glBegin(GL_QUADS);
            glVertex3f(creature.posx+creature.width, creature.posy+0.0, 0.0);
            glVertex3f(creature.posx+creature.width, creature.posy+creature.height, 0.0);
            glVertex3f(creature.posx+0.0, creature.posy+creature.height, 0.0);
            glVertex3f(creature.posx+0.0, creature.posy+0.0, 0.0);
        glEnd();
        
        glutSwapBuffers();
    }
}

void
respire()
{
    creature.energy--;
    if (creature.energy<0) exit(EXIT_SUCCESS);
}

void
eat()
{
    /* Detects if food inside creature then removes it if necessary. */
    int i;
    for (i=0; i<100; i++) {
        if (  (creature.posx                    < food[2*i+0])
            &&(creature.posx+creature.width-10  > food[2*i+0])
            &&(creature.posy                    < food[2*i+1])
            &&(creature.posy+creature.height-10 > food[2*i+1])) {
            /* HACKS */
            creature.energy+=10;
            food[2*i+0] = -10;
            food[2*i+1] = -10;
        }
    }
}

void
move()
{
    /* AI part */
    int multiplier;
    int dir = rand()%4;
    if (creature.energy < 500) multiplier = 10;
    else multiplier = 1;
    switch (dir) {
        case 0: creature.posx+=1.0*multiplier; break;
        case 1: creature.posy+=1.0*multiplier; break;
        case 2: creature.posx-=1.0*multiplier; break;
        case 3: creature.posy-=1.0*multiplier; break;
    }
    collision();
    if (creature.collided[0]+creature.collided[1]+creature.collided[2]+creature.collided[3])
      switch (dir) {
        case 0: creature.posx-=1.0*multiplier; break;
        case 1: creature.posy-=1.0*multiplier; break;
        case 2: creature.posx+=1.0*multiplier; break;
        case 3: creature.posy+=1.0*multiplier; break;
    }   
}

void
generateFood()
{
    int i;
    for (i=0; i<100; i++) {
        food[2*i+0] = rand() % 630;
        food[2*i+1] = rand() % 470;
    }
}

void
spawnCreature()
{

}

void
collision()
{
    if (creature.posx<0) creature.collided[3] = 1;
    else creature.collided[3] = 0;
    if (creature.posy<0) creature.collided[0] = 1;
    else creature.collided[0] = 0;
    if (creature.posx+creature.width>640) creature.collided[1] = 1;
    else creature.collided[1] = 0;
    if (creature.posy+creature.height>480) creature.collided[2] = 1;
    else creature.collided[2] = 0;
}

int
main (int argc, char ** argv)
{
    time_t t;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(100, 100);

    glutCreateWindow("Tropism 0.0.2");
    srand((unsigned) time(&t));
    glEnable(GL_DEPTH_TEST);
    setPerspective();
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(draw);
    glutIdleFunc(draw);

    generateFood();
    spawnCreature();
    
    glutMainLoop();
    
    return 0;
}

