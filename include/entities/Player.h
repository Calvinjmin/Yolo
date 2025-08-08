#pragma once
#include <SDL.h>
#include "Renderer.h"

class InputManager;


class Player {
public:
    Player();
    ~Player();
    
    void HandleInput(InputManager* input_manager);
    void Update(float deltaTime);
    void Render(Renderer* renderer, const Vector2& cameraOffset);
    
    Vector2 GetPosition() const { return position_; }
    void SetPosition(const Vector2& position) { position_ = position; }
    
private:
    Vector2 position_;
    Vector2 velocity_;
    float speed_;
    
    const int PLAYER_WIDTH = 32;
    const int PLAYER_HEIGHT = 32;
};