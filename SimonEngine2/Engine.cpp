#include "Engine.hpp"

namespace {
	std::chrono::steady_clock::duration timerPeriod(int rate) {
		const auto period = std::chrono::duration_cast<std::chrono::steady_clock::duration>(
			std::chrono::duration<double>(1.0 / rate)
		);
		return std::max(period, std::chrono::steady_clock::duration{ 1 });
	}

	unsigned int timerDelayUntil(std::chrono::steady_clock::time_point deadline) {
		const auto remaining = deadline - std::chrono::steady_clock::now();
		if (remaining <= std::chrono::steady_clock::duration::zero()) {
			return 0;
		}

		const double milliseconds =
			std::chrono::duration<double, std::milli>(remaining).count();
		return static_cast<unsigned int>(std::max(1.0, std::ceil(milliseconds)));
	}

	void ENGINEuncappedIdle() {
		glutPostRedisplay();
	}

	void ENGINEschedulerTimer(int v) {
		Engine::getInstance().schedulerTimer(v);
	}

	void GAMEdraw() {
		Engine& engine = Engine::getInstance();
		engine.refreshTime();
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
				0, 0, width, height,
				engine.getTime(),
				scene->getAmbientColor(), scene->getAmbientIntensity()
			)) {
				try {
					scene->sceneSubmit();
					renderer.drawScene();
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
}

void Engine::changeScene(std::unique_ptr<Scene> newScene) {
	pendingScene_ = std::move(newScene);
}

void Engine::configureFrameScheduling() {
	const bool uncappedRendering = currentScene_ &&
		currentScene_->getFrameSpeed() == Scene::uncappedFrameRate;

	if (uncappedRendering) {
		glutIdleFunc(ENGINEuncappedIdle);
	}
	else {
		glutIdleFunc(nullptr);
	}
}

bool Engine::applyPendingScene() {
	if (!pendingScene_) {
		return false;
	}

	if (currentScene_) {
		currentScene_->sceneDeInit();
	}

	currentScene_ = std::move(pendingScene_);
	InputManager::getInstance().clear();

	if (currentScene_) {
		currentScene_->sceneInit();
	}

	configureFrameScheduling();
	return true;
}

void Engine::schedulerTimer(int v) {
	const auto now = std::chrono::steady_clock::now();

	if (applyPendingScene()) {
		const int updateRate = currentScene_ ? currentScene_->getUpdateSpeed() : 1;
		nextUpdateTime_ = now + timerPeriod(updateRate);
		nextFrameTime_ = now;
	}

	const int updateRate = currentScene_ ? currentScene_->getUpdateSpeed() : 1;
	const auto updatePeriod = timerPeriod(updateRate);

	int updatesRun = 0;
	constexpr int maxCatchUpUpdates = 5;

	while (updatesRun < maxCatchUpUpdates) {
		const auto updateNow = std::chrono::steady_clock::now();
		if (updateNow < nextUpdateTime_) {
			break;
		}

		refreshTime();
		deltaTime_ = std::chrono::duration<float>(updatePeriod).count();

		if (currentScene_ && currentScene_->isUpdating()) {
			currentScene_->sceneUpdate();
		}
		InputManager::getInstance().update();

		nextUpdateTime_ += updatePeriod;
		++updatesRun;

		if (applyPendingScene()) {
			const auto sceneChangeTime = std::chrono::steady_clock::now();
			const int newUpdateRate = currentScene_ ? currentScene_->getUpdateSpeed() : 1;
			nextUpdateTime_ = sceneChangeTime + timerPeriod(newUpdateRate);
			nextFrameTime_ = sceneChangeTime;
			break;
		}
	}

	const auto scheduleTime = std::chrono::steady_clock::now();
	if (updatesRun == maxCatchUpUpdates && scheduleTime >= nextUpdateTime_) {
		nextUpdateTime_ = scheduleTime + updatePeriod;
	}

	const bool uncappedRendering = currentScene_ && currentScene_->getFrameSpeed() == Scene::uncappedFrameRate;

	if (!uncappedRendering) {
		const int frameRate = currentScene_ ? currentScene_->getFrameSpeed() : 1;
		const auto framePeriod = timerPeriod(frameRate);

		if (scheduleTime >= nextFrameTime_) {
			glutPostRedisplay();

			const auto missedPeriods = (scheduleTime - nextFrameTime_) / framePeriod + 1;
			nextFrameTime_ += framePeriod * missedPeriods;
		}
	}

	const auto nextDeadline = uncappedRendering ? nextUpdateTime_ : std::min(nextUpdateTime_, nextFrameTime_);

	glutTimerFunc(timerDelayUntil(nextDeadline), ENGINEschedulerTimer, v);
}

void Engine::recordDrawnFrame() {
	++framesDrawn_;

	const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	const float secondsPassed = std::chrono::duration<float>(now - fpsMeasurementStart_).count();

	if (secondsPassed >= 1.0f) {
		fps_ = static_cast<float>(framesDrawn_) / secondsPassed;

		framesDrawn_ = 0;
		fpsMeasurementStart_ = now;
	}
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
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutCreateWindow("(Simon Halaszi) (811196947)");
}

void Engine::initOpenGL() {
	if (!gladLoadGL()) {
		throw std::runtime_error("Failed to initialize GLAD");
	}

	GLenum startupError;
	while ((startupError = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL error after window creation: " << startupError << std::endl;
	}

	glutSwapInterval(0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

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
	std::cout << "FreeGLUT    : " << glutGet(GLUT_VERSION) << "\n";

	GLint major = 0;
	GLint minor = 0;
	GLint profileMask = 0;

	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profileMask);

	const char* profile = "Unknown profile";

	if (profileMask & GL_CONTEXT_CORE_PROFILE_BIT) {
		profile = "Core";
	}
	else if (profileMask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) {
		profile = "Compatibility";
	}

	std::cout << "GL Context  : " << major << "." << minor << " (" << profile << ")\n";
	std::cout << std::endl;
}

void Engine::registerInputFuncs() {
	glutKeyboardFunc(INPUTMANAGERprocKeys);
	glutKeyboardUpFunc(INPUTMANAGERprocKeysUp);
	glutSpecialFunc(INPUTMANAGERprocSpecialKeys);
	glutSpecialUpFunc(INPUTMANAGERprocSpecialKeysUp);
	glutMouseFunc(INPUTMANAGERprocMouse);
	glutMotionFunc(INPUTMANAGERmouseMove);
	glutPassiveMotionFunc(INPUTMANAGERpassiveMouseMove);
}

void Engine::registerTimeFuncs() {
	deltaTime_ = 0.0f;

	fpsMeasurementStart_ = std::chrono::steady_clock::now();
	framesDrawn_ = 0;
	fps_ = 0.0f;

	refreshTime();

	const auto now = std::chrono::steady_clock::now();
	const int updateRate = currentScene_ ? currentScene_->getUpdateSpeed() : 1;
	nextUpdateTime_ = now + timerPeriod(updateRate);
	nextFrameTime_ = now;

	configureFrameScheduling();

	glutTimerFunc(0, ENGINEschedulerTimer, 0);
}

void Engine::refreshTime() {
	const auto now = std::chrono::steady_clock::now();

	if (!hasStartTime_) {
		startTime_ = now;
		hasStartTime_ = true;
	}

	time_ = std::chrono::duration<float>(now - startTime_).count();
}

#include "RobotScene.hpp"

void Engine::init() {
	createWindow();
	initOpenGL();
	printGraphicsInfo();
	registerInputFuncs();

	if (!currentScene_) {
		std::cout << "Engine::init : Please wait while start up scene loads assets\n" << std::endl;

		currentScene_ = std::make_unique<RobotScene>();
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

	std::cout << "Engine::safeEngineExit: Exited game\n";
	exit(0);
}