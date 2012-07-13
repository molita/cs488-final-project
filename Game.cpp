#include "Game.h"
#include "image.hpp"

Game::Game(){
	std::string heightmap;
	heightmap = "aust.png";
	GenerateWorld(heightmap);
}

Game:: ~Game(){

}

// Get the height of the height field at location (x, y)
double Game::getHeight(double x, double y)
{
	// For now, just round x and y

	int tempX = (int)x;
	int tempY = (int)y;

	if (tempX < 0)
		tempX = 0;
	if (tempX > 127)
		tempX = 127;
	if (tempY < 0)
		tempY = 0;
	if (tempY > 127)
		tempY	= 127;

	return heightData[tempX][-tempY];

}

void Game::GenerateWorld(std::string file) {

	// Load the image
	int h = 128;
	int w = 128;
	Image img(w, h, 1);
	img.loadPng(file);
	
	// Store the data in a 2-dimensional array
	for (int j = 0; j < img.height(); j++)
		for (int i = 0; i < img.width(); i++)
			heightData[i][j] = img(i, j, 0);
	
	// Loop through the array and build the mesh
	
	// Create the new display list
	glNewList(2, GL_COMPILE);
	
	// Temporarily set color of vertex to green
	// glColor3f(0, 1, 0);
	
	// Temporary variables to store x, y, and z
	// Allows for easier readability too
	int bottomLeftX, bottomLeftZ;
	int bottomRightX, bottomRightZ;
	int topLeftX, topLeftZ;
	int topRightX, topRightZ;
	double bottomLeftY, bottomRightY, topLeftY, topRightY;


	// Multiplier for height. Used for scaling
	double multiplier = 1;

	// Draw Type
	// 0 = Lines (Wireframe)
	// 1 = Triangles
	// 2 = Quads
	int drawType = 1;

	switch (drawType)
	{
		case 0:
			glBegin(GL_LINES);
			break;
		case 1:
			glBegin(GL_TRIANGLES);
			break;
		case 2:
			glBegin(GL_QUADS);
			break;
		
	}

	for (int j = 0; j < img.height() - 1; j++)
	{
		for (int i = 0; i < img.width() - 1; i++)
		{
			// Set all the vertex values
			bottomLeftX = i;
			bottomLeftZ = j;
			bottomLeftY = heightData[bottomLeftX][bottomLeftZ] * multiplier;
			
			bottomRightX = i + 1;
			bottomRightZ = j;
			bottomRightY = heightData[bottomRightX][bottomRightZ] * multiplier;
		
			topRightX = i + 1;
			topRightZ = j + 1;
			topRightY = heightData[topRightX][topRightZ] * multiplier;

			topLeftX = i;
			topLeftZ = j + 1;
			topLeftY = heightData[topLeftX][topLeftZ] * multiplier;
		
			// Negate all the z values
			topLeftZ = -topLeftZ;
			topRightZ = -topRightZ;
			bottomLeftZ = -bottomLeftZ;
			bottomRightZ = -bottomRightZ;

			// Draw the mesh using either Wireframe, Triangles, or Quads
			switch  (drawType)
			{
				// Draw Wireframe
				case 0:
					drawWorldMeshVertex(bottomLeftX, bottomLeftY, bottomLeftZ);
					drawWorldMeshVertex(bottomRightX, bottomRightY, bottomRightZ);
					drawWorldMeshVertex(bottomRightX, bottomRightY, bottomRightZ);
					drawWorldMeshVertex(topLeftX, topLeftY, topLeftZ);
					drawWorldMeshVertex(bottomLeftX, bottomLeftY, bottomLeftZ);
					drawWorldMeshVertex(topLeftX, topLeftY, topLeftZ);
					drawWorldMeshVertex(topLeftX, topLeftY, topLeftZ);
					drawWorldMeshVertex(topRightX, topRightY, topRightZ);	
					drawWorldMeshVertex(bottomRightX, bottomRightY, bottomRightZ);
					drawWorldMeshVertex(topRightX, topRightY, topRightZ);
					break;

				// Draw Triangles
				case 1:
					drawWorldMeshVertex(bottomLeftX, bottomLeftY, bottomLeftZ);
					drawWorldMeshVertex(bottomRightX, bottomRightY, bottomLeftZ);
					drawWorldMeshVertex(topLeftX, topLeftY, topLeftZ);
					drawWorldMeshVertex(topRightX, topRightY, topRightZ);
					drawWorldMeshVertex(bottomRightX, bottomRightY, bottomRightZ);
					drawWorldMeshVertex(topLeftX, topLeftY, topLeftZ);
					break;
				// Draw Quads
				case 2:
					drawWorldMeshVertex(bottomLeftX, bottomLeftY, bottomLeftZ);
					drawWorldMeshVertex(bottomRightX, bottomRightY, bottomRightZ);
					drawWorldMeshVertex(topRightX, topRightY, topRightZ);
					drawWorldMeshVertex(topLeftX, topLeftY, topLeftZ);
				break;
			}
		}
	}
	glEnd();
	glEndList();	

}

// Function used to draw a vertex in the world mesh
// It takes x y and z values, and adjusts the y value based on its value
void Game::drawWorldMeshVertex(double x, double y, double z)
{
	// Set the thresholds for Water, Grass, and Mountain

	double water = 0.1;
	double grass = 0.5;

	// The scale for the y value
	double yScale =	12;

	// Set the color based on height
	if (y < water)
		glColor3f(0, 0, (y/water)+0.5);	
  else if (y < grass)
		glColor3f(0, (y/grass)+0.01, 0);
	else
		glColor3f(y, y, y);

	// Draw the vertex
	glVertex3f(x, y * yScale, z);
}


