#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <unordered_map>
#include <memory>

class TextRenderer;

struct Vector2 {
    float x, y;
    Vector2(float x = 0, float y = 0) : x(x), y(y) {}
};

struct Rect {
    int x, y, w, h;
    Rect(int x = 0, int y = 0, int w = 0, int h = 0) : x(x), y(y), w(w), h(h) {}
};

class Renderer {
public:
    Renderer();
    ~Renderer();
    
    bool Initialize(SDL_Window* window);
    void Shutdown();
    
    void Clear();
    void Present();
    
    SDL_Texture* LoadTexture(const std::string& path);
    void DrawTexture(SDL_Texture* texture, const Vector2& position, const Rect* srcRect = nullptr);
    void DrawRect(const Rect& rect, SDL_Color color);
    void DrawTile(SDL_Texture* texture, int tileIndex, const Vector2& position, int tileSize = 32);
    
    // Camera-aware drawing methods
    void DrawTextureWorld(SDL_Texture* texture, const Vector2& worldPosition, const Vector2& cameraOffset, const Rect* srcRect = nullptr);
    void DrawRectWorld(const Rect& worldRect, const Vector2& cameraOffset, SDL_Color color);
    void DrawTileWorld(SDL_Texture* texture, int tileIndex, const Vector2& worldPosition, const Vector2& cameraOffset, int tileSize = 32);
    
    int GetWindowWidth() const { return window_width_; }
    int GetWindowHeight() const { return window_height_; }
    
    // Text rendering methods
    void RenderText(const std::string& text, int x, int y, SDL_Color color, int fontSize = 16);
    void RenderWrappedText(const std::string& text, int x, int y, int maxWidth, SDL_Color color, int fontSize = 16);
    TextRenderer* GetTextRenderer() { return text_renderer_.get(); }
    
private:
    SDL_Renderer* renderer_;
    int window_width_;
    int window_height_;
    
    std::unordered_map<std::string, SDL_Texture*> texture_cache_;
    std::unique_ptr<TextRenderer> text_renderer_;
};