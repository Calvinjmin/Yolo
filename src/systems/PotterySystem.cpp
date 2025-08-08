#include "PotterySystem.h"
#include <random>
#include <chrono>

PotterySystem::PotterySystem()
    : basic_clay_count_(10), red_clay_count_(5), white_clay_count_(3),
      is_crafting_(false), current_recipe_(nullptr), crafting_progress_(0.0f) {
    InitializeRecipes();
}

void PotterySystem::InitializeRecipes() {
    recipes_.emplace_back(PotteryType::POT, ClayType::BASIC_CLAY, 3, 3000, "Basic Pot");
    recipes_.emplace_back(PotteryType::BOWL, ClayType::BASIC_CLAY, 2, 2000, "Simple Bowl");
    recipes_.emplace_back(PotteryType::VASE, ClayType::RED_CLAY, 4, 5000, "Red Vase");
    recipes_.emplace_back(PotteryType::PLATE, ClayType::WHITE_CLAY, 2, 4000, "White Plate");
}

void PotterySystem::Update(float deltaTime) {
    if (is_crafting_ && current_recipe_) {
        crafting_progress_ += deltaTime * 1000; // Convert to ms
        
        if (crafting_progress_ >= current_recipe_->crafting_time) {
            // Crafting complete
            int quality = CalculateQuality();
            pottery_inventory_.emplace_back(current_recipe_->type, current_recipe_->name, quality);
            
            is_crafting_ = false;
            current_recipe_ = nullptr;
            crafting_progress_ = 0.0f;
        }
    }
}

void PotterySystem::Render(Renderer* renderer) {
    // Draw pottery kiln/workshop area in top-right corner
    int workshopX = renderer->GetWindowWidth() - 200;
    int workshopY = 20;
    
    // Workshop background
    SDL_Color workshopColor = {101, 67, 33, 200}; // Semi-transparent brown
    Rect workshopRect(workshopX, workshopY, 180, 150);
    renderer->DrawRect(workshopRect, workshopColor);
    
    // Clay inventory display
    SDL_Color textBg = {0, 0, 0, 150};
    int yOffset = workshopY + 10;
    
    // Basic clay
    Rect clayRect(workshopX + 10, yOffset, 160, 20);
    renderer->DrawRect(clayRect, textBg);
    yOffset += 25;
    
    // Red clay  
    clayRect.y = yOffset;
    renderer->DrawRect(clayRect, textBg);
    yOffset += 25;
    
    // White clay
    clayRect.y = yOffset;
    renderer->DrawRect(clayRect, textBg);
    yOffset += 30;
    
    // Crafting progress bar if crafting
    if (is_crafting_) {
        SDL_Color progressBg = {50, 50, 50, 255};
        SDL_Color progressFg = {100, 255, 100, 255};
        
        Rect progressBgRect(workshopX + 10, yOffset, 160, 15);
        renderer->DrawRect(progressBgRect, progressBg);
        
        float progress = crafting_progress_ / current_recipe_->crafting_time;
        if (progress > 1.0f) progress = 1.0f;
        
        Rect progressFgRect(workshopX + 10, yOffset, static_cast<int>(160 * progress), 15);
        renderer->DrawRect(progressFgRect, progressFg);
    }
}

bool PotterySystem::StartCrafting(const PotteryRecipe& recipe) {
    if (is_crafting_) return false;
    
    // Check if we have enough clay
    int availableClay = GetClayAmount(recipe.required_clay);
    if (availableClay < recipe.clay_amount) {
        return false;
    }
    
    // Remove clay from inventory
    switch (recipe.required_clay) {
        case ClayType::BASIC_CLAY:
            basic_clay_count_ -= recipe.clay_amount;
            break;
        case ClayType::RED_CLAY:
            red_clay_count_ -= recipe.clay_amount;
            break;
        case ClayType::WHITE_CLAY:
            white_clay_count_ -= recipe.clay_amount;
            break;
    }
    
    // Start crafting
    is_crafting_ = true;
    current_recipe_ = const_cast<PotteryRecipe*>(&recipe);
    crafting_progress_ = 0.0f;
    
    return true;
}

PotteryItem* PotterySystem::GetCompletedItem() {
    if (!pottery_inventory_.empty()) {
        return &pottery_inventory_.back();
    }
    return nullptr;
}

void PotterySystem::AddClay(ClayType clayType, int amount) {
    switch (clayType) {
        case ClayType::BASIC_CLAY:
            basic_clay_count_ += amount;
            break;
        case ClayType::RED_CLAY:
            red_clay_count_ += amount;
            break;
        case ClayType::WHITE_CLAY:
            white_clay_count_ += amount;
            break;
    }
}

int PotterySystem::GetClayAmount(ClayType clayType) const {
    switch (clayType) {
        case ClayType::BASIC_CLAY:
            return basic_clay_count_;
        case ClayType::RED_CLAY:
            return red_clay_count_;
        case ClayType::WHITE_CLAY:
            return white_clay_count_;
        default:
            return 0;
    }
}

int PotterySystem::CalculateQuality() const {
    // Simple random quality system (1-5 stars)
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 5);
    
    return dis(gen);
}