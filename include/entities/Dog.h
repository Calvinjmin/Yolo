#pragma once
#include "InteractableObject.h"

class Dog : public InteractableObject {
public:
    Dog(float startX, float startY, float patrolWidth = 200.0f);
    ~Dog() = default;
    
    void Update(float deltaTime) override;
    void Render(Renderer* renderer, Vector2 cameraOffset) override;
    
    // Dog-specific methods
    void SetPatrolArea(float centerX, float centerY, float width);
    void SetSpeed(float speed) { speed_ = speed; }
    void UpdateWithPlayerPosition(float deltaTime, const Vector2& playerPosition);
    
protected:
    void RenderObject(Renderer* renderer, Vector2 cameraOffset) override;
    
private:
    void UpdateMovement(float deltaTime);
    void CheckBounds();
    bool CheckPlayerCollision(const Vector2& playerPosition);
    
    // Movement properties
    float speed_;
    float patrolCenterX_;
    float patrolWidth_;
    float minX_, maxX_;
    int direction_; // -1 for left, 1 for right
    
    // Visual properties
    bool facingRight_;
    float animationTimer_;
    
    // Dog dimensions
    const int DOG_WIDTH = 24;
    const int DOG_HEIGHT = 16;
};