#include "Game.hpp"
#include "image.hpp"

#include "glm.h"


Texture sbFront, sbBack, sbLeft, sbRight, sbTop;

Game::Game(){
	std::string heightmap;
	heightmap = "aust.png";
	GenerateWorld(heightmap);
	
	setupSkybox();

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
	if (tempY < -127)
		tempY = 0;
	if (tempY > 0)
		tempY = 0;

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

void Game::setupSkybox()
{
	// Load Texture TGA
	LoadTGA(&sbFront, "Textures/Skybox/front.tga");
	LoadTGA(&sbBack, "Textures/Skybox/back.tga");
	LoadTGA(&sbLeft, "Textures/Skybox/left.tga");
	LoadTGA(&sbRight, "Textures/Skybox/right.tga");
	LoadTGA(&sbTop, "Textures/Skybox/top.tga");

	// Load texture data

	glGenTextures(1, &sbFront.texID);
	glBindTexture(GL_TEXTURE_2D, sbFront.texID);
	glTexImage2D(GL_TEXTURE_2D, 0, sbFront.bpp / 8, sbFront.width, sbFront.height, 0, sbFront.type, GL_UNSIGNED_BYTE, sbFront.imageData);

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	glGenTextures(1, &sbBack.texID);
	glBindTexture(GL_TEXTURE_2D, sbBack.texID);
	glTexImage2D(GL_TEXTURE_2D, 0, sbBack.bpp / 8, sbBack.width, sbBack.height, 0, sbBack.type, GL_UNSIGNED_BYTE, sbBack.imageData);

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	glGenTextures(1, &sbLeft.texID);
	glBindTexture(GL_TEXTURE_2D, sbLeft.texID);
	glTexImage2D(GL_TEXTURE_2D, 0, sbLeft.bpp / 8, sbLeft.width, sbLeft.height, 0, sbLeft.type, GL_UNSIGNED_BYTE, sbLeft.imageData);

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	glGenTextures(1, &sbRight.texID);
	glBindTexture(GL_TEXTURE_2D, sbRight.texID);
	glTexImage2D(GL_TEXTURE_2D, 0, sbRight.bpp / 8, sbRight.width, sbRight.height, 0, sbRight.type, GL_UNSIGNED_BYTE, sbRight.imageData);

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	glGenTextures(1, &sbTop.texID);
	glBindTexture(GL_TEXTURE_2D, sbTop.texID);
	glTexImage2D(GL_TEXTURE_2D, 0, sbTop.bpp / 8, sbTop.width, sbTop.height, 0, sbTop.type, GL_UNSIGNED_BYTE, sbTop.imageData);
	
	// Set texture parameters
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	// Create Display List of Skybox
	glNewList(3, GL_COMPILE);

		// Front
		glBindTexture(GL_TEXTURE_2D, sbFront.texID);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(1, 0, 0);
			glTexCoord2f(1, 0); glVertex3f(0, 0, 0);
			glTexCoord2f(1, 1); glVertex3f(0, 1, 0);
			glTexCoord2f(0, 1); glVertex3f(1, 1, 0);
		glEnd();
	
		// Back
		glBindTexture(GL_TEXTURE_2D, sbBack.texID);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(0, 0, -1);
			glTexCoord2f(1, 0); glVertex3f(1, 0, -1);
			glTexCoord2f(1, 1); glVertex3f(1, 1, -1);
			glTexCoord2f(0, 1); glVertex3f(0, 1, -1);		
		glEnd();

		// Left
		glBindTexture(GL_TEXTURE_2D, sbLeft.texID);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(1, 0, -1);
			glTexCoord2f(1, 0); glVertex3f(1, 0, 0);
			glTexCoord2f(1, 1); glVertex3f(1, 1, 0);
			glTexCoord2f(0, 1); glVertex3f(1, 1, -1);		
		glEnd();

		// Right
		glBindTexture(GL_TEXTURE_2D, sbRight.texID);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
			glTexCoord2f(1, 0); glVertex3f(0, 0, -1);
			glTexCoord2f(1, 1); glVertex3f(0, 1, -1);
			glTexCoord2f(0, 1); glVertex3f(0, 1, 0);		
		glEnd();

		// Top
		glBindTexture(GL_TEXTURE_2D, sbTop.texID);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(1, 1, 0);
			glTexCoord2f(1, 0); glVertex3f(0, 1, 0);
			glTexCoord2f(1, 1); glVertex3f(0, 1, -1);
			glTexCoord2f(0, 1); glVertex3f(1, 1, -1);		
		glEnd();

	glEndList();
}

GLuint Game::LoadTexture( const char * filename, int width, int height )
{
    GLuint texture;
    unsigned char * data;
    FILE * file;

    //The following code will read in our RAW file
    file = fopen( filename, "rb" );
    if ( file == NULL ) return 0;
    data = (unsigned char *)malloc( width * height * 3 );
    fread( data, width * height * 3, 1, file );
    fclose( file );

    glGenTextures( 1, &texture ); //generate the texture with the loaded data
    glBindTexture( GL_TEXTURE_2D, texture ); //bind the textureto it’s array
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); //set texture environment parameters

    //here we are setting what textures to use and when. The MIN filter is which quality to show
    //when the texture is near the view, and the MAG filter is which quality to show when the texture
    //is far from the view.

    //The qualities are (in order from worst to best)
    //GL_NEAREST
    //GL_LINEAR
    //GL_LINEAR_MIPMAP_NEAREST
    //GL_LINEAR_MIPMAP_LINEAR

    //And if you go and use extensions, you can use Anisotropic filtering textures which are of an
    //even better quality, but this will do for now.
    //glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    //glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    //Here we are setting the parameter to repeat the texture instead of clamping the texture
    //to the edge of our shape. 
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    //Generate the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    free( data ); //free the texture
    return texture; //return whether it was successfull
}


