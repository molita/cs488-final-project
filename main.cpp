/* ***************************************************************
 *
 * File :  main.cpp
 *
 * Author : Tiberiu Popa
 *  	    J. Alexander Clarke
 * Date   : June 18th, 2002
 *
 * Modified:
 *
 * Purpose: Simple OpenGL program to illustrate the use of GLui with OpenGL and SDL for sound only
 *
 * ****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <time.h>

#include "SoundManager.h"

#include "Game.hpp"
#include "glm/glm.h"
#include "glm/glmint.h"



#ifdef WIN32
#include <windows.h>
#endif /* WIN32 */

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#ifdef WIN32
#include "glui.h"
#else
//#include <GL/glui.h>
#endif /* WIN32 */

//-------------------------------------------------------------------
// defines
//-------------------------------------------------------------------
#define CALLBACK_QUIT 'q'

#define MICKEY 1
#define WORLDMESH 2

#include "Particle.hpp"
//-------------------------------------------------------------------
// GLUT data 
//-------------------------------------------------------------------
int scrWidth, scrHeight;
float cur_x = -1;
float cur_y = 0;
float prev_x = 0;
float prev_y = 0;
int buttons[5] = {GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP};

int SND_shoot;

float xpos = 0, ypos = 0;

int music_on = 0;


// Camera Variables
double cameraAngleX;


// Player Variables
double playerPosX, playerPosY, playerPosZ;
double tankDirection;
double turretDirection;
double tankRadius = 2;

// Missile Variables
double missilePosX, missilePosY, missilePosZ;
double missileDirection;
double missileXmove, missileZmove;
bool missileLaunched, missileCollided, displayMissile;

// Alien variables
double alienPosX, alienPosY, alienPosZ;
double alienDirection;
double alienRadius = 0.5;

double alienAnimate1[3];
double alienAnimate2[3];
double alienAnimate3[3];

double alienXmove, alienZmove;
double animationCounter = 0;
double hopDestination[3];		// This will be a certain number of units in the hop direction
double hopDirection[3];
double alienHopModifier;
double alienHopHeight;
double alienMaxHopHeight = 3;

int prevDifference;

Game game;

// Models
GLMmodel* tankTurret;
GLMmodel* tankBody;
GLMmodel* alien;

//Texture alienTexture;

//--------------------------------------------------------------------
//  State variables
//--------------------------------------------------------------------

//-------------------------------------
// fix lights
//-------------------------------------

void lights(){

    GLfloat light_position1[] = {50, 50, 0, 1};
    GLfloat light1[] = {0.5, 0.5, 0.5, 1};
    GLfloat light2[] = {0.5, 0.5, .5, 1.0};
    GLfloat zero[] = {0, 0, 0 , 0};

    // setup 
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 25);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light2);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light1);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light2);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
    

}

// Check Collision
// For now, doing just tank vs one alien
// sX and sZ are the source X and Z
// dX and dZ are the destination X and Z

// Type 0 is tank
// Type 1 is alien

// Returns true if movement is acceptable
bool checkCollision(double sX, double sZ, double dX, double dZ, int type, double amountTranslatedX, double amountTranslatedZ)
{
	double tempX = sX + amountTranslatedX;
	double tempZ = sZ + amountTranslatedZ;

	// Radius of the source and destination objects
	int sR, dR;
	if (type == 0)
	{
		sR = tankRadius;
		dR = alienRadius;
	}
	if (type == 1)
	{
		sR = alienRadius;
		dR = tankRadius;
	}
	
	// This essentially checks if the intended move will go inside the bounding box of the destination
	if (
		((tempX + sR) > (dX - dR)) &&	// Check left side collision
		((tempX - sR) < (dX + dR)) && // Check right side collision
		((tempZ - sR) < (dZ + dR)) &&	// Check bottom side collision
		((tempZ + sR) > (dZ - dR)))		// Check top side collision
		// Return false, because there is collision		
		return false;
	else
	{
		// Additionally, check the Y value
		// Cannot go on Mountains or Water
		
		if ((game.getHeight(tempX, tempZ)) > 0.1 &&
				(game.getHeight(tempX, tempZ)) < 0.5)
			return true;	
		else
			return false;
	}
}

