#pragma once
#ifndef GAME_EXECUTION_H
#define GAME_EXECUTION_H
#include "elements.h" // Include to use the GoldMiner namespace and structures
#include <functional>

namespace GoldMiner {

    // Forward declarations: Important to avoid circular dependencies if you are passing the player in
    struct Player;
    struct Hook;
    struct Gold;
    class Game;

    //Check collisions between hook and gold
    bool checkCollision(const Hook& hook, const std::vector<Gold>& golds, int HOOK_IMAGE_WIDTH, int HOOK_IMAGE_HEIGHT);

    void handleHookMovement(Hook& hook, std::vector<Gold>& golds, int HOOK_LENGTH_INCREMENT, int MAX_HOOK_LENGTH, int HOOK_IMAGE_WIDTH, int HOOK_IMAGE_HEIGHT, std::function<void(int)> updateScore, Game& game);

    void handleGoldMovement(Hook& hook, std::vector<Gold>& golds);
} // namespace GoldMiner

#endif // !GAME_EXECUTION_H