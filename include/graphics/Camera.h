#pragma once
#include "Renderer.h"

class Camera {
public:
    Camera();
    ~Camera();
    
    void SetTarget(const Vector2& target);
    void Update(float deltaTime);
    
    Vector2 GetOffset() const { return offset_; }
    Vector2 WorldToScreen(const Vector2& worldPos) const;
    Vector2 ScreenToWorld(const Vector2& screenPos) const;
    
    void SetViewportSize(int width, int height);
    void SetWorldSize(int width, int height);
    
    int GetViewportWidth() const { return viewportWidth_; }
    int GetViewportHeight() const { return viewportHeight_; }
    int GetWorldWidth() const { return worldWidth_; }
    int GetWorldHeight() const { return worldHeight_; }
    
private:
    Vector2 position_;
    Vector2 target_;
    Vector2 offset_;
    
    int viewportWidth_;
    int viewportHeight_;
    int worldWidth_;
    int worldHeight_;
    
    float followSpeed_;
    
    void ClampToWorldBounds();
};