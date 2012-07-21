#ifndef PARTICLE_H
#define PARTICLE_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <cstdlib>

typedef struct particle {
  int color;
  float position[3];
  float velocity[3];
  float mass;
} particle;

void setCoef(float value);

void Display();

void Idle();

void Init();

void Collision(int n);

float forces(int i, int j);
#endif
