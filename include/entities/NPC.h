#pragma once
#include "Renderer.h"
#include "Interactable.h"
#include <SDL.h>
#include <vector>

class NPC : public Interactable {
public:
  NPC();
  NPC(int xPosition, int yPosition);
  NPC(int xPosition, int yPosition, const std::vector<std::string>& dialogue);
  ~NPC();
  
  void SetDialogue(const std::vector<std::string>& dialogue);
  Rect GetCollisionBounds() const;

  // Interactable interface implementation
  void Update(float deltaTime) override;
  void Render(Renderer* renderer, Vector2 cameraOffset) override;
  Vector2 GetPosition() const override;
  Rect GetInteractionBounds() const override;
  InteractableType GetType() const override;
  std::vector<std::string> GetDialogue() const override;
  float GetInteractionRadius() const override { return 45.0f; }
  
  // Dialogue management
  std::string GetCurrentDialogue() const;
  void NextDialogue();

private:
  Vector2 position_;
  std::vector<std::string> dialogue_;
  int currentDialogueIndex_;

  const int NPC_WIDTH = 32;
  const int NPC_HEIGHT = 32;
};