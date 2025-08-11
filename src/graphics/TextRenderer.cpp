#include "TextRenderer.h"
#include <iostream>
#include <sstream>

TextRenderer::TextRenderer() 
    : renderer_(nullptr), defaultFont16_(nullptr), defaultFont20_(nullptr), defaultFont24_(nullptr) {
}

TextRenderer::~TextRenderer() {
    Shutdown();
}

bool TextRenderer::Initialize(SDL_Renderer* renderer) {
    renderer_ = renderer;
    
    if (TTF_Init() != 0) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        return false;
    }
    
    // Try to load a system font (fallback to a simple built-in approach if no system font found)
    // On macOS, try common system fonts
    const char* fontPaths[] = {
        "/System/Library/Fonts/Helvetica.ttc",
        "/System/Library/Fonts/Arial.ttf", 
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",  // Linux
        "C:/Windows/Fonts/arial.ttf",  // Windows
        nullptr
    };
    
    for (const char** path = fontPaths; *path; ++path) {
        defaultFont16_ = TTF_OpenFont(*path, 16);
        if (defaultFont16_) {
            defaultFont20_ = TTF_OpenFont(*path, 20);
            defaultFont24_ = TTF_OpenFont(*path, 24);
            break;
        }
    }
    
    if (!defaultFont16_) {
        std::cerr << "Warning: Could not load system font. TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        std::cerr << "Falling back to built-in rendering..." << std::endl;
        return false;
    }
    
    return true;
}

void TextRenderer::Shutdown() {
    if (defaultFont16_) {
        TTF_CloseFont(defaultFont16_);
        defaultFont16_ = nullptr;
    }
    if (defaultFont20_) {
        TTF_CloseFont(defaultFont20_);
        defaultFont20_ = nullptr;
    }
    if (defaultFont24_) {
        TTF_CloseFont(defaultFont24_);
        defaultFont24_ = nullptr;
    }
    
    TTF_Quit();
}

void TextRenderer::RenderText(const std::string& text, int x, int y, SDL_Color color, int fontSize) {
    TTF_Font* font = GetDefaultFont(fontSize);
    if (font) {
        RenderText(text, x, y, color, font);
    }
}

void TextRenderer::RenderText(const std::string& text, int x, int y, SDL_Color color, TTF_Font* font) {
    if (!renderer_ || !font || text.empty()) return;
    
    int width, height;
    SDL_Texture* textTexture = CreateTextTexture(text, color, font, &width, &height);
    if (!textTexture) return;
    
    SDL_Rect destRect = {x, y, width, height};
    SDL_RenderCopy(renderer_, textTexture, nullptr, &destRect);
    SDL_DestroyTexture(textTexture);
}

void TextRenderer::RenderTextBox(const std::string& text, int x, int y, int width, int height, 
                                SDL_Color textColor, SDL_Color bgColor, int fontSize) {
    if (!renderer_) return;
    
    // Draw background
    SDL_SetRenderDrawColor(renderer_, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_Rect bgRect = {x, y, width, height};
    SDL_RenderFillRect(renderer_, &bgRect);
    
    // Draw text centered in box
    TTF_Font* font = GetDefaultFont(fontSize);
    if (!font) return;
    
    int textWidth, textHeight;
    TTF_SizeText(font, text.c_str(), &textWidth, &textHeight);
    
    int textX = x + (width - textWidth) / 2;
    int textY = y + (height - textHeight) / 2;
    
    RenderText(text, textX, textY, textColor, font);
}

void TextRenderer::RenderWrappedText(const std::string& text, int x, int y, int maxWidth, 
                                    SDL_Color color, int fontSize, int lineSpacing) {
    std::vector<std::string> lines = WrapText(text, maxWidth, fontSize);
    
    int currentY = y;
    for (const auto& line : lines) {
        RenderText(line, x, currentY, color, fontSize);
        currentY += fontSize + lineSpacing;
    }
}

void TextRenderer::GetTextSize(const std::string& text, int fontSize, int* width, int* height) {
    TTF_Font* font = GetDefaultFont(fontSize);
    if (font && TTF_SizeText(font, text.c_str(), width, height) != 0) {
        *width = 0;
        *height = 0;
    }
}

std::vector<std::string> TextRenderer::WrapText(const std::string& text, int maxWidth, int fontSize) {
    std::vector<std::string> lines;
    TTF_Font* font = GetDefaultFont(fontSize);
    if (!font) return lines;
    
    std::istringstream words(text);
    std::string word;
    std::string currentLine;
    
    while (words >> word) {
        std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
        
        int lineWidth;
        TTF_SizeText(font, testLine.c_str(), &lineWidth, nullptr);
        
        if (lineWidth <= maxWidth) {
            currentLine = testLine;
        } else {
            if (!currentLine.empty()) {
                lines.push_back(currentLine);
                currentLine = word;
            } else {
                // Word is too long for line, just add it anyway
                lines.push_back(word);
            }
        }
    }
    
    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }
    
    return lines;
}

TTF_Font* TextRenderer::LoadFont(const std::string& fontPath, int fontSize) {
    return TTF_OpenFont(fontPath.c_str(), fontSize);
}

TTF_Font* TextRenderer::GetDefaultFont(int fontSize) {
    if (fontSize <= 16) return defaultFont16_;
    if (fontSize <= 20) return defaultFont20_;
    return defaultFont24_;
}

SDL_Texture* TextRenderer::CreateTextTexture(const std::string& text, SDL_Color color, TTF_Font* font, int* width, int* height) {
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!textSurface) {
        std::cerr << "TTF_RenderText_Blended Error: " << TTF_GetError() << std::endl;
        return nullptr;
    }
    
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer_, textSurface);
    if (!textTexture) {
        std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
        return nullptr;
    }
    
    *width = textSurface->w;
    *height = textSurface->h;
    
    SDL_FreeSurface(textSurface);
    return textTexture;
}