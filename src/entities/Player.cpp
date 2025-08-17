#include "Player.h"
#include "InputManager.h"
#include "Renderer.h"
#include <cstdio>

Player::Player() 
    : position_(640.0f, 512.0f), velocity_(0.0f, 0.0f), speed_(200.0f), externalCollisionCheck_(nullptr) {
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
    Vector2 newPosition = position_;
    newPosition.x += velocity_.x * deltaTime;
    newPosition.y += velocity_.y * deltaTime;
    
    // World boundaries (compact world)
    const int WORLD_WIDTH = 1280;  // 10 tiles * 128px
    const int WORLD_HEIGHT = 1024; // 8 tiles * 128px
    
    // Basic world boundary checks
    if (newPosition.x < 0) newPosition.x = 0;
    if (newPosition.y < 0) newPosition.y = 0;
    if (newPosition.x > WORLD_WIDTH - PLAYER_WIDTH) newPosition.x = WORLD_WIDTH - PLAYER_WIDTH;
    if (newPosition.y > WORLD_HEIGHT - PLAYER_HEIGHT) newPosition.y = WORLD_HEIGHT - PLAYER_HEIGHT;
    
    // Check for collisions with objects
    if (!CheckCollision(newPosition)) {
        position_ = newPosition;
    }
    // If collision detected, player stays at old position
}

void Player::Render(Renderer* renderer, const Vector2& cameraOffset) {
    // Player colors - using green theme to distinguish from blue NPCs
    SDL_Color playerGreen = {60, 180, 75, 255};        // Main body color
    SDL_Color playerHighlight = {90, 210, 105, 255};   // Highlight color
    SDL_Color playerShadow = {40, 120, 50, 255};       // Shadow color
    
    // Player shadow (offset for depth)
    Rect shadowRect(
        static_cast<int>(position_.x - cameraOffset.x + 4),
        static_cast<int>(position_.y - cameraOffset.y + 4),
        PLAYER_WIDTH,
        PLAYER_HEIGHT
    );
    renderer->DrawRect(shadowRect, SDL_Color{0, 0, 0, 60});
    
    // Main player body
    Rect playerRect(
        static_cast<int>(position_.x - cameraOffset.x),
        static_cast<int>(position_.y - cameraOffset.y),
        PLAYER_WIDTH,
        PLAYER_HEIGHT
    );
    renderer->DrawRect(playerRect, playerGreen);
    
    // Player highlight (top edge for 3D effect)
    Rect highlightRect(
        static_cast<int>(position_.x - cameraOffset.x),
        static_cast<int>(position_.y - cameraOffset.y),
        PLAYER_WIDTH,
        6
    );
    renderer->DrawRect(highlightRect, playerHighlight);
    
    // Player depth edge (right side for 3D effect)
    Rect depthRect(
        static_cast<int>(position_.x - cameraOffset.x + PLAYER_WIDTH - 4),
        static_cast<int>(position_.y - cameraOffset.y + 6),
        4,
        PLAYER_HEIGHT - 6
    );
    renderer->DrawRect(depthRect, playerShadow);
    
    // Character face - eyes
    Rect leftEye(
        static_cast<int>(position_.x - cameraOffset.x + 8),
        static_cast<int>(position_.y - cameraOffset.y + 10),
        4, 4
    );
    Rect rightEye(
        static_cast<int>(position_.x - cameraOffset.x + 20),
        static_cast<int>(position_.y - cameraOffset.y + 10),
        4, 4
    );
    renderer->DrawRect(leftEye, SDL_Color{255, 255, 255, 255});
    renderer->DrawRect(rightEye, SDL_Color{255, 255, 255, 255});
    
    // Eye pupils
    Rect leftPupil(
        static_cast<int>(position_.x - cameraOffset.x + 9),
        static_cast<int>(position_.y - cameraOffset.y + 11),
        2, 2
    );
    Rect rightPupil(
        static_cast<int>(position_.x - cameraOffset.x + 21),
        static_cast<int>(position_.y - cameraOffset.y + 11),
        2, 2
    );
    renderer->DrawRect(leftPupil, SDL_Color{0, 0, 0, 255});
    renderer->DrawRect(rightPupil, SDL_Color{0, 0, 0, 255});
    
    // Optional: Add a simple mouth for more character
    Rect mouth(
        static_cast<int>(position_.x - cameraOffset.x + 12),
        static_cast<int>(position_.y - cameraOffset.y + 20),
        8, 2
    );
    renderer->DrawRect(mouth, SDL_Color{40, 40, 40, 255});
}

bool Player::CheckCollision(const Vector2& newPosition) const {
    const int TILE_SIZE = 128;
    
    // Player collision rectangle
    Rect playerRect(
        static_cast<int>(newPosition.x),
        static_cast<int>(newPosition.y),
        PLAYER_WIDTH,
        PLAYER_HEIGHT
    );
    
    // Water borders (impassable)
    // Top water border
    if (playerRect.y < TILE_SIZE) return true;
    // Bottom water border  
    if (playerRect.y + playerRect.h > 7 * TILE_SIZE) return true;
    // Left water border
    if (playerRect.x < TILE_SIZE) return true;
    // Right water border
    if (playerRect.x + playerRect.w > 9 * TILE_SIZE) return true;
    
    // House collision (tiles 2,2 to 3,3)
    Rect houseRect(2 * TILE_SIZE, 2 * TILE_SIZE, 2 * TILE_SIZE, 2 * TILE_SIZE);
    if (playerRect.x < houseRect.x + houseRect.w && 
        playerRect.x + playerRect.w > houseRect.x && 
        playerRect.y < houseRect.y + houseRect.h && 
        playerRect.y + playerRect.h > houseRect.y) {
        return true;
    }
    
    // Farm area - now fully walkable (dynamic objects handle their own collision)
    
    // Garden area - now fully walkable (dynamic objects handle their own collision)
    
    // Very minimal bushes - only a few strategic ones
    // Bush at (3,5) - corner
    Rect bush1(3 * TILE_SIZE + 50, 5 * TILE_SIZE + 50, 30, 20);
    if (playerRect.x < bush1.x + bush1.w && 
        playerRect.x + playerRect.w > bush1.x && 
        playerRect.y < bush1.y + bush1.h && 
        playerRect.y + playerRect.h > bush1.y) {
        return true;
    }
    
    // Bush at (6,5) - corner
    Rect bush2(6 * TILE_SIZE + 50, 5 * TILE_SIZE + 50, 30, 20);
    if (playerRect.x < bush2.x + bush2.w && 
        playerRect.x + playerRect.w > bush2.x && 
        playerRect.y < bush2.y + bush2.h && 
        playerRect.y + playerRect.h > bush2.y) {
        return true;
    }
    
    // Leave the entire middle area (tiles 4,5 - 5,6 and 3,6 - 4,6) completely walkable
    
    // Check external collision callback (for NPCs and other dynamic objects)
    if (externalCollisionCheck_ && externalCollisionCheck_(newPosition)) {
        return true;
    }
    
    return false; // No collision detected
}

void Player::SetCollisionCallback(std::function<bool(const Vector2&)> callback) {
    externalCollisionCheck_ = callback;
}