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


#include "SoundManager.h"

#include "Game.h"


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
//-------------------------------------------------------------------
// GLUT data 
//-------------------------------------------------------------------
int scrWidth, scrHeight;
float cur_x = -1;
float cur_y = 0;
float prev_x = 0;
float prev_y = 0;
int buttons[5] = {GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP};

int SND_ID_1 , SND_ID_2, SND_ID_3, MUS_ID_4;

float xpos = 0, ypos = 0;

int music_on = 0;

// Camera Variables
double cameraPosX, cameraPosY, cameraPosZ;
double cameraLookAtX, cameraLookAtY, cameraLookAtZ;		// May not be needed anymore
double cameraAngleX, cameraAngleY, cameraAngleZ;
double cameraDirectionX, cameraDirectionY, cameraDirectionZ;

// Player Variables
double playerPosX, playerPosY, playerPosZ;
double playerDirectionX, playerDirectionY, playerDirectionZ;

int prevDifference;

Game game;

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

//-------------------------------------------------------------------
// Custom render function
//-------------------------------------------------------------------

void render(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glLoadIdentity(); // Reset the view

    // move the scene 
    //glTranslatef(xpos - 100, ypos - 50, -150);

		//glTranslatef(0, 0, -128);		

		//glScalef(5, 5, 5);
		glCallList(2);
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

void updatePlayerDirection()
{
	GLfloat view[16];

	glGetFloatv(GL_PROJECTION_MATRIX, view);

	cameraDirectionX = view[2];
	cameraDirectionY = view[6];
	cameraDirectionZ = view[9];
	
	//std::cerr << "CURRENT PLAYER LOCATION (" << playerPosX << "," << playerPosY << "," << playerPosZ << ")" << std::endl;

	
}

//-------------------------------------------------------------------
// motion 
//-------------------------------------------------------------------
void passiveMotion(int x, int y){

	cur_x = x;
	cur_y = y;
  
	// Check which direction the mouse is moving

	// Looking up
	if (prev_y > cur_y)
			cameraAngleX = cameraAngleX - cameraRotateMotion(cur_y, prev_y);
	// Looking Down
	if (prev_y < cur_y)
			cameraAngleX = cameraAngleX + cameraRotateMotion(cur_y, prev_y);
	// Looking Right
	if (prev_x < cur_x)
			cameraAngleY = cameraAngleY + cameraRotateMotion(cur_x, prev_x);
	// Looking Left
	if (prev_x > cur_x)
			cameraAngleY = cameraAngleY - cameraRotateMotion(cur_x, prev_x);

  prev_x = cur_x;
  prev_y = cur_y;

	updatePlayerDirection();

  glutPostRedisplay();

}

//-------------------------------------------------------------------
// mouse 
//-------------------------------------------------------------------
void mouse(int button, int state, int x, int y)
  {
    if(button>0){	glutWarpPointer(scrWidth/2, scrHeight/2);

	  if(state==GLUT_DOWN && buttons[button]==GLUT_UP)
	      SM.PlaySound(button);

	  if(state==GLUT_UP && buttons[button]==GLUT_DOWN)
	      SM.StopSound(button);
      } else {
	  if(state==GLUT_DOWN){
	      if(!music_on){
		  music_on = 1;
		  SM.PlayMusic(MUS_ID_4);
	      }	else {
		  music_on = 0;
		  SM.StopMusic(MUS_ID_4); 
	      }
	  } 
      }
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

	// Camera Rotation
	glRotated(cameraAngleX / 10, 1, 0, 0);
	glRotated(cameraAngleY / 10, 0, 1, 0);
	glRotated(cameraAngleZ / 10, 0, 0, 1);

	glTranslated(-playerPosX, -playerPosY, -playerPosZ);


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

	// dump the whole buffer onto the screen should fix my bug
	glFinish();
	glutSwapBuffers();

  }

//-------------------------------------------------------------------
// keyboard 
//-------------------------------------------------------------------
void keyboard(unsigned char k, int x, int y)
{
  switch(k)
    {
		case 'a':
			//playerPosX = playerPosX - 5;
			// Rotate the tank left
			break;
		case 'd':
			//playerPosX = playerPosX + 5;
			// Rotate the tank right
			break;
		case 'w':
			// We're only moving the tank forward, not the camera
			// So must change this later
			playerPosX = playerPosX + (cameraDirectionX * 5);
			playerPosZ = playerPosZ + (cameraDirectionZ * 5);
			playerPosY = (12 * game.getHeight(playerPosX, playerPosZ)) + 1;
			break;
		case 's':
			//playerPosZ = playerPosZ + 5;
			// Move the tank backwards
			//playerPosX = playerPosX + (cameraDirectionX * 5);
			//playerPosZ = playerPosZ + (cameraDirectionZ * 5);
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
	glClearColor(0.00f, 0.0f, 0.0f, 0.0f);
	
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);					
	
	glDepthFunc(GL_LEQUAL);

	lights();
	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	

	// Hide the mouse
	glutSetCursor(GLUT_CURSOR_NONE);

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
      

    SND_ID_1 = SM.LoadSound("card.wav");
    SND_ID_2 = SM.LoadSound("OBS.wav");
    SND_ID_3 = SM.LoadSound("ghost.wav");
    MUS_ID_4 = SM.LoadMusic("UNREAL.S3M");

    // intialize glut and main window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
    glutInitWindowSize(scrWidth, scrHeight);
    main_window = glutCreateWindow("Render Test");
	
		// Initialize Camera Variables
		cameraPosX = 64;
		cameraPosY = 0;
		cameraPosZ = -264;
		cameraLookAtX = 0;
		cameraLookAtY = -300;
		cameraLookAtZ = -1000;

		cameraAngleX = 0;
		cameraAngleY = 0;
		cameraAngleZ = 0;

		// Initialize Player Variables
		playerPosX = 0;
		playerPosY = 10;
		playerPosZ = 0;

    // initialize callback
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard); 	
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(passiveMotion);
		glutEntryFunc(entry);

    init(argc, argv);

    reshape(scrWidth, scrHeight);

    glutMainLoop();
  
    return 0;
}

