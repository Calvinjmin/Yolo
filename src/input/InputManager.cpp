#include "InputManager.h"
#include <cstdio>
#include <iostream>

InputManager::InputManager() {
    InitializeKeyBindings();
}

void InputManager::InitializeKeyBindings() {
    key_bindings_[SDLK_w] = InputAction::MOVE_UP;
    key_bindings_[SDLK_UP] = InputAction::MOVE_UP;
    key_bindings_[SDLK_s] = InputAction::MOVE_DOWN;
    key_bindings_[SDLK_DOWN] = InputAction::MOVE_DOWN;
    key_bindings_[SDLK_a] = InputAction::MOVE_LEFT;
    key_bindings_[SDLK_LEFT] = InputAction::MOVE_LEFT;
    key_bindings_[SDLK_d] = InputAction::MOVE_RIGHT;
    key_bindings_[SDLK_RIGHT] = InputAction::MOVE_RIGHT;
    key_bindings_[SDLK_SPACE] = InputAction::INTERACT;
    key_bindings_[SDLK_e] = InputAction::USE_TOOL;
    key_bindings_[SDLK_i] = InputAction::INVENTORY;
    key_bindings_[SDLK_ESCAPE] = InputAction::MENU;
    key_bindings_[SDLK_q] = InputAction::MENU; // Q closes menus/dialogues
}

void InputManager::Update() {
    previous_state_ = current_state_;
}

void InputManager::HandleEvent(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        // Check for CMD+Q (or CTRL+Q on non-Mac)
        if (event.key.keysym.sym == SDLK_q && (event.key.keysym.mod & KMOD_GUI)) {
            current_state_[InputAction::QUIT] = true;
        }
        // Regular key bindings
        else {
            auto it = key_bindings_.find(event.key.keysym.sym);
            if (it != key_bindings_.end()) {
                current_state_[it->second] = true;
            }
        }
    }
    else if (event.type == SDL_KEYUP) {
        if (event.key.keysym.sym == SDLK_q && (event.key.keysym.mod & KMOD_GUI)) {
            current_state_[InputAction::QUIT] = false;
        }
        else {
            auto it = key_bindings_.find(event.key.keysym.sym);
            if (it != key_bindings_.end()) {
                current_state_[it->second] = false;
            }
        }
    }
}

bool InputManager::IsActionPressed(InputAction action) const {
    auto currentIt = current_state_.find(action);
    auto previousIt = previous_state_.find(action);
    
    bool currentPressed = (currentIt != current_state_.end()) ? currentIt->second : false;
    bool previousPressed = (previousIt != previous_state_.end()) ? previousIt->second : false;
    
    
    return currentPressed && !previousPressed;
}

bool InputManager::IsActionHeld(InputAction action) const {
    auto it = current_state_.find(action);
    return (it != current_state_.end()) ? it->second : false;
}

bool InputManager::IsActionReleased(InputAction action) const {
    auto currentIt = current_state_.find(action);
    auto previousIt = previous_state_.find(action);
    
    bool currentPressed = (currentIt != current_state_.end()) ? currentIt->second : false;
    bool previousPressed = (previousIt != previous_state_.end()) ? previousIt->second : false;
    
    return !currentPressed && previousPressed;
}