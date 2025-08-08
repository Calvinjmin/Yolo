#include "Game.h"
#include "Renderer.h"
#include "InputManager.h"
#include "FarmingSystem.h"
#include "PotterySystem.h"
#include "Player.h"
#include "Camera.h"
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
    farming_system_ = std::make_unique<FarmingSystem>(20, 15); // 20x15 farm grid
    pottery_system_ = std::make_unique<PotterySystem>();
    player_ = std::make_unique<Player>();
    camera_ = std::make_unique<Camera>();
    
    // Set camera viewport to window size
    camera_->SetViewportSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // Set camera to follow player starting position
    camera_->SetTarget(player_->GetPosition());
    
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
    input_manager_->Update();
    
    if (input_manager_->IsActionPressed(InputAction::QUIT)) {
        running_ = false;
    }

    player_->HandleInput(input_manager_.get());
    player_->Update(deltaTime);
    
    // Update camera to follow player
    camera_->SetTarget(player_->GetPosition());
    camera_->Update(deltaTime);
    
    farming_system_->Update(deltaTime);
    pottery_system_->Update(deltaTime);
}

void Game::Render() {
    renderer_->Clear();
    
    Vector2 cameraOffset = camera_->GetOffset();
    
    // Draw colorful checkerboard pattern to visualize camera movement
    const int TILE_SIZE = 256;
    const int WORLD_WIDTH_TILES = 9216 / TILE_SIZE;  // 36 tiles
    const int WORLD_HEIGHT_TILES = 6912 / TILE_SIZE; // 27 tiles
    
    // Define a palette of bright colors
    SDL_Color colors[] = {
        {255, 100, 100, 255}, // Red
        {100, 255, 100, 255}, // Green  
        {100, 100, 255, 255}, // Blue
        {255, 255, 100, 255}, // Yellow
        {255, 100, 255, 255}, // Magenta
        {100, 255, 255, 255}, // Cyan
        {255, 150, 100, 255}, // Orange
        {150, 100, 255, 255}, // Purple
    };
    const int NUM_COLORS = sizeof(colors) / sizeof(colors[0]);
    
    for (int tileX = 0; tileX < WORLD_WIDTH_TILES; tileX++) {
        for (int tileY = 0; tileY < WORLD_HEIGHT_TILES; tileY++) {
            // Create a pattern based on tile coordinates
            int colorIndex = (tileX + tileY) % NUM_COLORS;
            SDL_Color tileColor = colors[colorIndex];
            
            Rect tileRect(tileX * TILE_SIZE, tileY * TILE_SIZE, TILE_SIZE, TILE_SIZE);
            renderer_->DrawRectWorld(tileRect, cameraOffset, tileColor);
        }
    }
    
    // Render farm first (background)
    // Section Rendering
    // farming_system_->Render(renderer_.get());
    // pottery_system_->Render(renderer_.get());

    // Render player with camera offset
    player_->Render(renderer_.get(), cameraOffset);
    
    renderer_->Present();
}

void Game::Shutdown() {
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