void calculateAlienNextHop()
{
	hopDestination[3];		// This will be a certain number of units in the hop direction
	hopDirection[3];
		
	// Get the direction unit vector
	hopDirection[0] = playerPosX - alienPosX;
	hopDirection[1] = playerPosY - alienPosY;
	hopDirection[2] = playerPosZ - alienPosZ;

	// Unitize it
	double magnitude = sqrt(hopDirection[0]*hopDirection[0] + hopDirection[2]*hopDirection[2]);
	hopDirection[0] = hopDirection[0]/magnitude;
	hopDirection[1] = hopDirection[1]/magnitude;
	hopDirection[2] = hopDirection[2]/magnitude;

	hopDestination[0] = alienPosX + hopDirection[0];
	hopDestination[2] = alienPosZ + hopDirection[2];
	hopDestination[1] = game.getHeight(hopDestination[0], hopDestination[2]);
	
	alienXmove = hopDirection[0] / 60;
	alienZmove = hopDirection[2] / 60;	
}

void animateAlien()
{
		alienPosX = alienPosX + alienXmove;
		alienPosZ = alienPosZ + alienZmove;
		alienPosY = 12 * game.getHeight(alienPosX, alienPosZ) + alienHopHeight;

	if (alienHopHeight >= alienMaxHopHeight)
		alienHopModifier = -0.05;
	else if (alienHopHeight <= 0)
		alienHopModifier = 0.05;

	alienHopHeight = alienHopHeight + alienHopModifier;

	if (abs(alienPosX - hopDestination[0]) < 0.05)
	{
		// If true, there is collision
		if (
			((alienPosX + alienRadius) > (playerPosX - tankRadius)) &&	// Check left side collision
			((alienPosX - alienRadius) < (playerPosX + tankRadius)) && // Check right side collision
			((alienPosZ - alienRadius) < (playerPosZ + tankRadius)) &&	// Check bottom side collision
			((alienPosZ + alienRadius) > (playerPosZ - tankRadius)))		// Check top side collision
		{
			alienPosX = rand() % 100 + 15;
			alienPosZ = -(rand() % 100 + 15);			
		}

		calculateAlienNextHop();
	}
}

void animateMissile()
{
	if (missileLaunched)
	{
		if (!missileCollided)
		{
			missilePosX = missilePosX + missileXmove;
			missilePosZ = missilePosZ + missileZmove;
		}
		else
		{
			animationCounter++;
			if (animationCounter > 100 && displayMissile)
			{
				displayMissile = false;
			}
			if (animationCounter > 200)
			{
				animationCounter = 0;
				missileLaunched = false;
				missileCollided = false;

				Init();
				setCoef(0.999);

				missilePosX = playerPosX + 0.15;
				missilePosY = playerPosY + 0.9;
				missilePosZ = playerPosZ - 0;

				//std::cerr << "RESTARTING";

			}
			
		}
		// Check for collision with terrain
		if (missilePosY / 12 < game.getHeight(missilePosX, missilePosZ))
		{
			setCoef(1.1);
			missileCollided = true;
		}

		// Check if missile goes out of world bounds
		if (missilePosX < 0 || missilePosX > 127 ||
				missilePosZ > 0 || missilePosZ < -127)
		{
			missileCollided = true;
			missileLaunched = false;
			Init();
		}
	
		// Check if missile collides with enemy
		if (
			((alienPosX + alienRadius) > (missilePosX)) &&	// Check left side collision
			((alienPosX - alienRadius) < (missilePosX)) && // Check right side collision
			((alienPosZ - alienRadius) < (missilePosZ)) &&	// Check bottom side collision
			((alienPosZ + alienRadius) > (missilePosZ)))		// Check top side collision
		{
			missileCollided = true;
			missileLaunched = false;
			alienPosX = rand() % 100 + 15;
			alienPosZ = -(rand() % 100 + 15);	
			calculateAlienNextHop();
		}

	}
	else
	{
		missilePosX = playerPosX + 0.15;
		missilePosY = playerPosY + 0.9;
		missilePosZ = playerPosZ - 0;
	}
}

