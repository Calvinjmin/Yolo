#pragma once
#include <vector>
#include <memory>
#include "Renderer.h"

enum class CropType {
    NONE,
    POTATO,
    CARROT,
    WHEAT,
    TOMATO
};

enum class CropStage {
    EMPTY,
    TILLED,
    PLANTED,
    GROWING,
    READY_TO_HARVEST
};

struct FarmTile {
    CropStage stage = CropStage::EMPTY;
    CropType crop_type = CropType::NONE;
    int growth_time = 0;
    int max_growth_time = 0;
    bool watered = false;
    
    FarmTile() = default;
};

class FarmingSystem {
public:
    FarmingSystem(int width, int height);
    
    void Update(float deltaTime);
    void Render(Renderer* renderer);
    
    bool TillSoil(int x, int y);
    bool PlantSeed(int x, int y, CropType cropType);
    bool WaterTile(int x, int y);
    CropType HarvestCrop(int x, int y);
    
    bool IsValidPosition(int x, int y) const;
    FarmTile* GetTile(int x, int y);
    
private:
    std::vector<std::vector<FarmTile>> farm_grid_;
    int grid_width_;
    int grid_height_;
    int tile_size_;
    
    void UpdateCropGrowth(float deltaTime);
    int GetGrowthTimeForCrop(CropType cropType) const;
};