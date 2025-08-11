#include "NPC.h"

NPC::NPC() : position_(0, 0), currentDialogueIndex_(0) {
    dialogue_ = {"Hello, I'm a generic NPC!"};
}

NPC::NPC(int xPosition, int yPosition) : position_(xPosition, yPosition), currentDialogueIndex_(0) {
    dialogue_ = {"Hello, I'm a generic NPC!"};
}

NPC::NPC(int xPosition, int yPosition, const std::vector<std::string>& dialogue) 
    : position_(xPosition, yPosition), dialogue_(dialogue), currentDialogueIndex_(0) {
}

NPC::~NPC() {}

void NPC::Update(float deltaTime) {
    // NPCs can have idle animations, movement patterns, etc.
    // For now, the breeder NPC is stationary
}

void NPC::Render(Renderer* renderer, Vector2 cameraOffset) {
    // Render blue block NPC
    SDL_Color npcBlue = {50, 100, 200, 255};
    SDL_Color npcHighlight = {80, 130, 230, 255};
    SDL_Color npcShadow = {30, 60, 120, 255};
    
    // NPC shadow
    Rect shadowRect(position_.x + 4, position_.y + 4, NPC_WIDTH, NPC_HEIGHT);
    renderer->DrawRectWorld(shadowRect, cameraOffset, SDL_Color{0, 0, 0, 60});
    
    // Main NPC body
    Rect npcRect(position_.x, position_.y, NPC_WIDTH, NPC_HEIGHT);
    renderer->DrawRectWorld(npcRect, cameraOffset, npcBlue);
    
    // NPC highlight (top edge)
    Rect highlightRect(position_.x, position_.y, NPC_WIDTH, 6);
    renderer->DrawRectWorld(highlightRect, cameraOffset, npcHighlight);
    
    // NPC depth edge (right side)
    Rect depthRect(position_.x + NPC_WIDTH - 4, position_.y + 6, 4, NPC_HEIGHT - 6);
    renderer->DrawRectWorld(depthRect, cameraOffset, npcShadow);
    
    // Simple face (eyes)
    Rect leftEye(position_.x + 8, position_.y + 10, 4, 4);
    Rect rightEye(position_.x + 20, position_.y + 10, 4, 4);
    renderer->DrawRectWorld(leftEye, cameraOffset, SDL_Color{255, 255, 255, 255});
    renderer->DrawRectWorld(rightEye, cameraOffset, SDL_Color{255, 255, 255, 255});
    
    // Eye pupils
    Rect leftPupil(position_.x + 9, position_.y + 11, 2, 2);
    Rect rightPupil(position_.x + 21, position_.y + 11, 2, 2);
    renderer->DrawRectWorld(leftPupil, cameraOffset, SDL_Color{0, 0, 0, 255});
    renderer->DrawRectWorld(rightPupil, cameraOffset, SDL_Color{0, 0, 0, 255});
}

Vector2 NPC::GetPosition() const {
    return position_;
}

Rect NPC::GetInteractionBounds() const {
    return Rect(position_.x - 16, position_.y - 16, NPC_WIDTH + 32, NPC_HEIGHT + 32);
}

InteractableType NPC::GetType() const {
    return InteractableType::NPC;
}

std::vector<std::string> NPC::GetDialogue() const {
    return dialogue_;
}

void NPC::SetDialogue(const std::vector<std::string>& dialogue) {
    dialogue_ = dialogue;
}

Rect NPC::GetCollisionBounds() const {
    return Rect(position_.x, position_.y, NPC_WIDTH, NPC_HEIGHT);
}

std::string NPC::GetCurrentDialogue() const {
    if (dialogue_.empty()) return "";
    return dialogue_[currentDialogueIndex_];
}

void NPC::NextDialogue() {
    if (!dialogue_.empty()) {
        currentDialogueIndex_ = (currentDialogueIndex_ + 1) % dialogue_.size();
    }
}
