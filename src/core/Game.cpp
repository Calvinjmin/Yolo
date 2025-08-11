#include "Game.h"
#include "Renderer.h"
#include "InputManager.h"
#include "FarmingSystem.h"
#include "PotterySystem.h"
#include "Player.h"
#include "NPC.h"
#include "Camera.h"
#include "DialogueSystem.h"
#include <iostream>
#include <chrono>
#include <type_traits>

Game* Game::instance_ = nullptr;

Game::Game() 
    : running_(false), window_(nullptr) {
    instance_ = this;
}

Game::~Game() {
    Shutdown();
}

bool Game::Initialize() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & (IMG_INIT_PNG | IMG_INIT_JPG))) {
        std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
        return false;
    }
    
    window_ = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    
    if (!window_) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    renderer_ = std::make_unique<Renderer>();
    if (!renderer_->Initialize(window_)) {
        std::cerr << "Failed to initialize renderer!" << std::endl;
        return false;
    }
    
    input_manager_ = std::make_unique<InputManager>();
    farming_system_ = std::make_unique<FarmingSystem>(6, 4);
    pottery_system_ = std::make_unique<PotterySystem>();
    player_ = std::make_unique<Player>();
    camera_ = std::make_unique<Camera>();
    dialogue_system_ = std::make_unique<DialogueSystem>();
    
    // Position NPC in bottom left grass area
    const int TILE_SIZE = 128;
    std::vector<std::string> breederDialogue = {
        "Hello there, traveler!",
        "I'm the village breeder.",
        "I take care of the animals around here.",
    };
    breeder_npc_ = std::make_unique<NPC>(1 * TILE_SIZE + 32, 6 * TILE_SIZE + 32, breederDialogue);
    
    // Set camera viewport to window size
    camera_->SetViewportSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // Set camera to follow player starting position
    camera_->SetTarget(player_->GetPosition());
    
    // Initialize dialogue system
    dialogue_system_->Initialize();
    
    // Register dynamic interactables
    dialogue_system_->RegisterDynamicInteractable(breeder_npc_.get());
    
    // Set up collision callback for NPCs
    player_->SetCollisionCallback([this](const Vector2& position) {
        return this->CheckNPCCollision(position);
    });
    
    running_ = true;
    return true;
}

void Game::Run() {
    auto lastTime = std::chrono::high_resolution_clock::now();
    
    while (running_) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        HandleEvents();
        Update(deltaTime);
        Render();
        
        SDL_Delay(16); // ~60 FPS
    }
}

void Game::HandleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running_ = false;
        }
        
        input_manager_->HandleEvent(event);
    }
}

void Game::Update(float deltaTime) {
    // CMD + Q quits the game
    if (input_manager_->IsActionPressed(InputAction::QUIT)) {
        running_ = false;
    }
    
    // ESC quits the dialogue
    if (input_manager_->IsActionPressed(InputAction::MENU)) {
        if (dialogue_system_->IsDialogueActive()) {
            dialogue_system_->HideDialogue(); 
        }
    }

    player_->HandleInput(input_manager_.get());
    player_->Update(deltaTime);
    
    // Update camera to follow player
    camera_->SetTarget(player_->GetPosition());
    camera_->Update(deltaTime);
    
    // Update dialogue system
    dialogue_system_->Update(deltaTime);
    
    Vector2 playerPos = player_->GetPosition();
    InteractableType nearbyType = dialogue_system_->CheckNearbyInteraction(playerPos);
    
    // Update dialogue system state based on proximity
    if (nearbyType != InteractableType::NONE) {
        dialogue_system_->SetNearInteractable(true, nearbyType);
    } else {
        dialogue_system_->SetNearInteractable(false);
    }
    
    // Handle dialogue interactions
    if (input_manager_->IsActionPressed(InputAction::INTERACT)) {
        if (nearbyType != InteractableType::NONE) {
            // Start interaction if near an object
            dialogue_system_->ShowDialogue(nearbyType);
        } else if (dialogue_system_->IsDialogueActive()) {
            // Hide dialogue if not near anything and dialogue is active
            dialogue_system_->HideDialogue();
        }
    }
    
    // Handle dialogue progression
    if (input_manager_->IsActionPressed(InputAction::USE_TOOL) && dialogue_system_->IsDialogueActive()) {
        dialogue_system_->NextDialogue();
    }
    
    
    // === Loading Objects ===
    farming_system_->Update(deltaTime);
    pottery_system_->Update(deltaTime);
    breeder_npc_->Update(deltaTime);
    
    // Update input manager at the end to prepare for next frame
    input_manager_->Update();
}

