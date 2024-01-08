#include<SFML\Graphics.hpp>
#include "game.h"
#include<iostream>
#include <ctime>

int main() {
	srand(time(0));
	game app;
	app.init();
	app.map_set_game();
	app.loop();

	return 0;
}