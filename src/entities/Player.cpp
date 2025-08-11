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
    
    // Farm area collision (tiles 6,2 to 8,4)
    Rect farmRect(6 * TILE_SIZE, 2 * TILE_SIZE, 3 * TILE_SIZE, 3 * TILE_SIZE);
    if (playerRect.x < farmRect.x + farmRect.w && 
        playerRect.x + playerRect.w > farmRect.x && 
        playerRect.y < farmRect.y + farmRect.h && 
        playerRect.y + playerRect.h > farmRect.y) {
        return true;
    }
    
    // Garden flower patches (specific collision spots)
    // Flower patch at (4,5)
    Rect flowerPatch1(4 * TILE_SIZE + 20, 5 * TILE_SIZE + 20, 80, 80);
    if (playerRect.x < flowerPatch1.x + flowerPatch1.w && 
        playerRect.x + playerRect.w > flowerPatch1.x && 
        playerRect.y < flowerPatch1.y + flowerPatch1.h && 
        playerRect.y + playerRect.h > flowerPatch1.y) {
        return true;
    }
    
    // Flower patch at (6,6)
    Rect flowerPatch2(6 * TILE_SIZE + 20, 6 * TILE_SIZE + 20, 80, 80);
    if (playerRect.x < flowerPatch2.x + flowerPatch2.w && 
        playerRect.x + playerRect.w > flowerPatch2.x && 
        playerRect.y < flowerPatch2.y + flowerPatch2.h && 
        playerRect.y + playerRect.h > flowerPatch2.y) {
        return true;
    }
    
    // Bushes in garden area (tiles where (x+y) % 3 == 1)
    for (int y = 5; y < 7; y++) {
        for (int x = 3; x < 7; x++) {
            if ((x + y) % 3 == 1) {
                Rect bushRect(x * TILE_SIZE + 40, y * TILE_SIZE + 40, 40, 30);
                if (playerRect.x < bushRect.x + bushRect.w && 
                    playerRect.x + playerRect.w > bushRect.x && 
                    playerRect.y < bushRect.y + bushRect.h && 
                    playerRect.y + playerRect.h > bushRect.y) {
                    return true;
                }
            }
        }
    }
    
    // Check external collision callback (for NPCs and other dynamic objects)
    if (externalCollisionCheck_ && externalCollisionCheck_(newPosition)) {
        return true;
    }
    
    return false; // No collision detected
}

void Player::SetCollisionCallback(std::function<bool(const Vector2&)> callback) {
    externalCollisionCheck_ = callback;
}