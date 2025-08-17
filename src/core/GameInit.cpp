#include "GameInit.h"
#include "Renderer.h"
#include "InputManager.h"
#include "FarmingSystem.h"
#include "PotterySystem.h"
#include "Player.h"
#include "NPCManager.h"
#include "DynamicObjectManager.h"
#include "Dog.h"
#include "FlowerPatch.h"
#include "Camera.h"
#include "DialogueSystem.h"
#include <iostream>

bool GameInit::InitializeSDL() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & (IMG_INIT_PNG | IMG_INIT_JPG))) {
        std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
        return false;
    }
    
    return true;
}

SDL_Window* GameInit::CreateGameWindow(const char* title, int width, int height) {
    SDL_Window* window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN
    );
    
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return nullptr;
    }
    
    return window;
}

GameInit::InitResult GameInit::InitializeGameSystems(SDL_Window* window, int window_width, int window_height) {
    InitResult result;
    
    // Initialize renderer
    result.renderer = std::make_unique<Renderer>();
    if (!result.renderer->Initialize(window)) {
        std::cerr << "Failed to initialize renderer!" << std::endl;
        return {};
    }
    
    // Initialize input manager
    result.input_manager = std::make_unique<InputManager>();
    
    // Initialize game systems
    result.farming_system = std::make_unique<FarmingSystem>(6, 4);
    result.pottery_system = std::make_unique<PotterySystem>();
    
    // Initialize player
    result.player = std::make_unique<Player>();
    
    // Initialize camera
    result.camera = std::make_unique<Camera>();
    result.camera->SetViewportSize(window_width, window_height);
    result.camera->SetTarget(result.player->GetPosition());
    
    // Initialize dialogue system
    result.dialogue_system = std::make_unique<DialogueSystem>();
    result.dialogue_system->Initialize();
    
    // Initialize NPC manager and create NPCs
    result.npc_manager = std::make_unique<NPCManager>();
    
    const int TILE_SIZE = 128;
    
    // Create breeder NPC (bottom left grass area)
    std::vector<std::string> breederDialogue = {
        "Hello there, traveler!",
        "I'm the village breeder.",
        "I take care of the animals around here.",
    };
    result.npc_manager->AddNPC("breeder", 1 * TILE_SIZE + 32, 6 * TILE_SIZE + 32, breederDialogue);
    
    // Create fisher NPC (top part near water)
    std::vector<std::string> fisherDialogue = {
        "Good day, friend!",
        "The fish are biting well today.",
        "Would you like to learn about fishing?",
    };
    result.npc_manager->AddNPC("fisher", 4 * TILE_SIZE + 32, 1 * TILE_SIZE + 32, fisherDialogue);
    
    // Register all NPCs with dialogue system
    result.npc_manager->RegisterAllWithDialogue(result.dialogue_system.get());
    
    // Initialize dynamic object manager and create dynamic objects
    result.dynamic_object_manager = std::make_unique<DynamicObjectManager>();
    
    // Create a dog that patrols in the garden area
    auto dog = std::make_unique<Dog>(4 * TILE_SIZE, 6 * TILE_SIZE + 50, 300.0f);
    result.dynamic_object_manager->AddObject(std::move(dog));
    
    // Create flower patches in the garden area
    std::vector<std::string> gardenFlowerDialogue = {
        "Beautiful flowers bloom here in vibrant colors.",
        "The sweet fragrance fills the air.",
        "These flowers attract butterflies and bees."
    };
    auto gardenFlowers1 = std::make_unique<FlowerPatch>(4 * TILE_SIZE + 40, 5 * TILE_SIZE + 40, gardenFlowerDialogue, "garden");
    auto gardenFlowers2 = std::make_unique<FlowerPatch>(6 * TILE_SIZE + 40, 6 * TILE_SIZE + 40, gardenFlowerDialogue, "garden");
    result.dynamic_object_manager->AddObject(std::move(gardenFlowers1));
    result.dynamic_object_manager->AddObject(std::move(gardenFlowers2));
    
    // Create flower patches in the farm area (where we rendered flowers)
    std::vector<std::string> farmFlowerDialogue1 = {
        "These lovely flowers brighten up the farm area.",
        "Pink, yellow, and coral blooms dance in the breeze.",
        "The flowers seem well-tended and healthy."
    };
    std::vector<std::string> farmFlowerDialogue2 = {
        "A colorful patch of flowers adds beauty to this corner.",
        "The farmer must have a soft spot for flowers.",
        "These blooms provide a nice contrast to the crops."
    };
    auto farmFlowers1 = std::make_unique<FlowerPatch>(6 * TILE_SIZE + 35, 2 * TILE_SIZE + 35, farmFlowerDialogue1, "farm");
    auto farmFlowers2 = std::make_unique<FlowerPatch>(8 * TILE_SIZE + 35, 4 * TILE_SIZE + 35, farmFlowerDialogue2, "farm");
    result.dynamic_object_manager->AddObject(std::move(farmFlowers1));
    result.dynamic_object_manager->AddObject(std::move(farmFlowers2));
    
    // Set up collision callback for NPCs and dynamic objects
    result.player->SetCollisionCallback([npcManager = result.npc_manager.get(), 
                                        dynamicObjManager = result.dynamic_object_manager.get()]
                                       (const Vector2& position) {
        bool npcCollision = npcManager ? npcManager->CheckCollisionWithAny(position) : false;
        bool dynamicCollision = dynamicObjManager ? dynamicObjManager->CheckCollisionWithAny(position) : false;
        return npcCollision || dynamicCollision;
    });
    
    return result;
}

void GameInit::ShutdownSDL(SDL_Window* window) {
    if (window) {
        SDL_DestroyWindow(window);
    }
    
    IMG_Quit();
    SDL_Quit();
}