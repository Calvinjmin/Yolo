#pragma once
#include <string>
#include <vector>
#include <SDL.h>
#include "Renderer.h"
#include "Interactable.h"

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
    void RegisterDynamicInteractable(Interactable* interactable);
    InteractableType CheckNearbyDynamicInteraction(const Vector2& playerPosition);
    
private:
    bool isActive_;
    bool nearInteractable_;
    std::string currentText_;
    InteractableType currentType_;
    InteractableType nearbyType_;
    float displayTimer_;
    float fadeAlpha_;
    
    std::vector<InteractionZone> interactionZones_;
    std::vector<Interactable*> dynamicInteractables_;
    
    void RenderDialogueBox(Renderer* renderer, int windowWidth, int windowHeight);
    void RenderInteractionPrompt(Renderer* renderer, int windowWidth, int windowHeight);
    std::vector<std::string> GetDialogueForType(InteractableType type);
};