#ifndef GAME_H
#define GAME_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "image.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "Texture.h"

// Display Lists
#define WORLD 1

class Game {
  public:
    Game();		// Constructor
		~Game();	// Destructor

	double getHeight(double x, double y);
	void drawSkybox();

	private:
		// Generates a world using a heightmap
		// Takes in a .png file which will be read in as a heightmap
		double heightData[128][128];

		void GenerateWorld(std::string file);
		void drawWorldMeshVertex(double x, double y, double z);

		//Texture sbFront, sbBack, sbLeft, sbRight, sbTop;		// Skybox textures
		void setupSkybox();		// Function to set up skybox

		GLuint LoadTexture( const char * filename, int width, int height);

		bool runOnce;

};

#endif
