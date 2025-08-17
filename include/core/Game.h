#pragma once
#include "Player.h"
#include <SDL.h>
#include <SDL_image.h>
#include <memory>
#include <vector>

class Renderer;
class InputManager;
class FarmingSystem;
class PotterySystem;
class Camera;
class DialogueSystem;
class NPCManager;
class DynamicObjectManager;

class Game {
public:
  Game();
  ~Game();

  bool Initialize();
  void Run();
  void Shutdown();

  static Game &Instance() { return *instance_; }

private:
  void Update(float deltaTime);
  void Render();
  void HandleEvents();
  bool CheckNPCCollision(const Vector2& playerPosition) const;

  bool running_;
  SDL_Window *window_;

  std::unique_ptr<Renderer> renderer_;
  std::unique_ptr<InputManager> input_manager_;
  std::unique_ptr<FarmingSystem> farming_system_;
  std::unique_ptr<PotterySystem> pottery_system_;
  std::unique_ptr<Player> player_;
  std::unique_ptr<Camera> camera_;
  std::unique_ptr<DialogueSystem> dialogue_system_;
  std::unique_ptr<NPCManager> npc_manager_;
  std::unique_ptr<DynamicObjectManager> dynamic_object_manager_;

  static Game *instance_;

  const int WINDOW_WIDTH = 1024;
  const int WINDOW_HEIGHT = 768;
  const char *WINDOW_TITLE = "Yolo";
};