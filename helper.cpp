#include <iostream>
#include <cstdlib>
#include <ctime>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include "utility.h"
#include "constants.h"
#include "elements.h"
#include "game_execution.h" 

namespace GoldMiner {
    //Sound loading function
    Mix_Music* Game::LoadMusic(const std::string& filePath) {
        Mix_Music* music = Mix_LoadMUS(filePath.c_str());
        if (!music) {
            std::cerr << "Mix_LoadMUS(" << filePath << "): " << Mix_GetError() << std::endl;
        }
        return music;
    }

    Mix_Chunk* Game::LoadSoundEffect(const std::string& filePath) {
        Mix_Chunk* chunk = Mix_LoadWAV(filePath.c_str());
        if (!chunk) {
            std::cerr << "Mix_LoadWAV(" << filePath << "): " << Mix_GetError() << std::endl;
        }
        return chunk;
    }

    SDL_Texture* Game::loadTexture(const std::string& filePath) {
        SDL_Surface* surface = IMG_Load(filePath.c_str());
        if (!surface) {
            std::cerr << "Failed to load image: " << filePath << " Error: " << IMG_GetError() << std::endl;
            return nullptr;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
        SDL_FreeSurface(surface);
        if (!texture) {
            std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
        }
        return texture;
    }

    SDL_Texture* Game::loadTextTexture(const std::string& text, SDL_Color color) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(m_font, text.c_str(), color);
        if (!textSurface) {
            std::cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << std::endl;
            return nullptr;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
        SDL_FreeSurface(textSurface);
        if (!texture) {
            std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        }
        return texture;
    }

    TTF_Font* Game::loadFont(const std::string& fontPath, int fontSize) {
        TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
        if (!font) {
            std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        }
        return font;
    }

    TTF_Font* Game::loadFontSmall(const std::string& fontPath, int fontSize) {
        TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
        if (!font) {
            std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        }
        return font;
    }
}