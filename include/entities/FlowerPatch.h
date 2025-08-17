#pragma once
#include "InteractableObject.h"

class FlowerPatch : public InteractableObject {
public:
    FlowerPatch(float x, float y, const std::vector<std::string>& dialogue, 
                const std::string& patchType = "mixed");
    ~FlowerPatch() = default;
    
    void Update(float deltaTime) override;
    void Render(Renderer* renderer, Vector2 cameraOffset) override;
    
    void SetPatchType(const std::string& type) { patchType_ = type; }
    
protected:
    void RenderObject(Renderer* renderer, Vector2 cameraOffset) override;
    
private:
    std::string patchType_; // "mixed", "farm", "garden"
    float animationTimer_;
    
    // Flower patch dimensions
    const int PATCH_WIDTH = 35;
    const int PATCH_HEIGHT = 35;
};