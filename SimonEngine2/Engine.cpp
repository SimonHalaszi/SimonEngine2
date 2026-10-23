#include "Engine.hpp"
#include "WindowConstants.hpp"
#include "ChuckEdition.hpp" // the drip

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

void Engine::applyPendingScene() {
	if (!pendingScene_) {
		return;
	}

	if (currentScene_) {
		currentScene_->sceneDeInit();
	}

	currentScene_ = std::move(pendingScene_);
	InputManager::getInstance().reset();

	if (currentScene_) {
		currentScene_->sceneInit();
	}
}

void Engine::updateTimer(int v) {
	applyPendingScene();

	int updatesPerSecond = currentScene_ ? currentScene_->getUpdateSpeed() : 1;

	// Chuck Edition reads input BEFORE the scene does, so the splash screen can swallow
	// the key you hit to skip it (instead of that key also toggling wireframe or whatever)
	ChuckEdition::getInstance().update(currentScene_.get());

	if (currentScene_ && currentScene_->isUpdating()) {
		currentScene_->sceneUpdate();
	}

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
	const Scene* currentScene = Engine::getInstance().getCurrentScene();

	if (!currentScene) {
		return;
	}

	ChuckEdition& chuck = ChuckEdition::getInstance();
	chuck.beginFrame();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Sky, synthwave sun, stars... painted straight onto the cleared screen, no depth,
	// so the actual 3D scene always lands on top of it
	chuck.drawBackdrop();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	if (currentScene->isDrawing()) {
		glViewport(0, 0, ENGINE_WIN_W, ENGINE_WIN_H);
		currentScene->sceneDraw();
	}

	// HUD, notifications and post effects ride on top of everything
	chuck.drawOverlay(*currentScene);

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

#include "RobotScene.hpp"

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

	// Games want "held" to mean held. Without this, X11 auto-repeat delivers a release +
	// press pair ~30 times a second while a key is held, so WASD flickers and the input
	// latches would see fresh presses. (Windows never re-fired isPressed on repeat anyway.)
	glutIgnoreKeyRepeat(1);

	glutSpecialFunc(INPUTMANAGERprocSpecialKeys);
	glutSpecialUpFunc(INPUTMANAGERprocSpecialKeysUp);
	glutKeyboardFunc(INPUTMANAGERprocKeys);
	glutKeyboardUpFunc(INPUTMANAGERprocKeysUp);
	glutMouseFunc(INPUTMANAGERprocMouse);
	glutMotionFunc(INPUTMANAGERmouseMove);
	glutPassiveMotionFunc(INPUTMANAGERpassiveMouseMove);

	if (!currentScene_) {
		currentScene_ = std::make_unique<RobotScene>();
	}

	if (currentScene_) {
		currentScene_->sceneInit();
	}

	glutDisplayFunc(GAMEdraw);

	glutTimerFunc(0, ENGINEanimationTimer, 0);
	glutTimerFunc(0, ENGINEupdateTimer, 0);
	glutTimerFunc(0, ENGINEframeTimer, 0);

	// Window + GL context exist now, so the special edition can boot up
	ChuckEdition::getInstance().init();

}

void Engine::safeExit() {
	// sceneDeInit safely frees all resources allocated and all managers
	// If we dont deInit scene before exit we can get some weird errors
	if (currentScene_) {
		currentScene_->sceneDeInit();
	}
	std::cout << "Engine::safeEngineExit : Exited game\n";
	std::cout << "GG WP. Thanks for playing SimonEngine2: CHUCK SPECIAL EDITION\n";
	exit(0);
}