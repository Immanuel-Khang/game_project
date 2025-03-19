#include <iostream>
#include <cstdlib>
#include <ctime>
#include <SDL_image.h>
#include <sstream> // Required for stringstreams

#include "utility.h"
#include "constants.h"
#include "elements.h"
#include "game_execution.h" 

namespace GoldMiner {
    void Game::updateScoreTexture() {
        std::stringstream ss;
        ss << "Score: " << m_score;
        SDL_Color textColor = { 193, 154, 107 };
        SDL_Texture* newTexture = loadTextTexture(ss.str(), textColor);

        if (newTexture) {
            if (m_scoreTexture) SDL_DestroyTexture(m_scoreTexture);
            m_scoreTexture = newTexture;
        }
    }

    void Game::updateTimerTexture() {
        std::stringstream ss;
        ss << "Time: " << m_timerSeconds;
        SDL_Color textColor;
        if (m_timerSeconds <= 10) {
            textColor = { 255, 0, 0 }; // Red if time is low
        }
        else {
            textColor = { 193, 154, 107 };
        }
        SDL_Texture* newTexture = loadTextTexture(ss.str(), textColor);
        if (newTexture) {
            if (m_timerTexture) SDL_DestroyTexture(m_timerTexture);
            m_timerTexture = newTexture;
        }
    }

    void Game::update() {
        switch (m_currentState) {
        case GameState::Game:
            // **Swing Hook Left and Right Before Launch**
            if (!m_hook.is_active) {
                swingSpeedMultiplier += 0.001;
                if (swingingRight) {
                    m_hook.angle += ANGLE_SPEED * swingSpeedMultiplier; // Increase angle
                    if (m_hook.angle >= M_PI) { // Max right swing (~162°)
                        swingingRight = false; // Change direction
                    }
                }
                else {
                    m_hook.angle -= ANGLE_SPEED * swingSpeedMultiplier; // Decrease angle
                    if (m_hook.angle <= 0) { // Max left swing (~18°)
                        swingingRight = true; // Change direction
                    }
                }
            }

            // Determine the end condition and load the appropriate message only once
            if ((m_gold.size() == 0 || m_timerSeconds <= 0) && !m_showEndMessage) {
                m_showEndMessage = true; // Set the flag
                m_gameOver = true;

                // Load congratulatory or sorry message based on the condition
                SDL_Color color = { 178, 104, 200 };
                if (m_gold.size() == 0) {
                    std::string congrats = "Congratulations, you have finished the game!";
                    m_congratsTexture = loadTextTexture(congrats, color);
                }
                else {
                    std::string sorry = "Sorry, you ran out of time!";
                    m_sorryTexture = loadTextTexture(sorry, color);
                }
                m_endMessageTimer = SDL_GetTicks(); // Start the timer
            }

            //Check if there is an end message active, count 5 seconds, switch to main menu.
            if (m_showEndMessage) {
                Uint32 currentTime = SDL_GetTicks();
                if (currentTime - m_endMessageTimer >= 5000) { // 5 seconds
                    m_currentState = GameState::MainMenu; //Back to Main Menu!
                    m_showEndMessage = false;

                    //Clean up message at return
                    if (m_congratsTexture != nullptr) {
                        SDL_DestroyTexture(m_congratsTexture);
                        m_congratsTexture = nullptr;
                    }
                    if (m_sorryTexture != nullptr) {
                        SDL_DestroyTexture(m_sorryTexture);
                        m_sorryTexture = nullptr;
                    }
                }
            }

            if (!m_showEndMessage) {
                //Update time, but don't count timer when game is over.
                Uint32 currentTime = SDL_GetTicks();
                if (currentTime - m_timerStartTicks >= 1000) { //Check if one second has passed
                    m_timerSeconds--; //Decrement Timer
                    updateTimerTexture(); //Update time
                    m_timerStartTicks = currentTime;
                    // Play tick sound
                    if (m_tickSound) {
                        Mix_PlayChannel(-1, m_tickSound, 0);
                    }
                }

                // **Move Hook When Released**
                handleHookMovement(m_hook, m_gold, HOOK_LENGTH_INCREMENT, MAX_HOOK_LENGTH, HOOK_IMAGE_WIDTH, 
                    HOOK_IMAGE_HEIGHT, [this](int score) {setScore(score);}, *this);

                // **Move the Gold**
                handleGoldMovement(m_hook, m_gold);

                break;
            }
        }
    }
}