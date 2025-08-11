#include "DialogueSystem.h"
#include <algorithm>
#include <iostream>

DialogueSystem::DialogueSystem() 
    : isActive_(false), nearInteractable_(false), currentText_(""), 
      currentType_(InteractableType::NONE), nearbyType_(InteractableType::NONE),
      displayTimer_(0.0f), fadeAlpha_(0.0f) {
}

DialogueSystem::~DialogueSystem() {
}

void DialogueSystem::Initialize() {
    SetupInteractionZones();
}

void DialogueSystem::SetupInteractionZones() {
    const int TILE_SIZE = 128;
    
    // House interaction zone (tiles 2,2 to 3,3 = 256,256 to 512,512)
    Rect houseZone(2 * TILE_SIZE, 2 * TILE_SIZE, 2 * TILE_SIZE, 2 * TILE_SIZE);
    std::vector<std::string> houseDialogues = {
        "A cozy cottage with a red tile roof.",
        "Windows reflect warm sunlight beautifully.",
        "This looks like a peaceful place to live."
    };
    interactionZones_.push_back(InteractionZone(houseZone, InteractableType::HOUSE, houseDialogues));
    
    // Farm interaction zone (tiles 6,2 to 8,4 = 768,256 to 1152,640)
    Rect farmZone(6 * TILE_SIZE, 2 * TILE_SIZE, 3 * TILE_SIZE, 3 * TILE_SIZE);
    std::vector<std::string> farmDialogues = {
        "Rich soil perfect for growing crops.",
        "The seedlings are sprouting nicely!",
        "This farm bed looks well-maintained."
    };
    interactionZones_.push_back(InteractionZone(farmZone, InteractableType::FARM, farmDialogues));
    
    // Garden area (tiles 3,5 to 6,6 = 384,640 to 896,896)
    Rect gardenZone(3 * TILE_SIZE, 5 * TILE_SIZE, 4 * TILE_SIZE, 2 * TILE_SIZE);
    std::vector<std::string> gardenDialogues = {
        "Beautiful flowers bloom here in vibrant colors.",
        "The sweet fragrance fills the air.",
        "These flowers attract butterflies and bees."
    };
    interactionZones_.push_back(InteractionZone(gardenZone, InteractableType::GARDEN_FLOWER, gardenDialogues));
}

void DialogueSystem::Update(float deltaTime) {
    if (isActive_) {
        displayTimer_ += deltaTime;
        
        // Fade in effect
        if (fadeAlpha_ < 255.0f) {
            fadeAlpha_ = std::min(255.0f, fadeAlpha_ + 400.0f * deltaTime);
        }
    } else {
        // Fade out effect
        if (fadeAlpha_ > 0.0f) {
            fadeAlpha_ = std::max(0.0f, fadeAlpha_ - 600.0f * deltaTime);
        }
    }
}

InteractableType DialogueSystem::CheckNearbyInteraction(const Vector2& playerPosition) const {
    // Player collision rectangle
    Rect playerRect(
        static_cast<int>(playerPosition.x),
        static_cast<int>(playerPosition.y),
        32, // PLAYER_WIDTH
        32  // PLAYER_HEIGHT
    );
    
    for (const auto& zone : interactionZones_) {
        // Check if player is near the interaction zone (large area for easy interaction)
        int expandedMargin = 80;
        if (playerRect.x < zone.bounds.x + zone.bounds.w + expandedMargin && 
            playerRect.x + playerRect.w > zone.bounds.x - expandedMargin && 
            playerRect.y < zone.bounds.y + zone.bounds.h + expandedMargin && 
            playerRect.y + playerRect.h > zone.bounds.y - expandedMargin) {
            
            return zone.type;
        }
    }
    
    return InteractableType::NONE;
}

bool DialogueSystem::CheckInteraction(const Vector2& playerPosition, const Vector2& cameraOffset) {
    InteractableType nearbyType = CheckNearbyInteraction(playerPosition);
    
    if (nearbyType != InteractableType::NONE) {
        ShowDialogue(nearbyType);
        return true;
    }
    
    return false;
}

