#include "Engine.hpp"

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

	const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	if (hasPreviousUpdate_) {
		deltaTime_ = std::chrono::duration<float>(now - lastUpdateTime_).count();
	}
	else {
		deltaTime_ = 0.0f;
		hasPreviousUpdate_ = true;
	}

	lastUpdateTime_ = now;

	int updatesPerSecond = currentScene_ ? currentScene_->getUpdateSpeed() : 1;

	if (currentScene_ && currentScene_->isUpdating()) {
		currentScene_->sceneUpdate();
	}

	InputManager::getInstance().update();

	glutTimerFunc(int(1000 / updatesPerSecond), ENGINEupdateTimer, v);
}

void Engine::recordDrawnFrame() {
	++framesDrawn_;

	const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	const float secondsPassed = std::chrono::duration<float>(now - fpsMeasurementStart_).count();

	if (secondsPassed >= 1.0f) {
		fps_ = static_cast<float>(framesDrawn_) / secondsPassed;

		std::cout << "FPS: " << fps_ << "\n";

		framesDrawn_ = 0;
		fpsMeasurementStart_ = now;
	}
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
	Engine& engine = Engine::getInstance();
	const Scene* scene = engine.getCurrentScene();

	if (scene == nullptr) {
		return;
	}

	const int width = glutGet(GLUT_WINDOW_WIDTH);
	const int height = glutGet(GLUT_WINDOW_HEIGHT);

	if (width <= 0 || height <= 0) {
		return;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (scene->isDrawing()) {
		Renderer& renderer = Renderer::getInstance();

		if (renderer.beginPass(
			scene->getCamera(),
			0, 0,
			width, height
		)) {
			try {
				scene->sceneDraw();
			}
			catch (...) {
				renderer.endPass();
				throw;
			}

			renderer.endPass();
		}
	}

	glFlush();
	glutSwapBuffers();

	engine.recordDrawnFrame();
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

void Engine::createWindow() {
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(ENGINE_WIN_W, ENGINE_WIN_H);

	const int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
	const int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);

	glutInitWindowPosition(
		(screenWidth - ENGINE_WIN_W) / 2,
		(screenHeight - ENGINE_WIN_H) / 2
	);

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutCreateWindow("(Simon Halaszi) (811196947)");
}

void Engine::initOpenGL() {
	if (!gladLoadGL()) {
		throw std::runtime_error("Failed to initialize GLAD");
	}

	glutSwapInterval(0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Engine::printGraphicsInfo() const {
	auto glStr = [](GLenum name) -> const char* {
		const GLubyte* value = glGetString(name);
		return value ? reinterpret_cast<const char*>(value) : "null";
	};

	std::cout << "GL_VENDOR   : " << glStr(GL_VENDOR) << "\n";
	std::cout << "GL_RENDERER : " << glStr(GL_RENDERER) << "\n";
	std::cout << "GL_VERSION  : " << glStr(GL_VERSION) << "\n";
	std::cout << "GLSL_VERSION: " << glStr(GL_SHADING_LANGUAGE_VERSION) << "\n";

	GLint major = 0;
	GLint minor = 0;
	GLint profileMask = 0;

	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profileMask);

	const char* profile = "unknown profile";

	if (profileMask & GL_CONTEXT_CORE_PROFILE_BIT) {
		profile = "core";
	}
	else if (profileMask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) {
		profile = "compat";
	}

	std::cout << "GL Context  : " << major << "." << minor << " (" << profile << ")\n";
}

void Engine::registerInputFuncs() {
	glutSpecialFunc(INPUTMANAGERprocSpecialKeys);
	glutSpecialUpFunc(INPUTMANAGERprocSpecialKeysUp);

	glutKeyboardFunc(INPUTMANAGERprocKeys);
	glutKeyboardUpFunc(INPUTMANAGERprocKeysUp);

	glutMouseFunc(INPUTMANAGERprocMouse);
	glutMotionFunc(INPUTMANAGERmouseMove);
	glutPassiveMotionFunc(INPUTMANAGERpassiveMouseMove);
}

void Engine::registerTimeFuncs() {
	hasPreviousUpdate_ = false;
	deltaTime_ = 0.0f;

	fpsMeasurementStart_ = std::chrono::steady_clock::now();
	framesDrawn_ = 0;
	fps_ = 0.0f;

	glutTimerFunc(0, ENGINEanimationTimer, 0);
	glutTimerFunc(0, ENGINEupdateTimer, 0);
	glutTimerFunc(0, ENGINEframeTimer, 0);
}

void Engine::validateShaders() {
	GLuint colorProgram = EngineShader::loadProgram("../Shaders/Basic.vert", "../Shaders/Color.frag");

	glDeleteProgram(colorProgram);

	GLuint grayscaleProgram = EngineShader::loadProgram("../Shaders/Basic.vert", "../Shaders/Grayscale.frag");

	glDeleteProgram(grayscaleProgram);

	std::cout << "Shaders successfully compiled and linked.\n";
}

#include "TestScene.hpp"

void Engine::init() {
	createWindow();
	initOpenGL();
	printGraphicsInfo();
	registerInputFuncs();

	if (!currentScene_) {
		currentScene_ = std::make_unique<TestScene>();
	}

	currentScene_->sceneInit();

	glutDisplayFunc(GAMEdraw);
	registerTimeFuncs();
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