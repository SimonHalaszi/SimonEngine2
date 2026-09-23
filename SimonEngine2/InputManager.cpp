#include "InputManager.hpp"
#include <iostream>
#include <cctype>

void InputManager::keyDown(unsigned char key) { keyCurrent_[key] = true; }
void InputManager::keyUp(unsigned char key) { keyCurrent_[key] = false; }

bool InputManager::isDown(unsigned char key)      const { return keyCurrent_[key]; }
bool InputManager::isPressed(unsigned char key)   const { return keyCurrent_[key] && !keyPrevious_[key]; }
bool InputManager::isReleased(unsigned char key)  const { return !keyCurrent_[key] && keyPrevious_[key]; }

void InputManager::specialKeyDown(int key) {
    if (key >= 0 && key < int(specialKeyCurrent_.size())) {
        specialKeyCurrent_[key] = true;
    }
}
void InputManager::specialKeyUp(int key) {
    if (key >= 0 && key < int(specialKeyCurrent_.size())) {
        specialKeyCurrent_[key] = false;
    }
}

bool InputManager::isSpecialKeyDown(int key) const {
    if (key >= 0 && key < int(specialKeyCurrent_.size())) {
        return specialKeyCurrent_[key];
    }
    else {
        return false;
    }
}
bool InputManager::isSpecialKeyPressed(int key) const {
    if (key >= 0 && key < int(specialKeyCurrent_.size())) {
        return (specialKeyCurrent_[key] && !specialKeyPrevious_[key]);
    }
    else {
        return false;
    }
}
bool InputManager::isSpecialKeyReleased(int key)  const {
    if (key >= 0 && key < int(specialKeyCurrent_.size())) {
        return (!specialKeyCurrent_[key] && specialKeyPrevious_[key]);
    }
    else {
        return false;
    }
}

void InputManager::mouseButtonDown(int button, int x, int y) {
    if (button >= 0 && button < int(mouseCurrent_.size())) {
        mouseCurrent_[button] = true;
    }
}
void InputManager::mouseButtonUp(int button, int x, int y) {
    if (button >= 0 && button < int(mouseCurrent_.size())) {
        mouseCurrent_[button] = false;
    }
}

bool InputManager::isMouseButtonDown(MouseButton button) const {
    if (button == MOUSEBUTTON_SCROLLUP) {
        return scrollUpThisFrame_;
    }
    if (button == MOUSEBUTTON_SCROLLDOWN) {
        return scrollDownThisFrame_;
    }
    return mouseCurrent_[button];
}

bool InputManager::isMouseButtonPressed(MouseButton button) const {
    if (button == MOUSEBUTTON_SCROLLUP) {
        return scrollUpThisFrame_;
    }
    if (button == MOUSEBUTTON_SCROLLDOWN) {
        return scrollDownThisFrame_;
    }
    return mouseCurrent_[button] && !mousePrevious_[button];
}

bool InputManager::isMouseButtonReleased(MouseButton button) const {
    if (button == MOUSEBUTTON_SCROLLUP || button == MOUSEBUTTON_SCROLLDOWN) {
        return false;
    }
    return !mouseCurrent_[button] && mousePrevious_[button];
}

void InputManager::mouseMove(int x, int y) {
    mouseDeltaX_ = x - mouseX_;
    mouseDeltaY_ = y - mouseY_;
    mouseX_ = x;
    mouseY_ = y;
}

int InputManager::mouseX()        const { return mouseX_; }
int InputManager::mouseY()        const { return mouseY_; }
int InputManager::mouseDeltaX()   const { return mouseDeltaX_; }
int InputManager::mouseDeltaY()   const { return mouseDeltaY_; }

void InputManager::queueTypedChar(unsigned char key) { typedChars_ += static_cast<char>(key); }
const std::string& InputManager::getTypedChars() const { return typedChars_; }

void InputManager::registerScroll(int button) {
    if (button == 3) {
        scrollUpThisFrame_ = true;
    }
    if (button == 4) {
        scrollDownThisFrame_ = true;
    }
}

void InputManager::update() {
    typedChars_.clear();
    scrollUpThisFrame_ = false;
    scrollDownThisFrame_ = false;

    keyPrevious_ = keyCurrent_;
    mousePrevious_ = mouseCurrent_;
    specialKeyPrevious_ = specialKeyCurrent_;

    mouseDeltaX_ = mouseDeltaY_ = 0;
}

void INPUTMANAGERprocSpecialKeys(int key, int x, int y) {
    int k = mapSpecialKey(key);
    if (k != -1) {
        InputManager::getInstance().specialKeyDown(k);
    }
}

void INPUTMANAGERprocSpecialKeysUp(int key, int x, int y) {
    int k = mapSpecialKey(key);
    if (k != -1) {
        InputManager::getInstance().specialKeyUp(k);
    }
}

void INPUTMANAGERprocKeys(unsigned char key, int x, int y) {
    int code = static_cast<int>(key);
    char ch = std::isprint(static_cast<unsigned char>(key)) ? static_cast<char>(key) : '?';
    InputManager::getInstance().keyDown(key);
    InputManager::getInstance().queueTypedChar(key);
}

void INPUTMANAGERprocKeysUp(unsigned char key, int x, int y) {
    int code = static_cast<int>(key);
    char ch = std::isprint(static_cast<unsigned char>(key)) ? static_cast<char>(key) : '?';
    InputManager::getInstance().keyUp(key);
}

void INPUTMANAGERprocMouse(int button, int state, int x, int y) {
    if (button == 3 || button == 4) {
        bool up = (button == 3);
        InputManager::getInstance().registerScroll(button);
        return;
    }

    if (state == GLUT_DOWN) {
        InputManager::getInstance().mouseButtonDown(button, x, y);
    }
    else {
        InputManager::getInstance().mouseButtonUp(button, x, y);
    }
}

void INPUTMANAGERmouseMove(int x, int y) {
    InputManager::getInstance().mouseMove(x, y);
}

void INPUTMANAGERpassiveMouseMove(int x, int y) {
    InputManager::getInstance().mouseMove(x, y);
}

int mapSpecialKey(int key) {
    switch (key) {
    case GLUT_KEY_LEFT:   return 0;
    case GLUT_KEY_UP:     return 1;
    case GLUT_KEY_RIGHT:  return 2;
    case GLUT_KEY_DOWN:   return 3;
    case GLUT_KEY_F1:     return 4;
    case GLUT_KEY_F2:     return 5;
    case GLUT_KEY_F3:     return 6;
    case GLUT_KEY_F4:     return 7;
    case GLUT_KEY_F5:     return 8;
    case GLUT_KEY_F6:     return 9;
    case GLUT_KEY_F7:     return 10;
    case GLUT_KEY_F8:     return 11;
    case GLUT_KEY_F9:     return 12;
    case GLUT_KEY_F10:    return 13;
    case GLUT_KEY_F11:    return 14;
    case GLUT_KEY_F12:    return 15;
    default: return -1;
    }
}