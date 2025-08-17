#pragma once
#include "NPC.h"
#include <memory>
#include <vector>
#include <string>

struct NPCData {
    std::string name;
    float x, y;
    std::vector<std::string> dialogue;
};

class NPCManager {
public:
    NPCManager() = default;
    ~NPCManager() = default;

    void AddNPC(const NPCData& npcData);
    void AddNPC(const std::string& name, float x, float y, const std::vector<std::string>& dialogue);
    
    void UpdateAll(float deltaTime);
    void RenderAll(class Renderer* renderer, const Vector2& cameraOffset);
    
    bool CheckCollisionWithAny(const Vector2& playerPosition) const;
    void RegisterAllWithDialogue(class DialogueSystem* dialogueSystem);
    
    void Clear();
    size_t GetNPCCount() const { return npcs_.size(); }
    
    NPC* GetNPC(const std::string& name);
    const std::vector<std::unique_ptr<NPC>>& GetAllNPCs() const { return npcs_; }

private:
    std::vector<std::unique_ptr<NPC>> npcs_;
    std::vector<std::string> npc_names_;
};