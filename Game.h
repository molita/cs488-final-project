#ifndef GAME_H
#define GAME_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "image.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// Display Lists
#define WORLD 1

class Game {
  public:
    Game();		// Constructor
		~Game();	// Destructor

	private:
		// Generates a world using a heightmap
		// Takes in a .png file which will be read in as a heightmap
		void GenerateWorld(std::string file);

};

#endif
