#include "NPCManager.h"
#include "Renderer.h"
#include "DialogueSystem.h"

void NPCManager::AddNPC(const NPCData& npcData) {
    npcs_.push_back(std::make_unique<NPC>(npcData.x, npcData.y, npcData.dialogue));
    npc_names_.push_back(npcData.name);
}

void NPCManager::AddNPC(const std::string& name, float x, float y, const std::vector<std::string>& dialogue) {
    npcs_.push_back(std::make_unique<NPC>(x, y, dialogue));
    npc_names_.push_back(name);
}

void NPCManager::UpdateAll(float deltaTime) {
    for (auto& npc : npcs_) {
        if (npc) {
            npc->Update(deltaTime);
        }
    }
}

void NPCManager::RenderAll(Renderer* renderer, const Vector2& cameraOffset) {
    for (auto& npc : npcs_) {
        if (npc) {
            npc->Render(renderer, cameraOffset);
        }
    }
}

bool NPCManager::CheckCollisionWithAny(const Vector2& playerPosition) const {
    const int PLAYER_WIDTH = 32;
    const int PLAYER_HEIGHT = 32;
    
    Rect playerRect(
        static_cast<int>(playerPosition.x),
        static_cast<int>(playerPosition.y),
        PLAYER_WIDTH,
        PLAYER_HEIGHT
    );
    
    for (const auto& npc : npcs_) {
        if (npc) {
            Rect npcRect = npc->GetCollisionBounds();
            if (playerRect.x < npcRect.x + npcRect.w && 
                playerRect.x + playerRect.w > npcRect.x && 
                playerRect.y < npcRect.y + npcRect.h && 
                playerRect.y + playerRect.h > npcRect.y) {
                return true;
            }
        }
    }
    
    return false;
}

void NPCManager::RegisterAllWithDialogue(DialogueSystem* dialogueSystem) {
    for (auto& npc : npcs_) {
        if (npc && dialogueSystem) {
            dialogueSystem->RegisterDynamicInteractable(npc.get());
        }
    }
}

void NPCManager::Clear() {
    npcs_.clear();
    npc_names_.clear();
}

NPC* NPCManager::GetNPC(const std::string& name) {
    for (size_t i = 0; i < npc_names_.size(); ++i) {
        if (npc_names_[i] == name && i < npcs_.size()) {
            return npcs_[i].get();
        }
    }
    return nullptr;
}