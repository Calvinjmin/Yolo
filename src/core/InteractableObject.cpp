#include "InteractableObject.h"
#include <cmath>

InteractableObject::InteractableObject(float x, float y, InteractableType type, 
                                      const std::vector<std::string>& dialogue, 
                                      bool isInteractable)
    : position_(x, y), type_(type), dialogue_(dialogue), 
      isInteractable_(isInteractable), interactionRadius_(50.0f),
      proximityCallback_(nullptr) {
}

void InteractableObject::Update(float deltaTime) {
    // Base update - can be overridden by derived classes
}

void InteractableObject::Render(Renderer* renderer, Vector2 cameraOffset) {
    RenderObject(renderer, cameraOffset);
}

Rect InteractableObject::GetInteractionBounds() const {
    return Rect(
        static_cast<int>(position_.x - interactionRadius_),
        static_cast<int>(position_.y - interactionRadius_),
        static_cast<int>(OBJECT_WIDTH + 2 * interactionRadius_),
        static_cast<int>(OBJECT_HEIGHT + 2 * interactionRadius_)
    );
}

void InteractableObject::SetPosition(float x, float y) {
    position_.x = x;
    position_.y = y;
}

float InteractableObject::DistanceTo(const InteractableObject* other) const {
    if (!other) return INFINITY;
    return DistanceTo(other->GetPosition());
}

float InteractableObject::DistanceTo(const Vector2& position) const {
    float dx = position_.x - position.x;
    float dy = position_.y - position.y;
    return std::sqrt(dx * dx + dy * dy);
}

bool InteractableObject::IsNearObject(const InteractableObject* other, float threshold) const {
    return DistanceTo(other) <= threshold;
}

void InteractableObject::CheckProximityTo(const InteractableObject* other, float threshold) {
    if (other && IsNearObject(other, threshold) && proximityCallback_) {
        proximityCallback_(other);
    }
}