void DialogueSystem::ShowDialogue(InteractableType type) {
    currentType_ = type;
    auto dialogues = GetDialogueForType(type);
    
    if (!dialogues.empty()) {
        // Find the zone to get current dialogue index
        for (auto& zone : interactionZones_) {
            if (zone.type == type) {
                currentText_ = dialogues[zone.currentDialogue];
                break;
            }
        }
    }
    isActive_ = true;
    displayTimer_ = 0.0f;
    fadeAlpha_ = 255.0f; // Start fully visible
}

void DialogueSystem::NextDialogue() {
    for (auto& zone : interactionZones_) {
        if (zone.type == currentType_) {
            zone.currentDialogue = (zone.currentDialogue + 1) % zone.dialogues.size();
            currentText_ = zone.dialogues[zone.currentDialogue];
            displayTimer_ = 0.0f;
            break;
        }
    }
}

void DialogueSystem::HideDialogue() {
    isActive_ = false;
}

void DialogueSystem::SetNearInteractable(bool near, InteractableType type) {
    nearInteractable_ = near;
    nearbyType_ = type;
}

std::vector<std::string> DialogueSystem::GetDialogueForType(InteractableType type) {
    for (const auto& zone : interactionZones_) {
        if (zone.type == type) {
            return zone.dialogues;
        }
    }
    return {};
}

void DialogueSystem::Render(Renderer* renderer, int windowWidth, int windowHeight) {
    if (isActive_) {
        RenderDialogueBox(renderer, windowWidth, windowHeight);
    } else if (nearInteractable_) {
        // Show interaction prompt when near an object but not actively in dialogue
        RenderInteractionPrompt(renderer, windowWidth, windowHeight);
    }
}

void DialogueSystem::RenderInteractionPrompt(Renderer* renderer, int windowWidth, int windowHeight) {
    // Wider prompt at the top of the screen
    int promptWidth = 280;
    int promptHeight = 45;
    int promptX = (windowWidth - promptWidth) / 2;
    int promptY = 50;
    
    // Semi-transparent background
    Rect promptBox(promptX, promptY, promptWidth, promptHeight);
    SDL_Color bgColor = {0, 0, 0, 180};
    renderer->DrawRect(promptBox, bgColor);
    
    // Border
    SDL_Color borderColor = {255, 255, 255, 220};
    int borderWidth = 2;
    Rect borderTop(promptX, promptY, promptWidth, borderWidth);
    Rect borderBottom(promptX, promptY + promptHeight - borderWidth, promptWidth, borderWidth);
    Rect borderLeft(promptX, promptY, borderWidth, promptHeight);
    Rect borderRight(promptX + promptWidth - borderWidth, promptY, borderWidth, promptHeight);
    renderer->DrawRect(borderTop, borderColor);
    renderer->DrawRect(borderBottom, borderColor);
    renderer->DrawRect(borderLeft, borderColor);
    renderer->DrawRect(borderRight, borderColor);
    
    // Text - centered in wider box
    RenderText(renderer, "Press SPACE to interact", promptX + 30, promptY + 18);
}

