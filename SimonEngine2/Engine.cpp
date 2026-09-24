#include "Engine.hpp"
#include "WindowConstants.hpp"

// Function wrappers needed because OpenGL expects a certain function signature for these
void ENGINEanimationTimer(int v) {
	Engine::getInstance().animationTimer(v);
}
void ENGINEupdateTimer(int v) {
	Engine::getInstance().updateTimer(v);
}
void ENGINEframeTimer(int v) {
	Engine::getInstance().frameTimer(v);
}

void Engine::changeScene(std::unique_ptr<Scene> newScene) {
	pendingScene_ = std::move(newScene);
}

void Engine::updateTimer(int v) {
	int updatesPerSecond = 1;

	if (pendingScene_) {
		if (currentScene_) {
			currentScene_->sceneDeInit();
		}

		currentScene_ = std::move(pendingScene_);

		if (currentScene_) {
			currentScene_->sceneInit();
		}
	}

	if (currentScene_) {
		updatesPerSecond = currentScene_->getUpdateSpeed();
	}

	if (currentScene_ && currentScene_->isUpdating()) {
		currentScene_->sceneUpdate();
	}

	// Input manager updates.
	InputManager::getInstance().update();

	glutTimerFunc(int(1000 / updatesPerSecond), ENGINEupdateTimer, v);
}

void Engine::frameTimer(int v) {
	glutPostRedisplay();
	int frameUpdatesPerSecond = 1;
	if (currentScene_) {
		frameUpdatesPerSecond = currentScene_->getFrameSpeed();
	}
	glutTimerFunc(int(1000 / frameUpdatesPerSecond), ENGINEframeTimer, v); // Updates
}

void GAMEdraw() {
	const Scene* currentScene =
		Engine::getInstance().getCurrentScene();

	if (!currentScene) {
		return;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	if (currentScene->isDrawing()) {
		glViewport(0, 0, ENGINE_WIN_W, ENGINE_WIN_H);
		currentScene->sceneDraw();
	}

	glFlush();
	glutSwapBuffers();
}

void Engine::animationTimer(int v) {
	int animationUpdatesPerSecond = 1;
	if (currentScene_) {
		animationUpdatesPerSecond = currentScene_->getAnimationUpdateSpeed();
		if (currentScene_->isUpdatingAnimations()) {
			currentScene_->incrementAnimationFrame();
		}
	}
	glutTimerFunc(int(1000 / animationUpdatesPerSecond), ENGINEanimationTimer, v); // Creates a frame delay that is counted in miliseconds
}

#include "BasicScene.hpp"

void Engine::init() {
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	glutInitWindowSize(ENGINE_WIN_W, ENGINE_WIN_H);
	const int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
	const int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
	glutInitWindowPosition((screenWidth - ENGINE_WIN_W) / 2, (screenHeight - ENGINE_WIN_H) / 2);
	
	glutCreateWindow("(Simon Halaszi) (811196947)");

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glutSpecialFunc(INPUTMANAGERprocSpecialKeys);
	glutSpecialUpFunc(INPUTMANAGERprocSpecialKeysUp);
	glutKeyboardFunc(INPUTMANAGERprocKeys);
	glutKeyboardUpFunc(INPUTMANAGERprocKeysUp);
	glutMouseFunc(INPUTMANAGERprocMouse);
	glutMotionFunc(INPUTMANAGERmouseMove);
	glutPassiveMotionFunc(INPUTMANAGERpassiveMouseMove);

	if (!currentScene_) {
		currentScene_ = std::make_unique<BasicScene>();
	}

	if (currentScene_) {
		currentScene_->sceneInit();
	}

	glutDisplayFunc(GAMEdraw);

	glutTimerFunc(0, ENGINEanimationTimer, 0);
	glutTimerFunc(0, ENGINEupdateTimer, 0);
	glutTimerFunc(0, ENGINEframeTimer, 0);
}

void Engine::safeExit() {
	// sceneDeInit safely frees all resources allocated and all managers
	// If we dont deInit scene before exit we can get some weird errors
	if (currentScene_) {
		currentScene_->sceneDeInit();
	}
	std::cout << "Engine::safeEngineExit : Exited game\n";
	exit(0);
}