//-------------------------------------------------------------------
// Custom render function
//-------------------------------------------------------------------

void render(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glLoadIdentity(); // Reset the view

		// The Skybox
		glPushMatrix();
		glLoadIdentity();
		
		// Position and scale it
		// Center it around player and move it down a little
		glTranslated(playerPosX - 64, -70, playerPosZ + 64);
		glScaled(128, 128, 128);
		
		glDisable(GL_LIGHTING);
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		
		glColor3f(1, 1, 1);
		glCallList(3);

		glEnable(GL_LIGHTING);
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);		

		glPopMatrix();

	
		// The world mesh
		glCallList(2);

		// Body
		glColor3f(1, 0, 0);
		glPushMatrix();
		glLoadIdentity();
		glTranslated(playerPosX, playerPosY, playerPosZ);
		glRotated(tankDirection - 90, 0, 1, 0);
		glmDraw(tankBody, GLM_SMOOTH);
	
		glPopMatrix();
		
		// Turret
		glColor3f(1, 0, 0);
		glPushMatrix();
		glLoadIdentity();
		glTranslated(
			playerPosX+(0.5*cos(tankDirection*(M_PI/180))), 
			playerPosY+0.8, 
			playerPosZ-(0.5*sin(tankDirection*(M_PI/180))));

		glRotated(turretDirection - 90, 0, 1, 0);
		glmDraw(tankTurret, GLM_SMOOTH);

		glPopMatrix();
	
		//if (displayMissile)
		//{
			glPushMatrix();
			glLoadIdentity();
			glTranslated(missilePosX, missilePosY, missilePosZ);		
			Display();
			Idle();
		//}
		// Alien
		glColor3f(1, 1, 1);
		glPushMatrix();
		glLoadIdentity();
		glTranslated(alienPosX, alienPosY, alienPosZ);

		//glBindTexture(GL_TEXTURE_2D, alienTexture.texID); 

		glmDraw(alien, GLM_SMOOTH);

		glColor3f(1, 1, 1);
}

// Function to calculate the amount to rotate based on mouse movement

float cameraRotateMotion(float current, float previous)
{
	float difference = abs(current - previous);

	// Cap the difference 
	if (difference > 50)
		difference = prevDifference;

	prevDifference = difference;

	return difference * 0.5;
}

//-------------------------------------------------------------------
// motion 
//-------------------------------------------------------------------
void passiveMotion(int x, int y){

	cur_x = x;
	cur_y = y;

	// For looking up and down

	// Looking up
	if (prev_y < cur_y && cameraAngleX < 20 && cameraAngleX > -5)
		if (cameraAngleX > -4)
			cameraAngleX = cameraAngleX - 0.5;// cameraRotateMotion(cur_y, prev_y);
	// Looking Down
	if (prev_y > cur_y && cameraAngleX > -5 && cameraAngleX < 20)
		if (cameraAngleX < 19)
			cameraAngleX = cameraAngleX + 0.5;//cameraRotateMotion(cur_y, prev_y);

// For the tank turret
	// Looking Right
	if (prev_x < cur_x)
			turretDirection = turretDirection - cameraRotateMotion(cur_x, prev_x);
	// Looking Left
	if (prev_x > cur_x)
			turretDirection = turretDirection + cameraRotateMotion(cur_x, prev_x);

  prev_x = cur_x;
  prev_y = cur_y;

  glutPostRedisplay();
}