void DialogueSystem::RenderDialogueBox(Renderer* renderer, int windowWidth, int windowHeight) {
    int boxHeight = 100;
    int boxY = windowHeight - boxHeight - 30;
    int boxX = 30;
    int boxWidth = windowWidth - 60;
    
    // Elegant gradient background
    SDL_Color bgColor = {25, 25, 35, 240}; // Dark blue-grey with transparency
    Rect dialogueBox(boxX, boxY, boxWidth, boxHeight);
    renderer->DrawRect(dialogueBox, bgColor);
    
    // Subtle inner shadow effect
    SDL_Color shadowColor = {15, 15, 20, 180};
    Rect innerShadow(boxX + 2, boxY + 2, boxWidth - 4, boxHeight - 4);
    renderer->DrawRect(innerShadow, shadowColor);
    
    // Main content area
    SDL_Color contentBg = {30, 30, 40, 220};
    Rect contentArea(boxX + 4, boxY + 4, boxWidth - 8, boxHeight - 8);
    renderer->DrawRect(contentArea, contentBg);
    
    // Elegant border with rounded corners effect
    SDL_Color borderColor = {180, 180, 200, 255}; // Light grey border
    int borderWidth = 1;
    
    // Outer border
    Rect borderTop(boxX, boxY, boxWidth, borderWidth);
    Rect borderBottom(boxX, boxY + boxHeight - borderWidth, boxWidth, borderWidth);
    Rect borderLeft(boxX, boxY, borderWidth, boxHeight);
    Rect borderRight(boxX + boxWidth - borderWidth, boxY, borderWidth, boxHeight);
    renderer->DrawRect(borderTop, borderColor);
    renderer->DrawRect(borderBottom, borderColor);
    renderer->DrawRect(borderLeft, borderColor);
    renderer->DrawRect(borderRight, borderColor);
    
    // Corner highlights for rounded effect
    SDL_Color highlightColor = {220, 220, 240, 180};
    Rect cornerTL(boxX + 1, boxY + 1, 3, 3);
    Rect cornerTR(boxX + boxWidth - 4, boxY + 1, 3, 3);
    renderer->DrawRect(cornerTL, highlightColor);
    renderer->DrawRect(cornerTR, highlightColor);
    
    // Render main dialogue text
    if (!currentText_.empty()) {
        RenderText(renderer, currentText_, boxX + 20, boxY + 20);
    }
    
    // Control hints at bottom right
    SDL_Color hintColor = {160, 160, 180, 200};
    int hintY = boxY + boxHeight - 25;
    RenderHintText(renderer, "E: More info", boxX + 20, hintY, hintColor);
    RenderHintText(renderer, "Q: Exit", boxX + 120, hintY, hintColor);
}

void DialogueSystem::RenderText(Renderer* renderer, const std::string& text, int x, int y) {
    // Simple bitmap-style text rendering with basic character patterns
    SDL_Color textColor = {255, 255, 255, 255}; // White text
    
    int charWidth = 6;
    int charHeight = 8;
    int spacing = 2;
    
    for (size_t i = 0; i < text.length(); ++i) {
        char c = text[i];
        int charX = x + static_cast<int>(i) * (charWidth + spacing);
        
        if (c == ' ') {
            continue; // Skip spaces
        }
        
        // Simple bitmap patterns for common characters
        RenderCharacter(renderer, c, charX, y, charWidth, charHeight, textColor);
    }
}

void DialogueSystem::RenderHintText(Renderer* renderer, const std::string& text, int x, int y, SDL_Color color) {
    // Smaller text for hints
    int charWidth = 5;
    int charHeight = 6;
    int spacing = 1;
    
    for (size_t i = 0; i < text.length(); ++i) {
        char c = text[i];
        int charX = x + static_cast<int>(i) * (charWidth + spacing);
        
        if (c == ' ') {
            continue; // Skip spaces
        }
        
        // Render smaller characters for hints
        RenderCharacter(renderer, c, charX, y, charWidth, charHeight, color);
    }
}

