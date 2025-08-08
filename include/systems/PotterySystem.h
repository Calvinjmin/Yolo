#pragma once
#include <vector>
#include <string>
#include "Renderer.h"

enum class ClayType {
    BASIC_CLAY,
    RED_CLAY,
    WHITE_CLAY
};

enum class PotteryType {
    POT,
    VASE,
    BOWL,
    PLATE
};

struct PotteryRecipe {
    PotteryType type;
    ClayType required_clay;
    int clay_amount;
    int crafting_time;
    std::string name;
    
    PotteryRecipe(PotteryType t, ClayType c, int amount, int time, const std::string& n)
        : type(t), required_clay(c), clay_amount(amount), crafting_time(time), name(n) {}
};

struct PotteryItem {
    PotteryType type;
    std::string name;
    int quality; // 1-5 star rating
    
    PotteryItem(PotteryType t, const std::string& n, int q = 1) 
        : type(t), name(n), quality(q) {}
};

class PotterySystem {
public:
    PotterySystem();
    
    void Update(float deltaTime);
    void Render(Renderer* renderer);
    
    bool StartCrafting(const PotteryRecipe& recipe);
    bool IsCrafting() const { return is_crafting_; }
    PotteryItem* GetCompletedItem();
    
    void AddClay(ClayType clayType, int amount);
    int GetClayAmount(ClayType clayType) const;
    
    const std::vector<PotteryRecipe>& GetAvailableRecipes() const { return recipes_; }
    const std::vector<PotteryItem>& GetInventory() const { return pottery_inventory_; }
    
private:
    std::vector<PotteryRecipe> recipes_;
    std::vector<PotteryItem> pottery_inventory_;
    
    // Clay inventory
    int basic_clay_count_;
    int red_clay_count_;
    int white_clay_count_;
    
    // Crafting state
    bool is_crafting_;
    PotteryRecipe* current_recipe_;
    float crafting_progress_;
    
    void InitializeRecipes();
    int CalculateQuality() const; // Random quality calculation
};