#include "Renderer.h"
#include "TextRenderer.h"
#include <iostream>

Renderer::Renderer() 
    : renderer_(nullptr), window_width_(0), window_height_(0) {
}

Renderer::~Renderer() {
    Shutdown();
}

bool Renderer::Initialize(SDL_Window* window) {
    renderer_ = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Enable alpha blending
    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
    
    SDL_GetWindowSize(window, &window_width_, &window_height_);
    
    // Initialize text renderer
    text_renderer_ = std::make_unique<TextRenderer>();
    if (!text_renderer_->Initialize(renderer_)) {
        std::cerr << "Warning: TextRenderer initialization failed, falling back to basic rendering" << std::endl;
        text_renderer_.reset();
    }
    
    return true;
}

void Renderer::Shutdown() {
    text_renderer_.reset();
    
    for (auto& pair : texture_cache_) {
        SDL_DestroyTexture(pair.second);
    }
    texture_cache_.clear();
    
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
}

void Renderer::Clear() {
    SDL_SetRenderDrawColor(renderer_, 173, 216, 230, 255); // Ghibli soft powder blue sky
    SDL_RenderClear(renderer_);
}

void Renderer::Present() {
    SDL_RenderPresent(renderer_);
}

SDL_Texture* Renderer::LoadTexture(const std::string& path) {
    auto it = texture_cache_.find(path);
    if (it != texture_cache_.end()) {
        return it->second;
    }
    
    SDL_Texture* texture = IMG_LoadTexture(renderer_, path.c_str());
    if (!texture) {
        std::cerr << "IMG_LoadTexture Error: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    
    texture_cache_[path] = texture;
    return texture;
}

void Renderer::DrawTexture(SDL_Texture* texture, const Vector2& position, const Rect* srcRect) {
    if (!texture) return;
    
    SDL_Rect destRect;
    destRect.x = static_cast<int>(position.x);
    destRect.y = static_cast<int>(position.y);
    
    if (srcRect) {
        destRect.w = srcRect->w;
        destRect.h = srcRect->h;
        SDL_Rect src = {srcRect->x, srcRect->y, srcRect->w, srcRect->h};
        SDL_RenderCopy(renderer_, texture, &src, &destRect);
    } else {
        SDL_QueryTexture(texture, nullptr, nullptr, &destRect.w, &destRect.h);
        SDL_RenderCopy(renderer_, texture, nullptr, &destRect);
    }
}

void Renderer::DrawRect(const Rect& rect, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
    SDL_Rect sdlRect = {rect.x, rect.y, rect.w, rect.h};
    SDL_RenderFillRect(renderer_, &sdlRect);
}

void Renderer::DrawTile(SDL_Texture* texture, int tileIndex, const Vector2& position, int tileSize) {
    if (!texture) return;
    
    int tilesPerRow = 10; // Assuming 10 tiles per row in spritesheet
    int srcX = (tileIndex % tilesPerRow) * tileSize;
    int srcY = (tileIndex / tilesPerRow) * tileSize;
    
    Rect srcRect(srcX, srcY, tileSize, tileSize);
    DrawTexture(texture, position, &srcRect);
}

void Renderer::DrawTextureWorld(SDL_Texture* texture, const Vector2& worldPosition, const Vector2& cameraOffset, const Rect* srcRect) {
    Vector2 screenPos(worldPosition.x - cameraOffset.x, worldPosition.y - cameraOffset.y);
    DrawTexture(texture, screenPos, srcRect);
}

void Renderer::DrawRectWorld(const Rect& worldRect, const Vector2& cameraOffset, SDL_Color color) {
    Rect screenRect(
        worldRect.x - static_cast<int>(cameraOffset.x),
        worldRect.y - static_cast<int>(cameraOffset.y),
        worldRect.w,
        worldRect.h
    );
    DrawRect(screenRect, color);
}

void Renderer::DrawTileWorld(SDL_Texture* texture, int tileIndex, const Vector2& worldPosition, const Vector2& cameraOffset, int tileSize) {
    Vector2 screenPos(worldPosition.x - cameraOffset.x, worldPosition.y - cameraOffset.y);
    DrawTile(texture, tileIndex, screenPos, tileSize);
}

void Renderer::RenderText(const std::string& text, int x, int y, SDL_Color color, int fontSize) {
    if (text_renderer_) {
        text_renderer_->RenderText(text, x, y, color, fontSize);
    }
}

void Renderer::RenderWrappedText(const std::string& text, int x, int y, int maxWidth, SDL_Color color, int fontSize) {
    if (text_renderer_) {
        text_renderer_->RenderWrappedText(text, x, y, maxWidth, color, fontSize);
    }
}