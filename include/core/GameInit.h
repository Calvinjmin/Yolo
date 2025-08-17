#pragma once
#include <SDL.h>
#include <memory>

class Renderer;
class InputManager;
class FarmingSystem;
class PotterySystem;
class Player;
class Camera;
class DialogueSystem;
class NPCManager;
class DynamicObjectManager;

class GameInit {
public:
    struct InitResult {
        std::unique_ptr<Renderer> renderer;
        std::unique_ptr<InputManager> input_manager;
        std::unique_ptr<FarmingSystem> farming_system;
        std::unique_ptr<PotterySystem> pottery_system;
        std::unique_ptr<Player> player;
        std::unique_ptr<Camera> camera;
        std::unique_ptr<DialogueSystem> dialogue_system;
        std::unique_ptr<NPCManager> npc_manager;
        std::unique_ptr<DynamicObjectManager> dynamic_object_manager;
    };

    static bool InitializeSDL();
    static SDL_Window* CreateGameWindow(const char* title, int width, int height);
    static InitResult InitializeGameSystems(SDL_Window* window, int window_width, int window_height);
    static void ShutdownSDL(SDL_Window* window);
};