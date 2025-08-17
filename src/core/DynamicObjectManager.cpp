#include "DynamicObjectManager.h"
#include "Renderer.h"
#include "Dog.h"
#include <algorithm>

void DynamicObjectManager::AddObject(std::unique_ptr<InteractableObject> object) {
    if (object) {
        objects_.push_back(std::move(object));
    }
}

void DynamicObjectManager::RemoveObject(InteractableObject* object) {
    objects_.erase(
        std::remove_if(objects_.begin(), objects_.end(),
            [object](const std::unique_ptr<InteractableObject>& obj) {
                return obj.get() == object;
            }),
        objects_.end()
    );
}

void DynamicObjectManager::Clear() {
    objects_.clear();
}

void DynamicObjectManager::UpdateAll(float deltaTime) {
    for (auto& object : objects_) {
        if (object) {
            object->Update(deltaTime);
        }
    }
    
    // Check proximity between all objects (for future proximity events)
    for (size_t i = 0; i < objects_.size(); ++i) {
        for (size_t j = i + 1; j < objects_.size(); ++j) {
            if (objects_[i] && objects_[j]) {
                objects_[i]->CheckProximityTo(objects_[j].get(), 80.0f);
                objects_[j]->CheckProximityTo(objects_[i].get(), 80.0f);
            }
        }
    }
}

void DynamicObjectManager::UpdateAll(float deltaTime, const Vector2& playerPosition) {
    for (auto& object : objects_) {
        if (object) {
            // Try to cast to Dog and update with player position if it's a dog
            if (auto* dog = dynamic_cast<Dog*>(object.get())) {
                dog->UpdateWithPlayerPosition(deltaTime, playerPosition);
            } else {
                object->Update(deltaTime);
            }
        }
    }
    
    // Check proximity between all objects (for future proximity events)
    for (size_t i = 0; i < objects_.size(); ++i) {
        for (size_t j = i + 1; j < objects_.size(); ++j) {
            if (objects_[i] && objects_[j]) {
                objects_[i]->CheckProximityTo(objects_[j].get(), 80.0f);
                objects_[j]->CheckProximityTo(objects_[i].get(), 80.0f);
            }
        }
    }
}

void DynamicObjectManager::RenderAll(Renderer* renderer, const Vector2& cameraOffset) {
    for (auto& object : objects_) {
        if (object) {
            object->Render(renderer, cameraOffset);
        }
    }
}

InteractableObject* DynamicObjectManager::GetNearestObject(const Vector2& position, float maxDistance) {
    InteractableObject* nearest = nullptr;
    float nearestDistance = maxDistance;
    
    for (auto& object : objects_) {
        if (object && IsValidObject(object.get())) {
            float distance = object->DistanceTo(position);
            if (distance < nearestDistance) {
                nearestDistance = distance;
                nearest = object.get();
            }
        }
    }
    
    return nearest;
}

std::vector<InteractableObject*> DynamicObjectManager::GetObjectsInRange(const Vector2& position, float range) {
    std::vector<InteractableObject*> nearbyObjects;
    
    for (auto& object : objects_) {
        if (object && IsValidObject(object.get())) {
            if (object->DistanceTo(position) <= range) {
                nearbyObjects.push_back(object.get());
            }
        }
    }
    
    return nearbyObjects;
}

InteractableObject* DynamicObjectManager::GetInteractableNear(const Vector2& position, float range) {
    for (auto& object : objects_) {
        if (object && object->IsInteractable() && IsValidObject(object.get())) {
            if (object->IsPlayerInRange(position)) {
                return object.get();
            }
        }
    }
    
    return nullptr;
}

bool DynamicObjectManager::CheckCollisionWithAny(const Vector2& playerPosition) const {
    const int PLAYER_WIDTH = 32;
    const int PLAYER_HEIGHT = 32;
    
    Rect playerRect(
        static_cast<int>(playerPosition.x),
        static_cast<int>(playerPosition.y),
        PLAYER_WIDTH,
        PLAYER_HEIGHT
    );
    
    for (const auto& object : objects_) {
        if (object && IsValidObject(object.get())) {
            Vector2 objPos = object->GetPosition();
            Rect objRect(
                static_cast<int>(objPos.x),
                static_cast<int>(objPos.y),
                24, 16  // Assuming most objects are around this size
            );
            
            if (playerRect.x < objRect.x + objRect.w && 
                playerRect.x + playerRect.w > objRect.x && 
                playerRect.y < objRect.y + objRect.h && 
                playerRect.y + playerRect.h > objRect.y) {
                return true;
            }
        }
    }
    
    return false;
}

bool DynamicObjectManager::IsValidObject(const InteractableObject* object) const {
    return object != nullptr;
}