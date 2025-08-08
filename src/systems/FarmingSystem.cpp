#include "FarmingSystem.h"

FarmingSystem::FarmingSystem(int width, int height)
    : grid_width_(width), grid_height_(height), tile_size_(32) {
    farm_grid_.resize(grid_height_, std::vector<FarmTile>(grid_width_));
}

void FarmingSystem::Update(float deltaTime) {
    UpdateCropGrowth(deltaTime);
}

void FarmingSystem::UpdateCropGrowth(float deltaTime) {
    for (int y = 0; y < grid_height_; ++y) {
        for (int x = 0; x < grid_width_; ++x) {
            FarmTile& tile = farm_grid_[y][x];
            
            if (tile.stage == CropStage::GROWING && tile.watered) {
                tile.growth_time += static_cast<int>(deltaTime * 1000); // Convert to ms
                
                if (tile.growth_time >= tile.max_growth_time) {
                    tile.stage = CropStage::READY_TO_HARVEST;
                }
                
                tile.watered = false; // Reset watering status daily
            }
        }
    }
}

void FarmingSystem::Render(Renderer* renderer) {
    for (int y = 0; y < grid_height_; ++y) {
        for (int x = 0; x < grid_width_; ++x) {
            Vector2 position(x * tile_size_, y * tile_size_);
            const FarmTile& tile = farm_grid_[y][x];
            
            SDL_Color tileColor;
            
            switch (tile.stage) {
                case CropStage::EMPTY:
                    tileColor = {101, 67, 33, 255}; // Brown dirt
                    break;
                case CropStage::TILLED:
                    tileColor = {139, 69, 19, 255}; // Darker tilled dirt
                    break;
                case CropStage::PLANTED:
                    tileColor = {160, 82, 45, 255}; // Planted dirt
                    break;
                case CropStage::GROWING:
                    tileColor = {50, 205, 50, 255}; // Green growing
                    break;
                case CropStage::READY_TO_HARVEST:
                    tileColor = {255, 215, 0, 255}; // Golden ready
                    break;
            }
            
            Rect tileRect(position.x, position.y, tile_size_, tile_size_);
            renderer->DrawRect(tileRect, tileColor);
            
            // Draw border
            SDL_Color borderColor = {0, 0, 0, 255}; // Black border
            Rect borderRect(position.x, position.y, tile_size_, 2);
            renderer->DrawRect(borderRect, borderColor);
            borderRect = Rect(position.x, position.y, 2, tile_size_);
            renderer->DrawRect(borderRect, borderColor);
        }
    }
}

bool FarmingSystem::TillSoil(int x, int y) {
    if (!IsValidPosition(x, y)) return false;
    
    FarmTile& tile = farm_grid_[y][x];
    if (tile.stage == CropStage::EMPTY) {
        tile.stage = CropStage::TILLED;
        return true;
    }
    return false;
}

bool FarmingSystem::PlantSeed(int x, int y, CropType cropType) {
    if (!IsValidPosition(x, y)) return false;
    
    FarmTile& tile = farm_grid_[y][x];
    if (tile.stage == CropStage::TILLED) {
        tile.stage = CropStage::PLANTED;
        tile.crop_type = cropType;
        tile.max_growth_time = GetGrowthTimeForCrop(cropType);
        tile.growth_time = 0;
        
        // Start growing immediately
        tile.stage = CropStage::GROWING;
        return true;
    }
    return false;
}

bool FarmingSystem::WaterTile(int x, int y) {
    if (!IsValidPosition(x, y)) return false;
    
    FarmTile& tile = farm_grid_[y][x];
    if (tile.stage == CropStage::GROWING) {
        tile.watered = true;
        return true;
    }
    return false;
}

CropType FarmingSystem::HarvestCrop(int x, int y) {
    if (!IsValidPosition(x, y)) return CropType::NONE;
    
    FarmTile& tile = farm_grid_[y][x];
    if (tile.stage == CropStage::READY_TO_HARVEST) {
        CropType harvestedCrop = tile.crop_type;
        
        // Reset tile
        tile.stage = CropStage::EMPTY;
        tile.crop_type = CropType::NONE;
        tile.growth_time = 0;
        tile.max_growth_time = 0;
        tile.watered = false;
        
        return harvestedCrop;
    }
    return CropType::NONE;
}

bool FarmingSystem::IsValidPosition(int x, int y) const {
    return x >= 0 && x < grid_width_ && y >= 0 && y < grid_height_;
}

FarmTile* FarmingSystem::GetTile(int x, int y) {
    if (!IsValidPosition(x, y)) return nullptr;
    return &farm_grid_[y][x];
}

int FarmingSystem::GetGrowthTimeForCrop(CropType cropType) const {
    switch (cropType) {
        case CropType::POTATO: return 5000; // 5 seconds for demo
        case CropType::CARROT: return 7000; // 7 seconds
        case CropType::WHEAT: return 10000; // 10 seconds
        case CropType::TOMATO: return 8000; // 8 seconds
        default: return 5000;
    }
}