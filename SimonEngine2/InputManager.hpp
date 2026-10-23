#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include <GL/freeglut.h>
#include <array>
#include <string>

enum KeyState { KEYSTATE_UP, KEYSTATE_DOWN, KEYSTATE_PRESSED, KEYSTATE_RELEASED };
enum MouseButton { MOUSEBUTTON_LEFT = 0, MOUSEBUTTON_MIDDLE = 1, MOUSEBUTTON_RIGHT = 2, MOUSEBUTTON_SCROLLUP = 3, MOUSEBUTTON_SCROLLDOWN = 4 };

class InputManager {
    public:
        static InputManager& getInstance() {
            static InputManager instance;
            return instance;
        }

        void keyDown(unsigned char key);
        void keyUp(unsigned char key);

        bool isDown(unsigned char key) const;
        bool isPressed(unsigned char key) const;
        bool isReleased(unsigned char key) const;

        void specialKeyDown(int key);
        void specialKeyUp(int key);
    
        // These take a real GLUT_KEY_* code (GLUT_KEY_F4, GLUT_KEY_LEFT, ...), NOT a remapped slot
        bool isSpecialKeyDown(int glutKey) const;
        bool isSpecialKeyPressed(int glutKey) const;
        bool isSpecialKeyReleased(int glutKey)  const;
    
        void mouseButtonDown(int button, int x, int y);
        void mouseButtonUp(int button, int x, int y);

        bool isMouseButtonDown(MouseButton button) const;
        bool isMouseButtonPressed(MouseButton button) const;
        bool isMouseButtonReleased(MouseButton button) const;

        void mouseMove(int x, int y);

        int mouseX() const;
        int mouseY() const;
        int mouseDeltaX() const;
        int mouseDeltaY() const;

        void queueTypedChar(unsigned char key);
        const std::string& getTypedChars() const;

        void registerScroll(int button);

        void update();
        void reset();

        InputManager(const InputManager&) = delete;
        InputManager& operator=(const InputManager&) = delete;
        InputManager(const InputManager&&) = delete;
        InputManager& operator=(const InputManager&&) = delete;

    private:
        InputManager() : mouseX_(0), mouseY_(0), mouseDeltaX_(0), mouseDeltaY_(0) {}
        ~InputManager() {}

        std::string typedChars_;
        bool scrollUpThisFrame_ = false;
        bool scrollDownThisFrame_ = false;

        std::array<bool, 256> keyCurrent_ = {};
        std::array<bool, 256> keyPrevious_ = {};

        std::array<bool, 8> mouseCurrent_ = {};
        std::array<bool, 8> mousePrevious_ = {};

        // 12 F-keys + 4 arrows + PgUp/PgDn/Home/End/Insert, with headroom for future DLC
        std::array<bool, 32> specialKeyCurrent_ = {};
        std::array<bool, 32> specialKeyPrevious_ = {};

        // "Did it go down / up at ANY point since the last update?" latches.
        // Without these, a quick tap whose press AND release both land between two update
        // ticks is never seen (current == previous == false). Happens on slow frames, with
        // macro keyboards, and with automated input tools. Cleared every update().
        std::array<bool, 256> keyPressLatch_ = {};
        std::array<bool, 256> keyReleaseLatch_ = {};
        std::array<bool, 8> mousePressLatch_ = {};
        std::array<bool, 8> mouseReleaseLatch_ = {};
        std::array<bool, 32> specialKeyPressLatch_ = {};
        std::array<bool, 32> specialKeyReleaseLatch_ = {};

        int mouseX_, mouseY_;
        int mouseDeltaX_, mouseDeltaY_;
};

void INPUTMANAGERprocSpecialKeys(int key, int x, int y);
void INPUTMANAGERprocSpecialKeysUp(int key, int x, int y);
void INPUTMANAGERprocKeys(unsigned char key, int x, int y);
void INPUTMANAGERprocKeysUp(unsigned char key, int x, int y);
void INPUTMANAGERprocMouse(int button, int state, int x, int y);
void INPUTMANAGERmouseMove(int x, int y);
void INPUTMANAGERpassiveMouseMove(int x, int y);
int mapSpecialKey(int key);

#endif