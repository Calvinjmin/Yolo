#pragma once
#include <string>
#include <vector>
#include <SDL.h>
#include "Renderer.h"

enum class InteractableType {
    HOUSE,
    FARM,
    GARDEN_FLOWER,
    GARDEN_BUSH,
    WATER,
    NONE
};

struct InteractionZone {
    Rect bounds;
    InteractableType type;
    std::vector<std::string> dialogues;
    int currentDialogue;
    
    InteractionZone(Rect b, InteractableType t, std::vector<std::string> d) 
        : bounds(b), type(t), dialogues(d), currentDialogue(0) {}
};

class DialogueSystem {
public:
    DialogueSystem();
    ~DialogueSystem();
    
    void Initialize();
    void Update(float deltaTime);
    void Render(Renderer* renderer, int windowWidth, int windowHeight);
    
    bool CheckInteraction(const Vector2& playerPosition, const Vector2& cameraOffset);
    InteractableType CheckNearbyInteraction(const Vector2& playerPosition) const;
    void ShowDialogue(InteractableType type);
    void HideDialogue();
    void NextDialogue();
    bool IsDialogueActive() const { return isActive_; }
    bool IsNearInteractable() const { return nearInteractable_; }
    void SetNearInteractable(bool near, InteractableType type = InteractableType::NONE);
    
    void SetupInteractionZones();
    
private:
    bool isActive_;
    bool nearInteractable_;
    std::string currentText_;
    InteractableType currentType_;
    InteractableType nearbyType_;
    float displayTimer_;
    float fadeAlpha_;
    
    std::vector<InteractionZone> interactionZones_;
    
    void RenderDialogueBox(Renderer* renderer, int windowWidth, int windowHeight);
    void RenderInteractionPrompt(Renderer* renderer, int windowWidth, int windowHeight);
    void RenderText(Renderer* renderer, const std::string& text, int x, int y);
    void RenderHintText(Renderer* renderer, const std::string& text, int x, int y, SDL_Color color);
    void RenderCharacter(Renderer* renderer, char c, int x, int y, int w, int h, SDL_Color color);
    std::vector<std::string> GetDialogueForType(InteractableType type);
};