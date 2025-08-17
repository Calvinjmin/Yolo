#include "Dog.h"
#include "Renderer.h"
#include <cmath>

Dog::Dog(float startX, float startY, float patrolWidth)
    : InteractableObject(startX, startY, InteractableType::NPC, 
                        {"Woof! Woof!", "The dog seems friendly and energetic.", "It's enjoying its run around the area."}),
      speed_(80.0f), patrolCenterX_(startX), patrolWidth_(patrolWidth),
      direction_(1), facingRight_(true), animationTimer_(0.0f) {
    
    // Set patrol bounds
    minX_ = patrolCenterX_ - patrolWidth_ / 2.0f;
    maxX_ = patrolCenterX_ + patrolWidth_ / 2.0f;
    
    // Make sure dog stays within world bounds
    const int TILE_SIZE = 128;
    if (minX_ < TILE_SIZE) minX_ = TILE_SIZE;
    if (maxX_ > 9 * TILE_SIZE) maxX_ = 9 * TILE_SIZE;
    
    interactionRadius_ = 40.0f; // Slightly larger interaction area for the dog
}

void Dog::Update(float deltaTime) {
    InteractableObject::Update(deltaTime);
    UpdateMovement(deltaTime);
    
    // Update animation timer
    animationTimer_ += deltaTime;
    if (animationTimer_ > 1.0f) {
        animationTimer_ = 0.0f;
    }
}

void Dog::UpdateMovement(float deltaTime) {
    // Move the dog back and forth
    float movement = speed_ * direction_ * deltaTime;
    position_.x += movement;
    
    // Check bounds and reverse direction if needed
    CheckBounds();
    
    // Update facing direction
    facingRight_ = (direction_ == 1);
}

void Dog::CheckBounds() {
    if (position_.x <= minX_) {
        position_.x = minX_;
        direction_ = 1; // Go right
    } else if (position_.x >= maxX_) {
        position_.x = maxX_;
        direction_ = -1; // Go left
    }
}

void Dog::UpdateWithPlayerPosition(float deltaTime, const Vector2& playerPosition) {
    InteractableObject::Update(deltaTime);
    
    // Check for player collision before moving
    bool willCollide = false;
    float nextX = position_.x + (speed_ * direction_ * deltaTime);
    Vector2 nextPos = {nextX, position_.y};
    
    // Check if the next position would collide with player
    float distance = std::sqrt((nextPos.x - playerPosition.x) * (nextPos.x - playerPosition.x) + 
                              (nextPos.y - playerPosition.y) * (nextPos.y - playerPosition.y));
    
    if (distance < 35.0f) { // Player collision radius
        willCollide = true;
    }
    
    if (willCollide) {
        // Bounce off player - reverse direction
        direction_ = -direction_;
    }
    
    // Now update movement normally
    UpdateMovement(deltaTime);
    
    // Update animation timer
    animationTimer_ += deltaTime;
    if (animationTimer_ > 1.0f) {
        animationTimer_ = 0.0f;
    }
}

bool Dog::CheckPlayerCollision(const Vector2& playerPosition) {
    float distance = std::sqrt((position_.x - playerPosition.x) * (position_.x - playerPosition.x) + 
                              (position_.y - playerPosition.y) * (position_.y - playerPosition.y));
    return distance < 35.0f; // Collision threshold
}

void Dog::SetPatrolArea(float centerX, float centerY, float width) {
    patrolCenterX_ = centerX;
    patrolWidth_ = width;
    position_.y = centerY;
    
    minX_ = patrolCenterX_ - patrolWidth_ / 2.0f;
    maxX_ = patrolCenterX_ + patrolWidth_ / 2.0f;
}

void Dog::Render(Renderer* renderer, Vector2 cameraOffset) {
    RenderObject(renderer, cameraOffset);
}

void Dog::RenderObject(Renderer* renderer, Vector2 cameraOffset) {
    // Dog colors
    SDL_Color dogBrown = {139, 69, 19, 255};      // Main body
    SDL_Color dogLightBrown = {160, 82, 22, 255}; // Lighter areas
    SDL_Color dogDarkBrown = {101, 67, 33, 255};  // Darker areas/shadow
    SDL_Color dogBlack = {0, 0, 0, 255};          // Eyes, nose
    SDL_Color dogWhite = {255, 255, 255, 255};    // Highlights
    
    // Calculate screen position
    int screenX = static_cast<int>(position_.x - cameraOffset.x);
    int screenY = static_cast<int>(position_.y - cameraOffset.y);
    
    // Dog shadow
    Rect shadowRect(screenX + 2, screenY + 2, DOG_WIDTH, DOG_HEIGHT);
    renderer->DrawRect(shadowRect, SDL_Color{0, 0, 0, 60});
    
    // Main dog body
    Rect bodyRect(screenX, screenY, DOG_WIDTH, DOG_HEIGHT);
    renderer->DrawRect(bodyRect, dogBrown);
    
    // Dog head (front part)
    int headX = facingRight_ ? screenX + 16 : screenX;
    Rect headRect(headX, screenY, 8, 12);
    renderer->DrawRect(headRect, dogLightBrown);
    
    // Dog tail (back part, animated)
    int tailOffset = static_cast<int>(std::sin(animationTimer_ * 8.0f) * 2.0f); // Wagging tail
    int tailX = facingRight_ ? screenX - 2 : screenX + DOG_WIDTH - 2;
    Rect tailRect(tailX, screenY + 2 + tailOffset, 4, 6);
    renderer->DrawRect(tailRect, dogBrown);
    
    // Dog legs (simple animation)
    int legOffset = static_cast<int>(std::sin(animationTimer_ * 6.0f) * 1.0f);
    for (int i = 0; i < 4; i++) {
        int legX = screenX + 2 + i * 5;
        int legY = screenY + DOG_HEIGHT - 3 + (i % 2 == 0 ? legOffset : -legOffset);
        Rect legRect(legX, legY, 2, 3);
        renderer->DrawRect(legRect, dogDarkBrown);
    }
    
    // Dog eyes
    int eyeX = facingRight_ ? screenX + 18 : screenX + 2;
    Rect eyeRect(eyeX, screenY + 3, 2, 2);
    renderer->DrawRect(eyeRect, dogBlack);
    
    // Dog nose
    int noseX = facingRight_ ? screenX + 22 : screenX + 0;
    Rect noseRect(noseX, screenY + 6, 2, 1);
    renderer->DrawRect(noseRect, dogBlack);
    
    // Dog ear
    int earX = facingRight_ ? screenX + 15 : screenX + 5;
    Rect earRect(earX, screenY - 2, 4, 4);
    renderer->DrawRect(earRect, dogDarkBrown);
    
    // Highlight on body for 3D effect
    Rect highlightRect(screenX, screenY, DOG_WIDTH, 3);
    renderer->DrawRect(highlightRect, dogLightBrown);
}