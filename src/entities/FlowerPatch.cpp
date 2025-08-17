#include "FlowerPatch.h"
#include "Renderer.h"
#include <cmath>

FlowerPatch::FlowerPatch(float x, float y, const std::vector<std::string>& dialogue, 
                        const std::string& patchType)
    : InteractableObject(x, y, InteractableType::GARDEN_FLOWER, dialogue, true),
      patchType_(patchType), animationTimer_(0.0f) {
    
    // Set smaller interaction radius for flower patches
    interactionRadius_ = 25.0f;
}

void FlowerPatch::Update(float deltaTime) {
    InteractableObject::Update(deltaTime);
    
    // Update animation timer for gentle swaying effect
    animationTimer_ += deltaTime;
    if (animationTimer_ > 10.0f) {
        animationTimer_ = 0.0f;
    }
}

void FlowerPatch::Render(Renderer* renderer, Vector2 cameraOffset) {
    RenderObject(renderer, cameraOffset);
}

void FlowerPatch::RenderObject(Renderer* renderer, Vector2 cameraOffset) {
    // Calculate screen position
    int screenX = static_cast<int>(position_.x - cameraOffset.x);
    int screenY = static_cast<int>(position_.y - cameraOffset.y);
    
    // Render individual flowers
    int flowerCount = (patchType_ == "farm") ? 4 : 6;
    for (int i = 0; i < flowerCount; i++) {
        // Arrange flowers in a natural pattern
        int flowerX, flowerY;
        if (patchType_ == "farm") {
            flowerX = screenX + 5 + (i % 2) * 18;
            flowerY = screenY + 5 + (i / 2) * 18;
        } else {
            flowerX = screenX + 4 + (i % 3) * 12;
            flowerY = screenY + 4 + (i / 3) * 15;
        }
        
        // Add gentle swaying animation
        float sway = std::sin(animationTimer_ * 2.0f + i * 0.5f) * 1.0f;
        flowerX += static_cast<int>(sway);
        
        // Flower stem
        Rect stemRect(flowerX + 3, flowerY + 6, 2, 6);
        renderer->DrawRect(stemRect, SDL_Color{34, 139, 34, 255});
        
        // Flower head - different colors based on type and index
        SDL_Color flowerColor;
        if (patchType_ == "farm") {
            // Farm flowers: pink, yellow, coral
            if (i % 3 == 0) {
                flowerColor = {255, 182, 193, 255}; // Light pink
            } else if (i % 3 == 1) {
                flowerColor = {255, 255, 0, 255};   // Yellow
            } else {
                flowerColor = {255, 160, 122, 255}; // Light coral
            }
        } else {
            // Garden flowers: more variety
            if (i % 4 == 0) {
                flowerColor = {255, 182, 193, 255}; // Light pink
            } else if (i % 4 == 1) {
                flowerColor = {138, 43, 226, 255};  // Blue violet
            } else if (i % 4 == 2) {
                flowerColor = {255, 255, 0, 255};   // Yellow
            } else {
                flowerColor = {255, 69, 0, 255};    // Red orange
            }
        }
        
        Rect flowerHead(flowerX, flowerY, 6, 6);
        renderer->DrawRect(flowerHead, flowerColor);
        
        // Flower highlight
        Rect highlight(flowerX + 1, flowerY + 1, 2, 2);
        renderer->DrawRect(highlight, SDL_Color{255, 255, 255, 180});
        
        // Flower center
        Rect center(flowerX + 2, flowerY + 2, 2, 2);
        renderer->DrawRect(center, SDL_Color{255, 165, 0, 255});
        
        // Small leaves
        if (i % 2 == 0) {
            Rect leaf1(flowerX + 1, flowerY + 5, 3, 1);
            Rect leaf2(flowerX + 4, flowerY + 5, 2, 1);
            renderer->DrawRect(leaf1, SDL_Color{50, 160, 50, 255});
            renderer->DrawRect(leaf2, SDL_Color{40, 150, 40, 255});
        }
    }
}