#include "Player.h"
#include "InputManager.h"
#include "Renderer.h"
#include <cstdio>

Player::Player() 
    : position_(4608.0f, 3456.0f), velocity_(0.0f, 0.0f), speed_(200.0f) {
    // Start at center of 9x9 world
}

Player::~Player() {
}

void Player::HandleInput(InputManager* input_manager) {
    velocity_ = Vector2(0.0f, 0.0f);
    
    bool moving = false;
    if (input_manager->IsActionHeld(InputAction::MOVE_LEFT)) {
        velocity_.x -= speed_;
        moving = true;
    }
    if (input_manager->IsActionHeld(InputAction::MOVE_RIGHT)) {
        velocity_.x += speed_;
        moving = true;
    }
    if (input_manager->IsActionHeld(InputAction::MOVE_UP)) {
        velocity_.y -= speed_;
        moving = true;
    }
    if (input_manager->IsActionHeld(InputAction::MOVE_DOWN)) {
        velocity_.y += speed_;
        moving = true;
    }
    
}

void Player::Update(float deltaTime) {
    Vector2 oldPos = position_;
    position_.x += velocity_.x * deltaTime;
    position_.y += velocity_.y * deltaTime;
    
    // World boundaries (9x larger than screen)
    const int WORLD_WIDTH = 9216;  // 1024 * 9
    const int WORLD_HEIGHT = 6912; // 768 * 9
    
    if (position_.x < 0) position_.x = 0;
    if (position_.y < 0) position_.y = 0;
    if (position_.x > WORLD_WIDTH - PLAYER_WIDTH) position_.x = WORLD_WIDTH - PLAYER_WIDTH;
    if (position_.y > WORLD_HEIGHT - PLAYER_HEIGHT) position_.y = WORLD_HEIGHT - PLAYER_HEIGHT;
    
}

void Player::Render(Renderer* renderer, const Vector2& cameraOffset) {
    // Convert world position to screen position
    Vector2 screenPos(position_.x - cameraOffset.x, position_.y - cameraOffset.y);
    
    Rect playerRect(
        static_cast<int>(screenPos.x),
        static_cast<int>(screenPos.y),
        PLAYER_WIDTH,
        PLAYER_HEIGHT
    );
    
    SDL_Color green = {0, 255, 0, 255};
    renderer->DrawRect(playerRect, green);
    
}