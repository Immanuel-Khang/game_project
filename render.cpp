#include <iostream>
#include <cstdlib>
#include <ctime>
#include <SDL_image.h>

#include "utility.h"
#include "constants.h"
#include "elements.h"
#include "game_execution.h" 

namespace GoldMiner {
    void Game::render() {
        SDL_RenderClear(m_renderer);
        switch (m_currentState) {
        case GameState::MainMenu:
        {
            SDL_Color selectedColor = { 255, 255, 0 };
            SDL_Color normalColor = { 193, 154, 107 };

            SDL_Texture* startTexture = m_startTextTexture;
            SDL_Texture* introTexture = m_introductionTextTexture;
            SDL_Texture* quitTexture = m_quitTextTexture;

            SDL_Color startColor = normalColor;
            SDL_Color introColor = normalColor;
            SDL_Color quitColor = normalColor;

            if (m_menuSelection == 0) startColor = selectedColor;
            else if (m_menuSelection == 1) introColor = selectedColor;
            else if (m_menuSelection == 2) quitColor = selectedColor;

            SDL_Texture* startText = loadTextTexture("Start Game", startColor);
            SDL_Texture* introText = loadTextTexture("Introduction", introColor);
            SDL_Texture* quitText = loadTextTexture("Quit", quitColor);

            int startY = m_height / 4;
            int introY = m_height / 2;
            int quitY = m_height * 3 / 4;

            SDL_Rect startRect;
            SDL_QueryTexture(startText, NULL, NULL, &startRect.w, &startRect.h);
            startRect.x = m_width / 2 - startRect.w / 2;
            startRect.y = startY - startRect.h / 2;
            SDL_RenderCopy(m_renderer, startText, NULL, &startRect);
            SDL_DestroyTexture(startText);

            SDL_Rect introRect;
            SDL_QueryTexture(introText, NULL, NULL, &introRect.w, &introRect.h);
            introRect.x = m_width / 2 - introRect.w / 2;
            introRect.y = introY - introRect.h / 2;
            SDL_RenderCopy(m_renderer, introText, NULL, &introRect);
            SDL_DestroyTexture(introText);

            SDL_Rect quitRect;
            SDL_QueryTexture(quitText, NULL, NULL, &quitRect.w, &quitRect.h);
            quitRect.x = m_width / 2 - quitRect.w / 2;
            quitRect.y = quitY - quitRect.h / 2;
            SDL_RenderCopy(m_renderer, quitText, NULL, &quitRect);
            SDL_DestroyTexture(quitText);

            break;
        }
        case GameState::Game:
        {
            if (!m_showEndMessage) // Only show this when game isn't over yet.
            {
                SDL_Rect background_rec = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
                SDL_RenderCopy(m_renderer, m_background, NULL, &background_rec);

                SDL_Rect playerRect = { m_player.x, m_player.y, PLAYER_WIDTH, PLAYER_HEIGHT };
                SDL_RenderCopy(m_renderer, m_player.texture, NULL, &playerRect);

                // Render golds that are not collected
                for (int i = 0; i < m_gold.size(); i++) {
                    if (!m_gold[i].collected) {
                        SDL_RenderCopy(m_renderer, m_goldTexture, NULL, &m_gold[i].rect);
                    }
                }

                // Draw the hook
                if (!m_hook.is_active) {
                    // Draw swinging hook
                    SDL_Rect hookRect;
                    hookRect.w = HOOK_IMAGE_WIDTH;
                    hookRect.h = HOOK_IMAGE_HEIGHT;

                    // Calculate hook position based on swing angle
                    hookRect.x = m_hook.x - HOOK_IMAGE_WIDTH / 2;
                    hookRect.y = m_hook.y;

                    //Rotation point
                    SDL_Point center;
                    center.x = HOOK_IMAGE_WIDTH / 2;
                    center.y = 0;

                    SDL_RenderCopyEx(m_renderer, m_hook.hookTexture, NULL, &hookRect, m_hook.angle * 180 / M_PI - 90, &center, SDL_FLIP_NONE);
                }
                else {
                    //Draw moving hook and rope

                    int hook_end_x = m_hook.x + (int)(m_hook.length * cos(m_hook.angle));
                    int hook_end_y = m_hook.y + (int)(m_hook.length * sin(m_hook.angle));

                    for (int i = 0; i < m_hook.length; i += ROPE_SEGMENT_HEIGHT) {
                        SDL_Rect ropeRect;
                        ropeRect.x = m_hook.x + (int)(i * cos(m_hook.angle)) - ROPE_SEGMENT_WIDTH / 2; // Adjust for rope width
                        ropeRect.y = m_hook.y + (int)(i * sin(m_hook.angle)) - 4; // Adjust for rope width
                        ropeRect.w = ROPE_SEGMENT_WIDTH; // Rope width
                        ropeRect.h = ROPE_SEGMENT_HEIGHT;
                        SDL_RenderCopyEx(m_renderer, m_hook.ropeTexture, NULL, &ropeRect, m_hook.angle * 180 / M_PI - 90, NULL, SDL_FLIP_NONE); //No rotation required for rope
                    }

                    //Draw the hook at the end of the rope
                    SDL_Rect hookRect;
                    hookRect.w = HOOK_IMAGE_WIDTH;
                    hookRect.h = HOOK_IMAGE_HEIGHT;

                    // Calculate the top-center of the hook
                    hookRect.x = hook_end_x - HOOK_IMAGE_WIDTH / 2; //Center the x position
                    hookRect.y = hook_end_y;   //Top aligned Y coordinate

                    SDL_Point center;
                    center.x = HOOK_IMAGE_WIDTH / 2;
                    center.y = 0;

                    SDL_RenderCopyEx(m_renderer, m_hook.hookTexture, NULL, &hookRect, m_hook.angle * 180 / M_PI - 90, &center, SDL_FLIP_NONE);
                }

                // Render Score to Top - Left
                SDL_Rect scoreRect;
                scoreRect.x = 10;
                scoreRect.y = 10;
                if (m_scoreTexture) {
                    SDL_QueryTexture(m_scoreTexture, NULL, NULL, &scoreRect.w, &scoreRect.h); //Get the width and height of the texture.
                    SDL_RenderCopy(m_renderer, m_scoreTexture, NULL, &scoreRect);
                }
                
                //Render Timer to Top-Right
                SDL_Rect timerRect;
                timerRect.x = SCREEN_WIDTH - 10 - 100; // Adjust as needed
                timerRect.y = 10;
                if (m_timerTexture) {
                    SDL_QueryTexture(m_timerTexture, NULL, NULL, &timerRect.w, &timerRect.h);
                    SDL_RenderCopy(m_renderer, m_timerTexture, NULL, &timerRect);
                }
            }
            else // Game is over, render the end message in GameState::Game
            {
                SDL_SetRenderDrawColor(m_renderer, 210, 180, 140, 255);
                SDL_RenderClear(m_renderer);

                // Calculate the center and a vertical offset
                int centerX = m_width / 2;
                int centerY = m_height / 2;
                int verticalOffset = 20; // Adjust as needed for spacing between lines

                if (m_congratsTexture != nullptr) {
                    SDL_Rect congratsRect;
                    SDL_QueryTexture(m_congratsTexture, NULL, NULL, &congratsRect.w, &congratsRect.h);
                    congratsRect.x = centerX - congratsRect.w / 2;
                    congratsRect.y = centerY - congratsRect.h / 2 - verticalOffset; // Shifted up
                    SDL_RenderCopy(m_renderer, m_congratsTexture, NULL, &congratsRect);

                    if (m_highestScoreTexture != nullptr) {
                        SDL_Rect highestScoreRect;
                        SDL_QueryTexture(m_highestScoreTexture, NULL, NULL, &highestScoreRect.w, &highestScoreRect.h);
                        highestScoreRect.x = centerX - highestScoreRect.w / 2;
                        highestScoreRect.y = centerY + congratsRect.h / 2 + verticalOffset; // Shifted down, below congrats
                        SDL_RenderCopy(m_renderer, m_highestScoreTexture, NULL, &highestScoreRect);
                    }
                }

                if (m_sorryTexture != nullptr) {
                    SDL_Rect sorryRect;
                    SDL_QueryTexture(m_sorryTexture, NULL, NULL, &sorryRect.w, &sorryRect.h);
                    sorryRect.x = centerX - sorryRect.w / 2;
                    sorryRect.y = centerY - sorryRect.h / 2 - verticalOffset;  // Shifted up
                    SDL_RenderCopy(m_renderer, m_sorryTexture, NULL, &sorryRect);

                    if (m_highestScoreTexture != nullptr) {
                        SDL_Rect highestScoreRect;
                        SDL_QueryTexture(m_highestScoreTexture, NULL, NULL, &highestScoreRect.w, &highestScoreRect.h);
                        highestScoreRect.x = centerX - highestScoreRect.w / 2;
                        highestScoreRect.y = centerY + sorryRect.h / 2 + verticalOffset;  // Shifted down, below sorry message
                        SDL_RenderCopy(m_renderer, m_highestScoreTexture, NULL, &highestScoreRect);
                    }
                }
            }
            break;
        }
        case GameState::Introduction:
        {
            SDL_Color textColor = { 193, 154, 107 };
            // Load a smaller font specifically for the introduction text
            TTF_Font* smallFont = loadFontSmall("assets/arial.ttf", 18); // Smaller font size
            if (!smallFont) {
                std::cerr << "Failed to load small font for introduction!" << std::endl;
                break; // Exit this case if the small font fails to load
            }

            std::string line1 = "Use the spacebar to launch the hook and collect gold.";
            std::string line2 = "Collect all the gold before the time runs out!";

            // Create the textures with the smaller font
            SDL_Surface* text1Surface = TTF_RenderText_Solid(smallFont, line1.c_str(), textColor);
            SDL_Texture* text1Texture = SDL_CreateTextureFromSurface(m_renderer, text1Surface);
            SDL_FreeSurface(text1Surface);

            SDL_Surface* text2Surface = TTF_RenderText_Solid(smallFont, line2.c_str(), textColor);
            SDL_Texture* text2Texture = SDL_CreateTextureFromSurface(m_renderer, text2Surface);
            SDL_FreeSurface(text2Surface);

            if (text1Texture && text2Texture) {
                SDL_Rect text1Rect;
                SDL_QueryTexture(text1Texture, NULL, NULL, &text1Rect.w, &text1Rect.h);
                text1Rect.x = m_width / 2 - text1Rect.w / 2;
                text1Rect.y = m_height / 2 - text1Rect.h; 

                SDL_RenderCopy(m_renderer, text1Texture, NULL, &text1Rect);

                SDL_Rect text2Rect;
                SDL_QueryTexture(text2Texture, NULL, NULL, &text2Rect.w, &text2Rect.h);
                text2Rect.x = m_width / 2 - text2Rect.w / 2;
                text2Rect.y = m_height / 2 + 10; //Position the second line 10 pixels below the center
                SDL_RenderCopy(m_renderer, text2Texture, NULL, &text2Rect);

                SDL_DestroyTexture(text1Texture);
                SDL_DestroyTexture(text2Texture);
            }
            // Destroy the small font when done
            TTF_CloseFont(smallFont);
            break;
        }
        }
    
        SDL_RenderPresent(m_renderer);
    }
}
