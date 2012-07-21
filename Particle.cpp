#include "Particle.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

float const num_particles = 1000;
float last_time, present_time;
float speed = 5;
float coef = 0.999; // Coefficient of restriction
bool gravity = true;

particle particles[1000];

GLfloat colors[8][3] = {
  {1,140.0/255.0,0},
  {1,0,0},
  {1,165.0/255.0,0},
  {1,69.0/255.0,0},
  {1,1,0},
  {1,215.0/255.0,0},
  {1,165.0/255.0,0},
  {1,69.0/255.0,0}};

void Display()
{
  int i;

	glPointSize(2);
  glBegin(GL_POINTS);
  
  for (i = 0; i < num_particles; i++)
  {
    glColor3fv(colors[particles[i].color]);
    glVertex3fv(particles[i].position);
  }

  glEnd();
}

void Idle()
{
  int i, j;
  float dt;
  
  present_time = glutGet(GLUT_ELAPSED_TIME);
  dt = 0.001*(present_time - last_time);

  for (i = 0; i < num_particles; i++)
  {
    for (j = 0; j < 3; j++)
    {
      particles[i].position[j] += dt*particles[i].velocity[j];
      particles[i].velocity[j] += dt*forces(i,j)/particles[i].mass;
    }
    Collision(i);
  }
  last_time = present_time;
  glutPostRedisplay();
}

void Init()
{
  int i, j;
  for (i = 0; i < num_particles; i++)
  {
    particles[i].mass = 10.0;
    particles[i].color = i%8;
    for (j = 0; j < 3; j++)
    {
      particles[i].position[j] = 0.01*((float)rand()/RAND_MAX);//-1.0;
      particles[i].velocity[j] = speed * 2.0 * ((float)rand()/RAND_MAX)-1.0;
    }
  }
}

void Collision(int n)
{
  int i;

  for (i = 0; i < 3; i++)
  {
    if (particles[n].position[i] >= 1.0)
    {
      particles[n].velocity[i] = -coef*particles[n].velocity[i];
      particles[n].position[i] = 1.0-coef*(particles[n].position[i] - 0.0);
    }
    if (particles[n].position[i] <= 1.0)
    {
      particles[n].velocity[i] = -coef*particles[n].velocity[i];
      particles[n].position[i] = -0.0001-coef*(particles[n].position[i] + 0.0);
    }
  }
}

float forces(int i, int j)
{
  if (!gravity) return (0.0);
  else if (j==1) return (-1.0);
  else return (0.0);
}
