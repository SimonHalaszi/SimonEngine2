#include "Engine.hpp"
#include <iostream>

int main(int argc, char** argv) {
	glutInit(&argc, argv);

	// LET THE GAMES BEGIN
	Engine::getInstance().init();

	glutMainLoop();

	return 0;
}