//-------------------------------------------------------------------
// mouse 
//-------------------------------------------------------------------
void mouse(int button, int state, int x, int y)
  {
		switch (button)
		{
			// Left Click
			case 0:
				if (!missileLaunched)
				{
					SM.PlaySound(SND_shoot);
		
					//setCoef(0.999);	
					//Init();
		
					missileLaunched = true;
					displayMissile = true;
					missileCollided = false;
					missileDirection = turretDirection;

					missileXmove = -sin(missileDirection*(M_PI/180))*1;
					missileZmove = -cos(missileDirection*(M_PI/180))*1;

					missileXmove = missileXmove / 2;
					missileZmove = missileZmove / 2;

					missilePosX = playerPosX + 0.5;
					missilePosY = playerPosY + 1.1;
					missilePosZ = playerPosZ - 0;
				}
				break;
			
		}
/*
		std::cerr << button;

    if(button>0){	

			if(state==GLUT_DOWN && buttons[button]==GLUT_UP)
			    SM.PlaySound(SND_shoot);

			if(state==GLUT_UP && buttons[button]==GLUT_DOWN)
			    SM.StopSound(button);
		    } else {
			if(state==GLUT_DOWN){

			} 
    }
*/
      buttons[button] = state;

      glutPostRedisplay();

  }

//-------------------------------------------------------------------
// display p
//-------------------------------------------------------------------
void display(void)
  {
  
	/* set up for perspective drawing 
     */
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, scrWidth, scrHeight);
	gluPerspective(40.0, (GLfloat)scrWidth/(GLfloat)scrHeight, 0.1, 1000.0);

	// X axis rotation
	glRotated(cameraAngleX, 1, 0, 0);

	// Make camera rotate with turret direction
	glRotated(-turretDirection, 0, 1, 0);

	// Camera Translation
	glTranslated(
		-playerPosX + 20*(cos((turretDirection - 90)*(M_PI/180))*cos(180*(M_PI/180))), 
		-playerPosY - 7, 
		-playerPosZ - 20*(sin((turretDirection - 90)*(M_PI/180))*cos(180*(M_PI/180))));

	/* change to model view for drawing
     */
	glMatrixMode(GL_MODELVIEW);

	/* Reset modelview matrix
     */
	glLoadIdentity();
	

	/* Clear framebuffer (both colour and depth buffers)
     */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Set up light sources
     */

	// set up the track ball
	
	// enable rescalling of the normals
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);

	render();

	animateAlien();
	animateMissile();
	// dump the whole buffer onto the screen should fix my bug
	glFinish();
	glutSwapBuffers();

  }

//-------------------------------------------------------------------
// keyboard 
//-------------------------------------------------------------------
void keyboard(unsigned char k, int x, int y)
{
	double xMove, zMove;

  switch(k)
    {
		case 'a':
			// Rotate the tank left
			tankDirection = tankDirection + 1.0;
			break;
		case 'd':
			// Rotate the tank right
			tankDirection = tankDirection - 1.0;
			break;
		case 'w':
			// Move the tank forward
			xMove = -sin(tankDirection*(M_PI/180))*1;
			zMove = -cos(tankDirection*(M_PI/180))*1;

			if (checkCollision(playerPosX, playerPosZ, alienPosX, alienPosZ, 0, xMove, zMove))
			{
				playerPosX = playerPosX + xMove;
				playerPosZ = playerPosZ + zMove;
				playerPosY = (12 * game.getHeight(playerPosX, playerPosZ)) + 1;
			}

			break;
		case 's':
			// Move the tank backward
			playerPosX = playerPosX + sin(tankDirection*(M_PI/180))*1;
			playerPosZ = playerPosZ + cos(tankDirection*(M_PI/180))*1;			
			// Update the Y value based on height data
			playerPosY = (12 * game.getHeight(playerPosX, playerPosZ)) + 1;
			break;
    case CALLBACK_QUIT:
      exit(0);
      break;
    }


  glutPostRedisplay();
}

