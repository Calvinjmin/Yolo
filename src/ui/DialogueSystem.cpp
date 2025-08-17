#include "DialogueSystem.h"
#include "NPC.h"
#include "InteractableObject.h"
#include <algorithm>
#include <iostream>

DialogueSystem::DialogueSystem() 
    : isActive_(false), nearInteractable_(false), currentText_(""), 
      currentType_(InteractableType::NONE), nearbyType_(InteractableType::NONE),
      currentInteractable_(nullptr), displayTimer_(0.0f), fadeAlpha_(0.0f) {
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
    
    // Farm flower patch 1 (top-left corner: tile 6,2)
    Rect farmFlower1Zone(6 * TILE_SIZE, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE);
    std::vector<std::string> farmFlower1Dialogues = {
        "These lovely flowers brighten up the farm area.",
        "Pink, yellow, and coral blooms dance in the breeze.",
        "The flowers seem well-tended and healthy."
    };
    interactionZones_.push_back(InteractionZone(farmFlower1Zone, InteractableType::FARM_FLOWERS, farmFlower1Dialogues));
    
    // Farm flower patch 2 (bottom-right corner: tile 8,4)
    Rect farmFlower2Zone(8 * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE);
    std::vector<std::string> farmFlower2Dialogues = {
        "A colorful patch of flowers adds beauty to this corner.",
        "The farmer must have a soft spot for flowers.",
        "These blooms provide a nice contrast to the crops."
    };
    interactionZones_.push_back(InteractionZone(farmFlower2Zone, InteractableType::FARM_FLOWERS, farmFlower2Dialogues));
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
    
    // First check dynamic interactables (they get priority)
    for (const auto& interactable : dynamicInteractables_) {
        if (interactable && interactable->IsPlayerInRange(playerPosition)) {
            return interactable->GetType();
        }
    }
    
    // Then check static zones
    for (const auto& zone : interactionZones_) {
        // Much more accurate interaction margins based on zone type
        int expandedMargin;
        if (zone.type == InteractableType::FARM_FLOWERS) {
            expandedMargin = 25; // Very close for flower patches
        } else if (zone.type == InteractableType::GARDEN_FLOWER) {
            expandedMargin = 30; // Close for garden flowers
        } else if (zone.type == InteractableType::FARM) {
            expandedMargin = 35; // Close for farm areas
        } else if (zone.type == InteractableType::HOUSE) {
            expandedMargin = 40; // Slightly larger for house
        } else {
            expandedMargin = 35; // Reasonable default for other zones
        }
        
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
    currentInteractable_ = nullptr;
    
    if (type == InteractableType::NPC) {
        // Handle dynamic NPCs - this should not be used anymore
        // Use ShowDialogue(Interactable*) instead
        for (const auto& interactable : dynamicInteractables_) {
            if (interactable && interactable->GetType() == type) {
                if (auto npc = dynamic_cast<NPC*>(interactable)) {
                    currentText_ = npc->GetCurrentDialogue();
                    currentInteractable_ = interactable;
                    break;
                }
            }
        }
    } else {
        // Handle static zones
        auto dialogues = GetDialogueForType(type);
        if (!dialogues.empty()) {
            for (auto& zone : interactionZones_) {
                if (zone.type == type) {
                    currentText_ = dialogues[zone.currentDialogue];
                    break;
                }
            }
        }
    }
    
    isActive_ = true;
    displayTimer_ = 0.0f;
    fadeAlpha_ = 255.0f; // Start fully visible
}

void DialogueSystem::ShowDialogue(Interactable* specificInteractable) {
    if (!specificInteractable) return;
    
    currentType_ = specificInteractable->GetType();
    currentInteractable_ = specificInteractable;
    
    if (auto npc = dynamic_cast<NPC*>(specificInteractable)) {
        currentText_ = npc->GetCurrentDialogue();
    }
    
    isActive_ = true;
    displayTimer_ = 0.0f;
    fadeAlpha_ = 255.0f; // Start fully visible
}

void DialogueSystem::ShowDialogue(InteractableObject* specificObject) {
    if (!specificObject) return;
    
    currentType_ = specificObject->GetType();
    currentInteractable_ = specificObject; // InteractableObject inherits from Interactable
    
    auto dialogue = specificObject->GetDialogue();
    if (!dialogue.empty()) {
        currentText_ = dialogue[0]; // Show first dialogue
    }
    
    isActive_ = true;
    displayTimer_ = 0.0f;
    fadeAlpha_ = 255.0f; // Start fully visible
}

void DialogueSystem::NextDialogue() {
    if (currentType_ == InteractableType::NPC && currentInteractable_) {
        // Handle the specific NPC we're currently talking to
        if (auto npc = dynamic_cast<NPC*>(currentInteractable_)) {
            npc->NextDialogue();
            currentText_ = npc->GetCurrentDialogue();
            displayTimer_ = 0.0f;
        }
    } else {
        // Handle static zones
        for (auto& zone : interactionZones_) {
            if (zone.type == currentType_) {
                zone.currentDialogue = (zone.currentDialogue + 1) % zone.dialogues.size();
                currentText_ = zone.dialogues[zone.currentDialogue];
                displayTimer_ = 0.0f;
                break;
            }
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

void DialogueSystem::RegisterDynamicInteractable(Interactable* interactable) {
    if (interactable) {
        dynamicInteractables_.push_back(interactable);
    }
}

InteractableType DialogueSystem::CheckNearbyDynamicInteraction(const Vector2& playerPosition) {
    for (const auto& interactable : dynamicInteractables_) {
        if (interactable && interactable->IsPlayerInRange(playerPosition)) {
            return interactable->GetType();
        }
    }
    return InteractableType::NONE;
}

Interactable* DialogueSystem::GetNearbyInteractable(const Vector2& playerPosition) const {
    // Check dynamic interactables first (NPCs get priority)
    for (const auto& interactable : dynamicInteractables_) {
        if (interactable && interactable->IsPlayerInRange(playerPosition)) {
            return interactable;
        }
    }
    return nullptr;
}

std::vector<std::string> DialogueSystem::GetDialogueForType(InteractableType type) {
    // First check static zones
    for (const auto& zone : interactionZones_) {
        if (zone.type == type) {
            return zone.dialogues;
        }
    }
    
    // Then check dynamic interactables
    for (const auto& interactable : dynamicInteractables_) {
        if (interactable && interactable->GetType() == type) {
            return interactable->GetDialogue();
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
    SDL_Color promptTextColor = {255, 255, 255, 255};
    renderer->RenderText("Press SPACE to interact", promptX + 30, promptY + 18, promptTextColor, 16);
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
    
    // Render main dialogue text with wrapping
    if (!currentText_.empty()) {
        SDL_Color textColor = {255, 255, 255, 255};
        renderer->RenderWrappedText(currentText_, boxX + 20, boxY + 20, boxWidth - 40, textColor, 18);
    }
    
    // Control hints at bottom right
    SDL_Color hintColor = {160, 160, 180, 200};
    int hintY = boxY + boxHeight - 25;
    renderer->RenderText("E: More info", boxX + 20, hintY, hintColor, 14);
    renderer->RenderText("Q: Exit", boxX + 120, hintY, hintColor, 14);
}

