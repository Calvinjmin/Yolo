#pragma once
#include <SDL.h>
#include <unordered_map>

enum class InputAction {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    INTERACT,
    USE_TOOL,
    INVENTORY,
    MENU,
    QUIT
};

class InputManager {
public:
    InputManager();
    
    void Update();
    void HandleEvent(const SDL_Event& event);
    
    bool IsActionPressed(InputAction action) const;
    bool IsActionHeld(InputAction action) const;
    bool IsActionReleased(InputAction action) const;
    
private:
    std::unordered_map<SDL_Keycode, InputAction> key_bindings_;
    std::unordered_map<InputAction, bool> current_state_;
    std::unordered_map<InputAction, bool> previous_state_;
    
    void InitializeKeyBindings();
};