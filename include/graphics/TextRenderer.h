#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <memory>
#include <vector>
#include "Renderer.h"

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();
    
    bool Initialize(SDL_Renderer* renderer);
    void Shutdown();
    
    // Basic text rendering
    void RenderText(const std::string& text, int x, int y, SDL_Color color, int fontSize = 16);
    void RenderText(const std::string& text, int x, int y, SDL_Color color, TTF_Font* font);
    
    // Text with background box
    void RenderTextBox(const std::string& text, int x, int y, int width, int height, 
                      SDL_Color textColor, SDL_Color bgColor, int fontSize = 16);
    
    // Multi-line text with wrapping
    void RenderWrappedText(const std::string& text, int x, int y, int maxWidth, 
                          SDL_Color color, int fontSize = 16, int lineSpacing = 4);
    
    // Get text dimensions
    void GetTextSize(const std::string& text, int fontSize, int* width, int* height);
    std::vector<std::string> WrapText(const std::string& text, int maxWidth, int fontSize);
    
    // Font management
    TTF_Font* LoadFont(const std::string& fontPath, int fontSize);
    TTF_Font* GetDefaultFont(int fontSize);
    
private:
    SDL_Renderer* renderer_;
    TTF_Font* defaultFont16_;
    TTF_Font* defaultFont20_;
    TTF_Font* defaultFont24_;
    
    SDL_Texture* CreateTextTexture(const std::string& text, SDL_Color color, TTF_Font* font, int* width, int* height);
};