void DialogueSystem::RenderCharacter(Renderer* renderer, char c, int x, int y, int w, int h, SDL_Color color) {
    // Simple 6x8 bitmap font patterns
    std::vector<std::vector<int>> pattern;
    
    switch (c) {
        case 'A': pattern = {{0,1,1,1,0,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,1,1,1,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'B': pattern = {{1,1,1,1,0,0},{1,0,0,0,1,0},{1,1,1,1,0,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,1,1,1,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'C': pattern = {{0,1,1,1,1,0},{1,0,0,0,0,0},{1,0,0,0,0,0},{1,0,0,0,0,0},{1,0,0,0,0,0},{0,1,1,1,1,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'D': pattern = {{1,1,1,1,0,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,1,1,1,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'E': pattern = {{1,1,1,1,1,0},{1,0,0,0,0,0},{1,0,0,0,0,0},{1,1,1,1,0,0},{1,0,0,0,0,0},{1,1,1,1,1,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'F': pattern = {{1,1,1,1,1,0},{1,0,0,0,0,0},{1,1,1,1,0,0},{1,0,0,0,0,0},{1,0,0,0,0,0},{1,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'G': pattern = {{0,1,1,1,1,0},{1,0,0,0,0,0},{1,0,0,0,0,0},{1,0,1,1,1,0},{1,0,0,0,1,0},{0,1,1,1,1,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'H': pattern = {{1,0,0,0,1,0},{1,0,0,0,1,0},{1,1,1,1,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'I': pattern = {{1,1,1,1,1,0},{0,0,1,0,0,0},{0,0,1,0,0,0},{0,0,1,0,0,0},{0,0,1,0,0,0},{1,1,1,1,1,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'L': pattern = {{1,0,0,0,0,0},{1,0,0,0,0,0},{1,0,0,0,0,0},{1,0,0,0,0,0},{1,0,0,0,0,0},{1,1,1,1,1,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'M': pattern = {{1,0,0,0,1,0},{1,1,0,1,1,0},{1,0,1,0,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'N': pattern = {{1,0,0,0,1,0},{1,1,0,0,1,0},{1,0,1,0,1,0},{1,0,0,1,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'O': pattern = {{0,1,1,1,0,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{0,1,1,1,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'P': pattern = {{1,1,1,1,0,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,1,1,1,0,0},{1,0,0,0,0,0},{1,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'Q': pattern = {{0,1,1,1,0,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,0,1,0,1,0},{1,0,0,1,1,0},{0,1,1,1,1,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'R': pattern = {{1,1,1,1,0,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,1,1,1,0,0},{1,0,0,1,0,0},{1,0,0,0,1,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'S': pattern = {{0,1,1,1,1,0},{1,0,0,0,0,0},{1,0,0,0,0,0},{0,1,1,1,0,0},{0,0,0,0,1,0},{1,1,1,1,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'T': pattern = {{1,1,1,1,1,0},{0,0,1,0,0,0},{0,0,1,0,0,0},{0,0,1,0,0,0},{0,0,1,0,0,0},{0,0,1,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'U': pattern = {{1,0,0,0,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{0,1,1,1,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'V': pattern = {{1,0,0,0,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{0,1,0,1,0,0},{0,1,0,1,0,0},{0,0,1,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'W': pattern = {{1,0,0,0,1,0},{1,0,0,0,1,0},{1,0,1,0,1,0},{1,0,1,0,1,0},{1,1,0,1,1,0},{1,0,0,0,1,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'Y': pattern = {{1,0,0,0,1,0},{1,0,0,0,1,0},{0,1,0,1,0,0},{0,0,1,0,0,0},{0,0,1,0,0,0},{0,0,1,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        
        // Lowercase letters
        case 'a': pattern = {{0,0,0,0,0,0},{0,0,0,0,0,0},{0,1,1,1,0,0},{0,0,0,0,1,0},{0,1,1,1,1,0},{1,0,0,0,1,0},{0,1,1,1,1,0},{0,0,0,0,0,0}}; break;
        case 'b': pattern = {{1,0,0,0,0,0},{1,0,0,0,0,0},{1,1,1,1,0,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,1,1,1,0,0},{0,0,0,0,0,0}}; break;
        case 'c': pattern = {{0,0,0,0,0,0},{0,0,0,0,0,0},{0,1,1,1,1,0},{1,0,0,0,0,0},{1,0,0,0,0,0},{1,0,0,0,0,0},{0,1,1,1,1,0},{0,0,0,0,0,0}}; break;
        case 'd': pattern = {{0,0,0,0,1,0},{0,0,0,0,1,0},{0,1,1,1,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{0,1,1,1,1,0},{0,0,0,0,0,0}}; break;
        case 'e': pattern = {{0,0,0,0,0,0},{0,0,0,0,0,0},{0,1,1,1,0,0},{1,0,0,0,1,0},{1,1,1,1,0,0},{1,0,0,0,0,0},{0,1,1,1,1,0},{0,0,0,0,0,0}}; break;
        case 'f': pattern = {{0,0,1,1,1,0},{0,1,0,0,0,0},{0,1,0,0,0,0},{1,1,1,1,0,0},{0,1,0,0,0,0},{0,1,0,0,0,0},{0,1,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'g': pattern = {{0,0,0,0,0,0},{0,0,0,0,0,0},{0,1,1,1,1,0},{1,0,0,0,1,0},{0,1,1,1,1,0},{0,0,0,0,1,0},{1,1,1,1,0,0},{0,0,0,0,0,0}}; break;
        case 'h': pattern = {{1,0,0,0,0,0},{1,0,0,0,0,0},{1,1,1,1,0,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{0,0,0,0,0,0}}; break;
        case 'i': pattern = {{0,0,1,0,0,0},{0,0,0,0,0,0},{0,1,1,0,0,0},{0,0,1,0,0,0},{0,0,1,0,0,0},{0,0,1,0,0,0},{0,1,1,1,0,0},{0,0,0,0,0,0}}; break;
        case 'k': pattern = {{1,0,0,0,0,0},{1,0,0,1,0,0},{1,0,1,0,0,0},{1,1,0,0,0,0},{1,0,1,0,0,0},{1,0,0,1,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'l': pattern = {{1,1,0,0,0,0},{0,1,0,0,0,0},{0,1,0,0,0,0},{0,1,0,0,0,0},{0,1,0,0,0,0},{1,1,1,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'm': pattern = {{0,0,0,0,0,0},{0,0,0,0,0,0},{1,1,0,1,1,0},{1,0,1,0,1,0},{1,0,1,0,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{0,0,0,0,0,0}}; break;
        case 'n': pattern = {{0,0,0,0,0,0},{0,0,0,0,0,0},{1,1,1,1,0,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{0,0,0,0,0,0}}; break;
        case 'o': pattern = {{0,0,0,0,0,0},{0,0,0,0,0,0},{0,1,1,1,0,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{0,1,1,1,0,0},{0,0,0,0,0,0}}; break;
        case 'p': pattern = {{0,0,0,0,0,0},{0,0,0,0,0,0},{1,1,1,1,0,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,1,1,1,0,0},{1,0,0,0,0,0},{1,0,0,0,0,0}}; break;
        case 'r': pattern = {{0,0,0,0,0,0},{0,0,0,0,0,0},{1,0,1,1,0,0},{1,1,0,0,0,0},{1,0,0,0,0,0},{1,0,0,0,0,0},{1,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 's': pattern = {{0,0,0,0,0,0},{0,0,0,0,0,0},{0,1,1,1,1,0},{1,0,0,0,0,0},{0,1,1,1,0,0},{0,0,0,0,1,0},{1,1,1,1,0,0},{0,0,0,0,0,0}}; break;
        case 't': pattern = {{0,0,1,0,0,0},{0,0,1,0,0,0},{1,1,1,1,0,0},{0,0,1,0,0,0},{0,0,1,0,0,0},{0,0,1,0,0,0},{0,0,1,1,0,0},{0,0,0,0,0,0}}; break;
        case 'u': pattern = {{0,0,0,0,0,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,0,0,1,1,0},{0,1,1,0,1,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'v': pattern = {{0,0,0,0,0,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{0,1,0,1,0,0},{0,1,0,1,0,0},{0,0,1,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'w': pattern = {{0,0,0,0,0,0},{1,0,0,0,1,0},{1,0,1,0,1,0},{1,0,1,0,1,0},{1,1,0,1,1,0},{1,0,0,0,1,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case 'y': pattern = {{0,0,0,0,0,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{0,1,1,1,1,0},{0,0,0,0,1,0},{1,1,1,1,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        
        // Numbers and punctuation
        case '.': pattern = {{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,1,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case ':': pattern = {{0,0,0,0,0,0},{0,1,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,1,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case '!': pattern = {{0,1,0,0,0,0},{0,1,0,0,0,0},{0,1,0,0,0,0},{0,1,0,0,0,0},{0,0,0,0,0,0},{0,1,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        case '?': pattern = {{0,1,1,1,0,0},{1,0,0,0,1,0},{0,0,0,1,0,0},{0,0,1,0,0,0},{0,0,0,0,0,0},{0,0,1,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
        
        // Default for unknown characters
        default: pattern = {{1,1,1,1,1,0},{1,0,0,0,1,0},{1,0,1,0,1,0},{1,0,0,0,1,0},{1,0,0,0,1,0},{1,1,1,1,1,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}; break;
    }
    
    // Render the pattern
    for (int row = 0; row < 8 && row < h; ++row) {
        for (int col = 0; col < 6 && col < w; ++col) {
            if (row < pattern.size() && col < pattern[row].size() && pattern[row][col] == 1) {
                Rect pixel(x + col, y + row, 1, 1);
                renderer->DrawRect(pixel, color);
            }
        }
    }
}