#include "game_execution.h"
#include "utility.h"
#include <cmath>

namespace GoldMiner {

    bool checkCollision(const Hook& hook, const std::vector<Gold>& golds, int HOOK_IMAGE_WIDTH, int HOOK_IMAGE_HEIGHT) {
        int hook_end_x = hook.x + (int)(hook.length * cos(hook.angle));
        int hook_end_y = hook.y + (int)(hook.length * sin(hook.angle));

        // Create hook rectangle from center to test for better collision logic.
        SDL_Rect hookRect = { hook_end_x - HOOK_IMAGE_WIDTH / 2, hook_end_y, HOOK_IMAGE_WIDTH, HOOK_IMAGE_HEIGHT };

        for (size_t i = 0; i < golds.size(); ++i) {
            if (!golds[i].collected && !golds[i].attachedToHook) {
                // Check if the rectangles intersect
                if (SDL_HasIntersection(&hookRect, &golds[i].rect)) {
                    return true; // Collision detected
                }
            }
        }
        return false; // No collision
    }

    void handleHookMovement(Hook& hook, std::vector<Gold>& golds, int HOOK_LENGTH_INCREMENT, int MAX_HOOK_LENGTH, int HOOK_IMAGE_WIDTH, int HOOK_IMAGE_HEIGHT, std::function<void(int)> updateScore, Game& game) {
        if (hook.is_active) {
            if (!hook.is_returning) {
                hook.length += HOOK_LENGTH_INCREMENT; // Extend the hook

                if (checkCollision(hook, golds, HOOK_IMAGE_WIDTH, HOOK_IMAGE_HEIGHT)) {
                    // Collision with gold detected
                    hook.is_returning = true;
                    // Find the index of the collided gold
                    Mix_PlayChannel(-1, game.m_goldCollisionSound, 0);
                    for (size_t i = 0; i < golds.size(); ++i) {
                        int hook_end_x = hook.x + (int)(hook.length * cos(hook.angle));
                        int hook_end_y = hook.y + (int)(hook.length * sin(hook.angle));
                        SDL_Rect hookRect = { hook_end_x - HOOK_IMAGE_WIDTH / 2, hook_end_y, HOOK_IMAGE_WIDTH, HOOK_IMAGE_HEIGHT };

                        if (!golds[i].collected && !golds[i].attachedToHook && SDL_HasIntersection(&hookRect, &golds[i].rect)) {
                            hook.attachedGoldIndex = (int)i; // store the index of the attached gold.
                            golds[i].attachedToHook = true;
                            break;
                        }
                    }
                }
                else if (hook.length > MAX_HOOK_LENGTH) { // hook goes out of bounds without collision
                    hook.is_returning = true;
                }
            }
            else {
                hook.length -= HOOK_LENGTH_INCREMENT;
                if (hook.length <= 0) {
                    hook.is_active = false; // Retracted fully
                    hook.is_returning = false;
                    hook.length = 0;

                    // Detach gold when fully retracted
                    if (hook.attachedGoldIndex != -1) {
                        //Add the Gold Value
                        game.setScore(game.getScore() + golds[hook.attachedGoldIndex].value); //update the score through the game class.
                        golds.erase(golds.begin() + hook.attachedGoldIndex);
                        hook.attachedGoldIndex = -1;
                    }
                }
            }
        }
    }

    void handleGoldMovement(Hook& hook, std::vector<Gold>& golds) {
        if (hook.is_active && hook.is_returning && hook.attachedGoldIndex != -1) {
            // Update gold position to follow the hook during retraction, positioning it at the *center* of the hook image
            int hook_end_x = hook.x + (int)(hook.length * cos(hook.angle));
            int hook_end_y = hook.y + (int)(hook.length * sin(hook.angle));
            // Position the gold to the *center* of the hook image at the end of the line
            golds[hook.attachedGoldIndex].x = hook_end_x - golds[hook.attachedGoldIndex].size / 2;
            golds[hook.attachedGoldIndex].y = hook_end_y - golds[hook.attachedGoldIndex].size / 2;

            golds[hook.attachedGoldIndex].rect.x = golds[hook.attachedGoldIndex].x; // update the rec so that is is repeatedly rendered
            golds[hook.attachedGoldIndex].rect.y = golds[hook.attachedGoldIndex].y;
        }
    }
}