#pragma once
#include "Renderer.h"
#include <SDL.h>

class WorldRenderer {
public:
    WorldRenderer();
    ~WorldRenderer();
    
    void RenderWorld(Renderer* renderer, Vector2 cameraOffset);
    
private:
    void RenderSky(Renderer* renderer, Vector2 cameraOffset);
    void RenderGround(Renderer* renderer, Vector2 cameraOffset);
    void RenderWaterBorders(Renderer* renderer, Vector2 cameraOffset);
    void RenderHouse(Renderer* renderer, Vector2 cameraOffset);
    void RenderFarm(Renderer* renderer, Vector2 cameraOffset);
    void RenderGarden(Renderer* renderer, Vector2 cameraOffset);
    void RenderPaths(Renderer* renderer, Vector2 cameraOffset);
    
    // Helper methods for specific rendering tasks
    void RenderHouseTile(Renderer* renderer, int x, int y, bool isRoof, Vector2 cameraOffset);
    void RenderFarmTile(Renderer* renderer, int x, int y, Vector2 cameraOffset);
    void RenderGardenTile(Renderer* renderer, int x, int y, Vector2 cameraOffset);
    void RenderDirtPath(Renderer* renderer, int x, int y, bool isHorizontal, Vector2 cameraOffset);
    
    // World constants
    static const int TILE_SIZE = 128;
    static const int WORLD_WIDTH_TILES = 10;
    static const int WORLD_HEIGHT_TILES = 8;
    
    // Color palette
    struct GhibliColors {
        SDL_Color sky = {173, 216, 230, 255};
        SDL_Color grass = {118, 154, 57, 255};
        SDL_Color garden = {95, 127, 58, 255};
        SDL_Color water = {79, 143, 186, 255};
        SDL_Color houseRoof = {165, 42, 42, 255};
        SDL_Color houseWalls = {237, 201, 175, 255};
        SDL_Color houseBrown = {101, 67, 33, 255};
        SDL_Color farmSoil = {139, 90, 43, 255};
        SDL_Color dirtPath = {160, 130, 98, 255};
        SDL_Color roofAccent = {139, 26, 26, 255};
        SDL_Color windowBlue = {100, 149, 237, 255};
        SDL_Color gardenFlower = {255, 182, 193, 255};
    } colors_;
};