void Game::Render() {
    renderer_->Clear();
    
    Vector2 cameraOffset = camera_->GetOffset();
    
    const int TILE_SIZE = 128;
    const int WORLD_WIDTH_TILES = 10;  
    const int WORLD_HEIGHT_TILES = 8;  
    
    SDL_Color ghibliSky = {173, 216, 230, 255};          // Soft powder blue sky
    SDL_Color ghibliGrass = {118, 154, 57, 255};         // Warmer, more golden grass
    SDL_Color ghibliGarden = {95, 127, 58, 255};         // Lush garden green
    SDL_Color ghibliWater = {79, 143, 186, 255};         // Clearer, more vibrant water
    SDL_Color houseRoof = {165, 42, 42, 255};            // Warmer red roof
    SDL_Color houseWalls = {237, 201, 175, 255};         // Cream-colored walls
    SDL_Color houseBrown = {101, 67, 33, 255};           // Rich wood brown
    SDL_Color farmSoil = {139, 90, 43, 255};             // Rich earth brown
    SDL_Color dirtPath = {160, 130, 98, 255};            // Warm dirt path color
    
    SDL_Color roofAccent = {139, 26, 26, 255};           // Darker roof edges
    SDL_Color windowBlue = {100, 149, 237, 255};         // Window accents
    SDL_Color gardenFlower = {255, 182, 193, 255};       // Light pink flowers
    
    // Fill sky background (top portion)
    for (int y = 0; y < 2; y++) {
        for (int x = 0; x < WORLD_WIDTH_TILES; x++) {
            Rect skyRect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
            renderer_->DrawRectWorld(skyRect, cameraOffset, ghibliSky);
        }
    }
    
    // Ground base layer
    for (int y = 2; y < WORLD_HEIGHT_TILES; y++) {
        for (int x = 0; x < WORLD_WIDTH_TILES; x++) {
            Rect groundRect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
            renderer_->DrawRectWorld(groundRect, cameraOffset, ghibliGrass);
        }
    }
    
    // Top border
    for (int x = 0; x < WORLD_WIDTH_TILES; x++) {
        Rect waterRect(x * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE);
        renderer_->DrawRectWorld(waterRect, cameraOffset, ghibliWater);
        // Add wave lines
        for (int i = 0; i < 3; i++) {
            Rect waveRect(x * TILE_SIZE + 10 + i * 35, 20 + i * 25, 60, 6);
            renderer_->DrawRectWorld(waveRect, cameraOffset, SDL_Color{100, 160, 200, 255}); // Lighter blue waves
        }
    }
    // Bottom border
    for (int x = 0; x < WORLD_WIDTH_TILES; x++) {
        Rect waterRect(x * TILE_SIZE, (WORLD_HEIGHT_TILES-1) * TILE_SIZE, TILE_SIZE, TILE_SIZE);
        renderer_->DrawRectWorld(waterRect, cameraOffset, ghibliWater);
        // Add wave lines
        for (int i = 0; i < 3; i++) {
            Rect waveRect(x * TILE_SIZE + 15 + i * 30, (WORLD_HEIGHT_TILES-1) * TILE_SIZE + 30 + i * 20, 50, 6);
            renderer_->DrawRectWorld(waveRect, cameraOffset, SDL_Color{100, 160, 200, 255});
        }
    }
    // Left border
    for (int y = 1; y < WORLD_HEIGHT_TILES-1; y++) {
        Rect waterRect(0, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
        renderer_->DrawRectWorld(waterRect, cameraOffset, ghibliWater);
        // Add wave lines
        for (int i = 0; i < 3; i++) {
            Rect waveRect(20 + i * 25, y * TILE_SIZE + 10 + i * 35, 6, 60);
            renderer_->DrawRectWorld(waveRect, cameraOffset, SDL_Color{100, 160, 200, 255});
        }
    }
    // Right border
    for (int y = 1; y < WORLD_HEIGHT_TILES-1; y++) {
        Rect waterRect((WORLD_WIDTH_TILES-1) * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
        renderer_->DrawRectWorld(waterRect, cameraOffset, ghibliWater);
        // Add wave lines
        for (int i = 0; i < 3; i++) {
            Rect waveRect((WORLD_WIDTH_TILES-1) * TILE_SIZE + 30 + i * 20, y * TILE_SIZE + 15 + i * 30, 6, 50);
            renderer_->DrawRectWorld(waveRect, cameraOffset, SDL_Color{100, 160, 200, 255});
        }
    }
    
    // Detailed 3D-style house with shadows and depth (top-left area)
    for (int y = 2; y < 4; y++) {
        for (int x = 2; x < 4; x++) {
            if (y == 2) {
                // Drop shadow for roof
                Rect roofShadow(x * TILE_SIZE + 8, y * TILE_SIZE + 8, TILE_SIZE, TILE_SIZE);
                renderer_->DrawRectWorld(roofShadow, cameraOffset, SDL_Color{0, 0, 0, 80});
                
                // Roof base with gradient effect
                Rect roofRect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
                renderer_->DrawRectWorld(roofRect, cameraOffset, houseRoof);
                
                // Roof highlight (top edge)
                Rect roofHighlight(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, 12);
                renderer_->DrawRectWorld(roofHighlight, cameraOffset, SDL_Color{220, 60, 60, 255});
                
                // Roof shadow (bottom edge)
                Rect roofShadowEdge(x * TILE_SIZE, y * TILE_SIZE + 116, TILE_SIZE, 12);
                renderer_->DrawRectWorld(roofShadowEdge, cameraOffset, roofAccent);
                
                // Ghibli-style organic roof tiles with irregular spacing
                for (int i = 0; i < 4; i++) {
                    // Irregular tile rows for hand-drawn feel
                    int tileY = y * TILE_SIZE + i * 28 + ((i % 2) * 4); // Slightly uneven rows
                    
                    // Dark tile lines with slight curves
                    Rect tileLineH(x * TILE_SIZE, tileY, TILE_SIZE, 4);
                    renderer_->DrawRectWorld(tileLineH, cameraOffset, roofAccent);
                    
                    // Add individual tile highlights with variation
                    for (int j = 0; j < 4; j++) {
                        int tileX = x * TILE_SIZE + j * 30 + ((j % 2) * 3);
                        Rect tileHighlight(tileX, tileY + 4, 26, 3);
                        SDL_Color highlightColor = {
                            static_cast<Uint8>(180 + (j * 5)), 
                            static_cast<Uint8>(45 + (j * 3)), 
                            static_cast<Uint8>(45 + (j * 2)), 
                            255
                        };
                        renderer_->DrawRectWorld(tileHighlight, cameraOffset, highlightColor);
                    }
                }
                
                // Vertical roof divisions with 3D effect
                if (x == 3) {
                    Rect tileLineV(x * TILE_SIZE + 60, y * TILE_SIZE, 8, TILE_SIZE);
                    renderer_->DrawRectWorld(tileLineV, cameraOffset, roofAccent);
                    Rect tileHighlightV(x * TILE_SIZE + 62, y * TILE_SIZE, 4, TILE_SIZE);
                    renderer_->DrawRectWorld(tileHighlightV, cameraOffset, SDL_Color{200, 50, 50, 255});
                }
            } else {
                // Wall shadow
                Rect wallShadow(x * TILE_SIZE + 6, y * TILE_SIZE + 6, TILE_SIZE, TILE_SIZE);
                renderer_->DrawRectWorld(wallShadow, cameraOffset, SDL_Color{0, 0, 0, 60});
                
                // Wall base with gradient
                Rect wallRect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
                renderer_->DrawRectWorld(wallRect, cameraOffset, houseWalls);
                
                // Wall highlight (top)
                Rect wallHighlight(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, 8);
                renderer_->DrawRectWorld(wallHighlight, cameraOffset, SDL_Color{240, 200, 155, 255});
                
                // Wall shadow (bottom)
                Rect wallShadowEdge(x * TILE_SIZE, y * TILE_SIZE + 120, TILE_SIZE, 8);
                renderer_->DrawRectWorld(wallShadowEdge, cameraOffset, SDL_Color{190, 160, 115, 255});
                
                if (x == 2) {
                    // Enhanced door with depth
                    Rect doorShadow(x * TILE_SIZE + 42, y * TILE_SIZE + 22, 40, 80);
                    renderer_->DrawRectWorld(doorShadow, cameraOffset, SDL_Color{0, 0, 0, 40});
                    
                    Rect doorRect(x * TILE_SIZE + 40, y * TILE_SIZE + 20, 40, 80);
                    renderer_->DrawRectWorld(doorRect, cameraOffset, SDL_Color{139, 69, 19, 255});
                    
                    // Ghibli-style door with organic wood grain
                    Rect doorPanel1(x * TILE_SIZE + 44, y * TILE_SIZE + 25, 32, 30);
                    renderer_->DrawRectWorld(doorPanel1, cameraOffset, SDL_Color{160, 82, 22, 255});
                    Rect doorPanel2(x * TILE_SIZE + 44, y * TILE_SIZE + 60, 32, 30);
                    renderer_->DrawRectWorld(doorPanel2, cameraOffset, SDL_Color{160, 82, 22, 255});
                    
                    // Add wood grain lines
                    for (int k = 0; k < 8; k++) {
                        int grainY = y * TILE_SIZE + 28 + k * 8 + ((k % 2) * 2);
                        Rect woodGrain(x * TILE_SIZE + 45, grainY, 30, 1 + (k % 2));
                        SDL_Color grainColor = {
                            static_cast<Uint8>(120 + (k * 3)), 
                            static_cast<Uint8>(52 + (k * 2)), 
                            static_cast<Uint8>(12 + k), 
                            255
                        };
                        renderer_->DrawRectWorld(woodGrain, cameraOffset, grainColor);
                    }
                    
                    // Door knob with highlight
                    Rect knobShadow(x * TILE_SIZE + 73, y * TILE_SIZE + 56, 6, 6);
                    renderer_->DrawRectWorld(knobShadow, cameraOffset, SDL_Color{0, 0, 0, 60});
                    Rect knobRect(x * TILE_SIZE + 72, y * TILE_SIZE + 55, 6, 6);
                    renderer_->DrawRectWorld(knobRect, cameraOffset, SDL_Color{255, 215, 0, 255});
                    Rect knobHighlight(x * TILE_SIZE + 72, y * TILE_SIZE + 55, 3, 3);
                    renderer_->DrawRectWorld(knobHighlight, cameraOffset, SDL_Color{255, 255, 200, 255});
                } else {
                    // Enhanced window with depth and reflection
                    Rect windowShadow(x * TILE_SIZE + 32, y * TILE_SIZE + 32, 60, 50);
                    renderer_->DrawRectWorld(windowShadow, cameraOffset, SDL_Color{0, 0, 0, 40});
                    
                    Rect windowRect(x * TILE_SIZE + 30, y * TILE_SIZE + 30, 60, 50);
                    renderer_->DrawRectWorld(windowRect, cameraOffset, windowBlue);
                    
                    // Window reflection
                    Rect windowReflection(x * TILE_SIZE + 35, y * TILE_SIZE + 35, 25, 20);
                    renderer_->DrawRectWorld(windowReflection, cameraOffset, SDL_Color{200, 220, 255, 180});
                    
                    // Enhanced window frame with beveled edges
                    Rect frameH1(x * TILE_SIZE + 26, y * TILE_SIZE + 26, 68, 6);
                    Rect frameH2(x * TILE_SIZE + 26, y * TILE_SIZE + 78, 68, 6);
                    Rect frameV1(x * TILE_SIZE + 26, y * TILE_SIZE + 26, 6, 58);
                    Rect frameV2(x * TILE_SIZE + 86, y * TILE_SIZE + 26, 6, 58);
                    renderer_->DrawRectWorld(frameH1, cameraOffset, houseBrown);
                    renderer_->DrawRectWorld(frameH2, cameraOffset, houseBrown);
                    renderer_->DrawRectWorld(frameV1, cameraOffset, houseBrown);
                    renderer_->DrawRectWorld(frameV2, cameraOffset, houseBrown);
                    
                    // Frame highlights
                    Rect frameHighlightH1(x * TILE_SIZE + 26, y * TILE_SIZE + 26, 68, 2);
                    Rect frameHighlightV1(x * TILE_SIZE + 26, y * TILE_SIZE + 26, 2, 58);
                    renderer_->DrawRectWorld(frameHighlightH1, cameraOffset, SDL_Color{180, 100, 40, 255});
                    renderer_->DrawRectWorld(frameHighlightV1, cameraOffset, SDL_Color{180, 100, 40, 255});
                    
                    // Window cross with depth
                    Rect crossH(x * TILE_SIZE + 30, y * TILE_SIZE + 50, 60, 6);
                    Rect crossV(x * TILE_SIZE + 56, y * TILE_SIZE + 30, 6, 50);
                    renderer_->DrawRectWorld(crossH, cameraOffset, houseBrown);
                    renderer_->DrawRectWorld(crossV, cameraOffset, houseBrown);
                    
                    // Cross highlights
                    Rect crossHighlightH(x * TILE_SIZE + 30, y * TILE_SIZE + 50, 60, 2);
                    Rect crossHighlightV(x * TILE_SIZE + 56, y * TILE_SIZE + 30, 2, 50);
                    renderer_->DrawRectWorld(crossHighlightH, cameraOffset, SDL_Color{180, 100, 40, 255});
                    renderer_->DrawRectWorld(crossHighlightV, cameraOffset, SDL_Color{180, 100, 40, 255});
                }
            }
        }
    }
    
    // Enhanced 3D raised farm beds (top-right, visible from start)
    for (int y = 2; y < 5; y++) {
        for (int x = 6; x < 9; x++) {
            // Drop shadow for raised bed
            Rect bedShadow(x * TILE_SIZE + 4, y * TILE_SIZE + 4, TILE_SIZE, TILE_SIZE);
            renderer_->DrawRectWorld(bedShadow, cameraOffset, SDL_Color{0, 0, 0, 40});
            
            // Raised bed border (wooden frame)
            Rect bedBorder(x * TILE_SIZE - 4, y * TILE_SIZE - 4, TILE_SIZE + 8, TILE_SIZE + 8);
            renderer_->DrawRectWorld(bedBorder, cameraOffset, SDL_Color{101, 67, 33, 255}); // Dark wood
            
            // Bed border highlights
            Rect borderHighlight(x * TILE_SIZE - 4, y * TILE_SIZE - 4, TILE_SIZE + 8, 4);
            renderer_->DrawRectWorld(borderHighlight, cameraOffset, SDL_Color{139, 90, 43, 255}); // Lighter wood
            Rect borderHighlightL(x * TILE_SIZE - 4, y * TILE_SIZE - 4, 4, TILE_SIZE + 8);
            renderer_->DrawRectWorld(borderHighlightL, cameraOffset, SDL_Color{139, 90, 43, 255});
            
            // Soil base with texture
            SDL_Color soilColor = ((x + y) % 2 == 0) ? farmSoil : SDL_Color{122, 77, 38, 255};
            Rect farmRect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
            renderer_->DrawRectWorld(farmRect, cameraOffset, soilColor);
            
            // Add raised soil texture
            for (int i = 0; i < 16; i++) {
                int soilX = x * TILE_SIZE + 8 + (i % 4) * 28;
                int soilY = y * TILE_SIZE + 8 + (i / 4) * 28;
                Rect soilClump(soilX, soilY, 20, 20);
                SDL_Color clumpColor = ((i % 3) == 0) ? 
                    SDL_Color{110, 70, 30, 255} : SDL_Color{130, 85, 40, 255};
                renderer_->DrawRectWorld(soilClump, cameraOffset, clumpColor);
            }
            
            // Enhanced furrow lines with depth
            for (int i = 0; i < 3; i++) {
                // Dark furrow bottom
                Rect furrowRect(x * TILE_SIZE + 10, y * TILE_SIZE + 20 + i * 35, TILE_SIZE - 20, 8);
                renderer_->DrawRectWorld(furrowRect, cameraOffset, SDL_Color{78, 44, 22, 255});
                // Furrow highlight edge
                Rect furrowHighlight(x * TILE_SIZE + 10, y * TILE_SIZE + 18 + i * 35, TILE_SIZE - 20, 2);
                renderer_->DrawRectWorld(furrowHighlight, cameraOffset, SDL_Color{150, 95, 45, 255});
            }
            
            // Enhanced crop sprouts with shadows
            if ((x + y) % 2 == 1) {
                for (int i = 0; i < 4; i++) {
                    int sproutX = x * TILE_SIZE + 20 + (i % 2) * 60;
                    int sproutY = y * TILE_SIZE + 30 + (i / 2) * 40;
                    
                    // Sprout shadow
                    Rect sproutShadow(sproutX + 2, sproutY + 2, 12, 16);
                    renderer_->DrawRectWorld(sproutShadow, cameraOffset, SDL_Color{0, 0, 0, 30});
                    
                    // Sprout base
                    Rect sproutRect(sproutX, sproutY, 12, 16);
                    renderer_->DrawRectWorld(sproutRect, cameraOffset, SDL_Color{34, 139, 34, 255});
                    
                    // Sprout highlight
                    Rect sproutHighlight(sproutX, sproutY, 4, 6);
                    renderer_->DrawRectWorld(sproutHighlight, cameraOffset, SDL_Color{60, 180, 60, 255});
                    
                    // Small leaves
                    Rect leaf1(sproutX + 2, sproutY - 2, 8, 4);
                    Rect leaf2(sproutX + 4, sproutY - 4, 6, 3);
                    renderer_->DrawRectWorld(leaf1, cameraOffset, SDL_Color{50, 160, 50, 255});
                    renderer_->DrawRectWorld(leaf2, cameraOffset, SDL_Color{40, 150, 40, 255});
                }
            }
        }
    }
    
    // Enhanced 3D garden with depth and shadows (bottom-center, visible from start)
    for (int y = 5; y < 7; y++) {
        for (int x = 3; x < 7; x++) {
            // Garden grass base with subtle texture
            Rect gardenRect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
            renderer_->DrawRectWorld(gardenRect, cameraOffset, ghibliGarden);
            
            // Add grass texture variation
            for (int i = 0; i < 20; i++) {
                int grassX = x * TILE_SIZE + 5 + (i % 5) * 24;
                int grassY = y * TILE_SIZE + 5 + (i / 5) * 24;
                Rect grassPatch(grassX, grassY, 16, 16);
                SDL_Color grassVariation = ((i % 3) == 0) ? 
                    SDL_Color{95, 117, 42, 255} : SDL_Color{75, 97, 37, 255};
                renderer_->DrawRectWorld(grassPatch, cameraOffset, grassVariation);
            }
            
            // Enhanced flower clusters with 3D effect
            if ((x == 4 && y == 5) || (x == 6 && y == 6)) {
                // Flower patch shadow
                Rect flowerShadow(x * TILE_SIZE + 23, y * TILE_SIZE + 23, 80, 80);
                renderer_->DrawRectWorld(flowerShadow, cameraOffset, SDL_Color{0, 0, 0, 30});
                
                // Raised flower patch base
                Rect flowerPatchRect(x * TILE_SIZE + 20, y * TILE_SIZE + 20, 80, 80);
                renderer_->DrawRectWorld(flowerPatchRect, cameraOffset, SDL_Color{60, 92, 32, 255});
                
                // Flower patch highlight edge
                Rect patchHighlight(x * TILE_SIZE + 20, y * TILE_SIZE + 20, 80, 4);
                renderer_->DrawRectWorld(patchHighlight, cameraOffset, SDL_Color{80, 112, 42, 255});
                
                // Individual 3D flowers
                for (int i = 0; i < 6; i++) {
                    int flowerX = x * TILE_SIZE + 30 + (i % 3) * 25;
                    int flowerY = y * TILE_SIZE + 30 + (i / 3) * 25;
                    
                    // Flower shadow
                    Rect flowerShadowSmall(flowerX + 2, flowerY + 2, 8, 8);
                    renderer_->DrawRectWorld(flowerShadowSmall, cameraOffset, SDL_Color{0, 0, 0, 40});
                    
                    // Flower stem
                    Rect flowerStem(flowerX + 3, flowerY + 6, 2, 8);
                    renderer_->DrawRectWorld(flowerStem, cameraOffset, SDL_Color{34, 139, 34, 255});
                    
                    // Flower head
                    Rect flowerRect(flowerX, flowerY, 8, 8);
                    SDL_Color flowerColor = (i % 2 == 0) ? gardenFlower : SDL_Color{255, 255, 0, 255};
                    renderer_->DrawRectWorld(flowerRect, cameraOffset, flowerColor);
                    
                    // Flower highlight
                    Rect flowerHighlight(flowerX + 1, flowerY + 1, 3, 3);
                    SDL_Color highlightColor = (i % 2 == 0) ? 
                        SDL_Color{255, 220, 220, 255} : SDL_Color{255, 255, 180, 255};
                    renderer_->DrawRectWorld(flowerHighlight, cameraOffset, highlightColor);
                    
                    // Flower center
                    Rect flowerCenter(flowerX + 3, flowerY + 3, 2, 2);
                    renderer_->DrawRectWorld(flowerCenter, cameraOffset, SDL_Color{255, 165, 0, 255});
                }
            }
            
            // Enhanced 3D bushes
            if ((x + y) % 3 == 1) {
                // Bush shadow
                Rect bushShadow(x * TILE_SIZE + 43, y * TILE_SIZE + 43, 40, 30);
                renderer_->DrawRectWorld(bushShadow, cameraOffset, SDL_Color{0, 0, 0, 50});
                
                // Bush base
                Rect bushRect(x * TILE_SIZE + 40, y * TILE_SIZE + 40, 40, 30);
                renderer_->DrawRectWorld(bushRect, cameraOffset, SDL_Color{34, 85, 34, 255});
                
                // Bush layers for depth
                Rect bushLayer1(x * TILE_SIZE + 42, y * TILE_SIZE + 38, 36, 25);
                renderer_->DrawRectWorld(bushLayer1, cameraOffset, SDL_Color{44, 95, 44, 255});
                
                Rect bushLayer2(x * TILE_SIZE + 45, y * TILE_SIZE + 35, 30, 20);
                renderer_->DrawRectWorld(bushLayer2, cameraOffset, SDL_Color{54, 105, 54, 255});
                
                // Bush highlights
                Rect bushHighlight1(x * TILE_SIZE + 40, y * TILE_SIZE + 37, 20, 6);
                renderer_->DrawRectWorld(bushHighlight1, cameraOffset, SDL_Color{70, 130, 70, 255});
                
                Rect bushHighlight2(x * TILE_SIZE + 55, y * TILE_SIZE + 40, 15, 4);
                renderer_->DrawRectWorld(bushHighlight2, cameraOffset, SDL_Color{80, 140, 80, 255});
            }
            
            // Enhanced grass blade details with shadows
            for (int i = 0; i < 12; i++) {
                int grassX = x * TILE_SIZE + 8 + (i % 4) * 28;
                int grassY = y * TILE_SIZE + 8 + (i / 4) * 28;
                
                // Grass shadow
                Rect grassShadow(grassX + 1, grassY + 1, 3, 12);
                renderer_->DrawRectWorld(grassShadow, cameraOffset, SDL_Color{0, 0, 0, 20});
                
                // Grass blade
                Rect grassDetail(grassX, grassY, 3, 12);
                SDL_Color grassColor = ((i % 3) == 0) ? 
                    SDL_Color{120, 160, 50, 255} : SDL_Color{100, 140, 40, 255};
                renderer_->DrawRectWorld(grassDetail, cameraOffset, grassColor);
                
                // Grass highlight
                Rect grassHighlight(grassX, grassY, 1, 6);
                renderer_->DrawRectWorld(grassHighlight, cameraOffset, SDL_Color{140, 180, 60, 255});
            }
        }
    }
    
    // Ghibli-style organic dirt paths - horizontal path from house to farm
    for (int x = 4; x < 6; x++) {
        // Worn dirt path base with natural variation
        Rect pathRect(x * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE);
        renderer_->DrawRectWorld(pathRect, cameraOffset, dirtPath);
        
        // Add natural dirt texture with organic patches
        for (int i = 0; i < 24; i++) {
            int dirtX = x * TILE_SIZE + 8 + (i % 6) * 18 + ((i % 3) * 4); // Irregular spacing
            int dirtY = 3 * TILE_SIZE + 8 + (i / 6) * 25 + ((i % 2) * 6);
            int patchSize = 12 + (i % 8); // Varied patch sizes
            
            SDL_Color dirtVariation;
            if (i % 4 == 0) {
                dirtVariation = {140, 110, 78, 255}; // Darker dirt
            } else if (i % 4 == 1) {
                dirtVariation = {175, 145, 118, 255}; // Lighter dirt
            } else if (i % 4 == 2) {
                dirtVariation = {155, 120, 85, 255}; // Medium dirt
            } else {
                dirtVariation = {130, 105, 70, 255}; // Rich earth
            }
            
            Rect dirtPatch(dirtX, dirtY, patchSize, patchSize - 2);
            renderer_->DrawRectWorld(dirtPatch, cameraOffset, dirtVariation);
        }
        
        // Add small pebbles and natural debris
        for (int i = 0; i < 8; i++) {
            int pebbleX = x * TILE_SIZE + 15 + (i % 3) * 35 + ((i % 2) * 10);
            int pebbleY = 3 * TILE_SIZE + 20 + (i / 3) * 30 + ((i % 3) * 8);
            
            // Small pebble
            Rect pebble(pebbleX, pebbleY, 4 + (i % 3), 3 + (i % 2));
            SDL_Color pebbleColor = ((i % 2) == 0) ? 
                SDL_Color{120, 115, 110, 255} : SDL_Color{105, 100, 95, 255};
            renderer_->DrawRectWorld(pebble, cameraOffset, pebbleColor);
        }
        
        // Natural grass edges bleeding into path
        for (int i = 0; i < 6; i++) {
            int grassX = x * TILE_SIZE + (i % 2) * 110 + ((i % 3) * 8);
            int grassY = 3 * TILE_SIZE + (i / 2) * 40 + ((i % 2) * 12);
            
            Rect grassTuft(grassX, grassY, 8, 6);
            renderer_->DrawRectWorld(grassTuft, cameraOffset, SDL_Color{108, 144, 47, 200}); // Semi-transparent grass
        }
    }
    
    // Ghibli-style organic dirt path - vertical path from center to garden
    for (int y = 4; y < 5; y++) {
        for (int pathX = 4; pathX < 6; pathX++) {
            // Worn dirt path base
            Rect pathRect(pathX * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
            renderer_->DrawRectWorld(pathRect, cameraOffset, dirtPath);
            
            // Add natural dirt texture with organic patches
            for (int i = 0; i < 20; i++) {
                int dirtX = pathX * TILE_SIZE + 6 + (i % 5) * 22 + ((i % 3) * 5);
                int dirtY = y * TILE_SIZE + 6 + (i / 5) * 28 + ((i % 2) * 7);
                int patchSize = 10 + (i % 6);
                
                SDL_Color dirtVariation;
                if (i % 5 == 0) {
                    dirtVariation = {135, 105, 75, 255}; // Rich earth
                } else if (i % 5 == 1) {
                    dirtVariation = {170, 140, 115, 255}; // Light dirt
                } else if (i % 5 == 2) {
                    dirtVariation = {145, 115, 80, 255}; // Medium dirt
                } else if (i % 5 == 3) {
                    dirtVariation = {125, 95, 65, 255}; // Dark soil
                } else {
                    dirtVariation = {160, 125, 90, 255}; // Sandy dirt
                }
                
                Rect dirtPatch(dirtX, dirtY, patchSize, patchSize - 1);
                renderer_->DrawRectWorld(dirtPatch, cameraOffset, dirtVariation);
            }
            
            // Add footprint-like indentations for Ghibli charm
            for (int i = 0; i < 6; i++) {
                int footX = pathX * TILE_SIZE + 25 + (i % 2) * 45 + ((i % 3) * 6);
                int footY = y * TILE_SIZE + 15 + (i / 2) * 35;
                
                // Footprint depression
                Rect footprint(footX, footY, 18, 10);
                renderer_->DrawRectWorld(footprint, cameraOffset, SDL_Color{125, 95, 65, 255});
                
                // Footprint toes
                for (int j = 0; j < 3; j++) {
                    Rect toe(footX + 2 + j * 4, footY - 2, 3, 3);
                    renderer_->DrawRectWorld(toe, cameraOffset, SDL_Color{120, 90, 60, 255});
                }
            }
            
            // Scattered pebbles and organic elements
            for (int i = 0; i < 10; i++) {
                int pebbleX = pathX * TILE_SIZE + 10 + (i % 4) * 28 + ((i % 2) * 12);
                int pebbleY = y * TILE_SIZE + 12 + (i / 4) * 35 + ((i % 3) * 10);
                
                Rect pebble(pebbleX, pebbleY, 3 + (i % 4), 2 + (i % 3));
                SDL_Color pebbleColor = ((i % 3) == 0) ? 
                    SDL_Color{115, 110, 105, 255} : SDL_Color{95, 90, 85, 255};
                renderer_->DrawRectWorld(pebble, cameraOffset, pebbleColor);
            }
            
            // Natural grass tufts growing along path edges
            for (int i = 0; i < 8; i++) {
                int grassX = pathX * TILE_SIZE + (i % 2) * 115 + ((i % 4) * 6);
                int grassY = y * TILE_SIZE + 5 + (i / 2) * 28 + ((i % 3) * 8);
                
                Rect grassTuft(grassX, grassY, 6 + (i % 3), 8);
                renderer_->DrawRectWorld(grassTuft, cameraOffset, SDL_Color{108, 144, 47, 180});
            }
        }
    }
    

    // Render NPC
    breeder_npc_->Render(renderer_.get(), cameraOffset);
    
    // Render player with camera offset
    player_->Render(renderer_.get(), cameraOffset);
    
    // Render dialogue system (UI overlay)
    dialogue_system_->Render(renderer_.get(), WINDOW_WIDTH, WINDOW_HEIGHT);
    
    renderer_->Present();
}

bool Game::CheckNPCCollision(const Vector2& playerPosition) const {
    const int PLAYER_WIDTH = 32;
    const int PLAYER_HEIGHT = 32;
    
    // Player collision rectangle
    Rect playerRect(
        static_cast<int>(playerPosition.x),
        static_cast<int>(playerPosition.y),
        PLAYER_WIDTH,
        PLAYER_HEIGHT
    );
    
    // Check collision with breeder NPC
    if (breeder_npc_) {
        Rect npcRect = breeder_npc_->GetCollisionBounds();
        if (playerRect.x < npcRect.x + npcRect.w && 
            playerRect.x + playerRect.w > npcRect.x && 
            playerRect.y < npcRect.y + npcRect.h && 
            playerRect.y + playerRect.h > npcRect.y) {
            return true;
        }
    }
    
    return false;
}

void Game::Shutdown() {
    dialogue_system_.reset();
    breeder_npc_.reset();
    camera_.reset();
    pottery_system_.reset();
    farming_system_.reset();
    input_manager_.reset();
    player_.reset();
    renderer_.reset();
    
    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    
    IMG_Quit();
    SDL_Quit();
}