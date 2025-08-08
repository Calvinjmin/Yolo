#include "Camera.h"
#include <cstdio>

Camera::Camera() 
    : position_(4608.0f, 3456.0f), target_(4608.0f, 3456.0f), offset_(0.0f, 0.0f),
      viewportWidth_(1024), viewportHeight_(768), 
      worldWidth_(9216), worldHeight_(6912), followSpeed_(5.0f) {
    // Initialize at world center to match player starting position
}

Camera::~Camera() {
}

void Camera::SetTarget(const Vector2& target) {
    target_ = target;
}

void Camera::Update(float deltaTime) {
    // Snap camera directly to target for immediate following
    position_ = target_;
    
    // Center camera on position
    offset_.x = position_.x - viewportWidth_ / 2.0f;
    offset_.y = position_.y - viewportHeight_ / 2.0f;
    
    
    ClampToWorldBounds();
}

Vector2 Camera::WorldToScreen(const Vector2& worldPos) const {
    return Vector2(worldPos.x - offset_.x, worldPos.y - offset_.y);
}

Vector2 Camera::ScreenToWorld(const Vector2& screenPos) const {
    return Vector2(screenPos.x + offset_.x, screenPos.y + offset_.y);
}

void Camera::SetViewportSize(int width, int height) {
    viewportWidth_ = width;
    viewportHeight_ = height;
}

void Camera::SetWorldSize(int width, int height) {
    worldWidth_ = width;
    worldHeight_ = height;
}

void Camera::ClampToWorldBounds() {
    // Only clamp offset if we're near world edges
    // Allow camera to show empty space when player is near edges
    
    // Only clamp when showing areas outside the world bounds
    if (offset_.x < 0) offset_.x = 0;
    if (offset_.y < 0) offset_.y = 0;
    if (offset_.x > worldWidth_ - viewportWidth_) {
        offset_.x = worldWidth_ - viewportWidth_;
    }
    if (offset_.y > worldHeight_ - viewportHeight_) {
        offset_.y = worldHeight_ - viewportHeight_;
    }
}