#include "Engine.hpp"
#include <iostream>

// P.S. if some of these comments are just straight up wrong its because I did a lot of refactoring

// Look how clean my main is :). Makes me want to cry
int main(int argc, char** argv) {
	glutInit(&argc, argv);

	// LET THE GAMES BEGIN
	Engine::getInstance().init();

	glutMainLoop();

	return 0;
}