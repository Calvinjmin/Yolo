#pragma once
#include "Renderer.h"
#include <vector>
#include <string>
#include <cmath>

enum class InteractableType {
    NONE,
    HOUSE,
    FARM, 
    FARM_FLOWERS,
    GARDEN,
    GARDEN_FLOWER,
    GARDEN_BUSH,
    WATER,
    NPC
};

class Interactable {
public:
    virtual ~Interactable() = default;
    
    virtual void Update(float deltaTime) = 0;
    virtual void Render(Renderer* renderer, Vector2 cameraOffset) = 0;
    virtual Vector2 GetPosition() const = 0;
    virtual Rect GetInteractionBounds() const = 0;
    virtual InteractableType GetType() const = 0;
    virtual std::vector<std::string> GetDialogue() const = 0;
    
    virtual bool IsPlayerInRange(Vector2 playerPosition) const {
        Vector2 objectPos = GetPosition();
        
        // Calculate distance between player and object center
        float dx = playerPosition.x - objectPos.x;
        float dy = playerPosition.y - objectPos.y;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        // Use a default interaction radius if not overridden
        float interactionRadius = GetInteractionRadius();
        
        return distance <= interactionRadius;
    }
    
    // Virtual method to get interaction radius - can be overridden by derived classes
    virtual float GetInteractionRadius() const {
        return 50.0f; // Default radius
    }
};