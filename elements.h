#pragma once
#include <SDL.h>
#include <vector>
#ifndef ELEMEMENTS_H
#define ELEMEMENTS_H

namespace GoldMiner {
    // Structures
    struct Player {
        int x;
        int y;
        SDL_Texture* texture;
    };

    struct Gold {
        int x;
        int y;
        bool collected;
        int size; // Add size to the gold structure
        SDL_Rect rect; // boundary of a gold piece
        bool attachedToHook; // Flag to indicate if the gold is being pulled by the hook
        int value;
    };

    struct Hook {
        int x;
        int y;
        int length;
        double angle;
        bool is_active;
        bool is_returning;
        int attachedGoldIndex; // Index of the gold piece attached to the hook (-1 if none)
        SDL_Texture* hookTexture;
        SDL_Texture* ropeTexture; //New rope Texture
    };
} // namespace GoldMiner

#endif // !ELEMEMENTS_H