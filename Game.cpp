#include "Game.h"
#include "image.hpp"

Game::Game(){
	

	std::string heightmap;
	heightmap = "aust.png";
	GenerateWorld(heightmap);

}

Game:: ~Game(){

}

void Game::GenerateWorld(std::string file) {

	// Load the image
	Image img(128, 128, 3);
	img.loadPng(file);

	for (int i = 0; i < img.width(); i++)
	{
		for (int j = 0; j < img.height(); j++)
		{
			if (img(i, j, 0) != 0)
			std::cerr << "Color at: (" << i << "," << j << ") = "  << img(i, j, 0) << std::endl;
		}
	}
	

}
