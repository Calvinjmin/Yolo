#pragma once
#include "Interactable.h"
#include <memory>
#include <functional>

class InteractableObject : public Interactable {
public:
    InteractableObject(float x, float y, InteractableType type, 
                      const std::vector<std::string>& dialogue, 
                      bool isInteractable = true);
    virtual ~InteractableObject() = default;
    
    // Interactable interface
    void Update(float deltaTime) override;
    void Render(Renderer* renderer, Vector2 cameraOffset) override;
    Vector2 GetPosition() const override { return position_; }
    Rect GetInteractionBounds() const override;
    InteractableType GetType() const override { return type_; }
    std::vector<std::string> GetDialogue() const override { return dialogue_; }
    float GetInteractionRadius() const override { return interactionRadius_; }
    
    // Dynamic object features
    virtual void SetPosition(float x, float y);
    virtual void SetInteractable(bool interactable) { isInteractable_ = interactable; }
    virtual bool IsInteractable() const { return isInteractable_; }
    
    // Proximity detection
    float DistanceTo(const InteractableObject* other) const;
    float DistanceTo(const Vector2& position) const;
    bool IsNearObject(const InteractableObject* other, float threshold = 100.0f) const;
    
    // Callback system for proximity events
    void SetProximityCallback(std::function<void(const InteractableObject*)> callback) {
        proximityCallback_ = callback;
    }
    
    void CheckProximityTo(const InteractableObject* other, float threshold = 100.0f);
    
protected:
    Vector2 position_;
    InteractableType type_;
    std::vector<std::string> dialogue_;
    bool isInteractable_;
    float interactionRadius_;
    
    // Proximity system
    std::function<void(const InteractableObject*)> proximityCallback_;
    
    // Visual properties
    virtual void RenderObject(Renderer* renderer, Vector2 cameraOffset) {}
    
private:
    const int OBJECT_WIDTH = 32;
    const int OBJECT_HEIGHT = 32;
};