#pragma once
#include "InteractableObject.h"
#include <memory>
#include <vector>
#include <functional>

class DynamicObjectManager {
public:
    DynamicObjectManager() = default;
    ~DynamicObjectManager() = default;
    
    // Object management
    void AddObject(std::unique_ptr<InteractableObject> object);
    void RemoveObject(InteractableObject* object);
    void Clear();
    
    // Update and render all objects
    void UpdateAll(float deltaTime);
    void UpdateAll(float deltaTime, const Vector2& playerPosition);
    void RenderAll(Renderer* renderer, const Vector2& cameraOffset);
    
    // Proximity detection
    InteractableObject* GetNearestObject(const Vector2& position, float maxDistance = 100.0f);
    std::vector<InteractableObject*> GetObjectsInRange(const Vector2& position, float range);
    InteractableObject* GetInteractableNear(const Vector2& position, float range = 50.0f);
    
    // Object queries
    size_t GetObjectCount() const { return objects_.size(); }
    const std::vector<std::unique_ptr<InteractableObject>>& GetAllObjects() const { return objects_; }
    
    // Collision detection for player
    bool CheckCollisionWithAny(const Vector2& playerPosition) const;
    
private:
    std::vector<std::unique_ptr<InteractableObject>> objects_;
    
    // Helper methods
    bool IsValidObject(const InteractableObject* object) const;
};