//-------------------------------------------------------------------
// init 
//-------------------------------------------------------------------
void init(int argc, char** argv)
{
  glEnable(GL_TEXTURE_2D);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_COLOR_MATERIAL);

	glCullFace(GL_BACK);

	// Black Background
	glClearColor(0, 0, 0, 0);	
	//glClearColor((float)99/255, (float)122/255, (float)192/255, 0.0f);
	
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);					
	
	glDepthFunc(GL_LEQUAL);

	lights();
	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	

	// Hide the mouse
	glutSetCursor(GLUT_CURSOR_NONE);

	// Load the models

	// Tank Turret
	tankTurret = glmReadOBJ("models/TankTurret2.obj");
	glmScale(tankTurret, 0.02);
	glmVertexNormals(tankTurret, 180.0, false);

	// Tank Body
	tankBody = glmReadOBJ("models/TankBody.obj");
	glmScale(tankBody, 0.02);
	glmVertexNormals(tankBody, 180.0, false);

	// Alien
	alien = glmReadOBJ("models/meleeAlien.obj");
	glmScale(alien, 0.05);
	glmFacetNormals(alien);
	glmVertexNormals(alien, 90.0, false);
	//glmVertexNormals(alien, 180.0, false);

	// Instantiate the game
	game = Game();
}

//-------------------------------------------------------------------
// reshape 
//-------------------------------------------------------------------
void reshape(int w, int h)
  {
  scrWidth = w;
  scrHeight = h;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, w, h);
  gluPerspective(40.0, (GLfloat)w/(GLfloat)h, 0.1, 1000.0);

  /* Reset to modelview matrix mode to avoid confusion later
   */
  glMatrixMode(GL_MODELVIEW);
  glutPostRedisplay();

  }

// Entry
// Is called when the cursor enters or leaves the window
void entry(int state)
{	
	// Warp the pointer to the middle of the screen when it leaves window
	if (state == GLUT_LEFT)
	{
		// Need to keep same orientation, so detect which direction the motion was

		int warpToX = scrWidth / 2;
		int warpToY = scrHeight / 2;

		// Up
		if (prev_y > cur_y)
			warpToY++;
		// Down
		if (prev_y < cur_y)
			warpToY--;
		// Left
		if (prev_x < cur_x)
			warpToX--;
		// Right
		if (prev_x > cur_x)
			warpToX++;
	
		

		glutWarpPointer(warpToX, warpToY);
	}
}

//-------------------------------------------------------------------
// main 
//-------------------------------------------------------------------
int main(int argc, char** argv){

	Game game;
      
    int main_window;
    scrWidth = 1200;
    scrHeight = 800;
    
		SND_shoot = SM.LoadSound("Sounds/shoot.wav");

    // intialize glut and main window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
    glutInitWindowSize(scrWidth, scrHeight);
    main_window = glutCreateWindow("Mutant Invaders");

		// Initialize Player Variables
		playerPosX = 40;
		playerPosY = 4.2;
		playerPosZ = -40;

		// Alien Start Position
		alienPosX = 60;
		alienPosY = 12 * game.getHeight(60, -60);
		alienPosZ = -60;

		turretDirection = 270;

		cameraAngleX = 19;

    // initialize callback
    glutDisplayFunc(display);
		glutIdleFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard); 	
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(passiveMotion);
		glutEntryFunc(entry);

    init(argc, argv);

		calculateAlienNextHop();

		missilePosX = playerPosX + 0.5;
		missilePosY = playerPosY + 1.1;
		missilePosZ = playerPosZ - 0;

    Init();

		// Calculate the alien Hop Modifier
		alienHopModifier = alienMaxHopHeight/60;

    reshape(scrWidth, scrHeight);

    glutMainLoop();
  
    return 0;
}




