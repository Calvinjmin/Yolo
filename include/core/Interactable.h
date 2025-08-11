#pragma once
#include "Renderer.h"
#include <vector>
#include <string>

enum class InteractableType {
    NONE,
    HOUSE,
    FARM, 
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
        Rect bounds = GetInteractionBounds();
        
        return playerPosition.x >= bounds.x && 
               playerPosition.x <= bounds.x + bounds.w &&
               playerPosition.y >= bounds.y && 
               playerPosition.y <= bounds.y + bounds.h;
    }
};