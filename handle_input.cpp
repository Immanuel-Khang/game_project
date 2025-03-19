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
    void Game::handleEvents() {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                m_currentState = GameState::Quit;
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_UP:
                    if (m_currentState == GameState::MainMenu) {
                        m_menuSelection = (m_menuSelection - 1 + 3) % 3; // Cycle up through menu
                    }
                    break;
                case SDLK_DOWN:
                    if (m_currentState == GameState::MainMenu) {
                        m_menuSelection = (m_menuSelection + 1) % 3; // Cycle down through menu
                    }
                    break;
                case SDLK_SPACE:
                case SDLK_RETURN:
                    switch (m_currentState) {
                    case GameState::MainMenu:
                        if (m_menuSelection == 0) {
                            m_currentState = GameState::Game;
                            if (!init_elements()) {
                                std::cerr << "Failed to initialize elements!" << std::endl;
                                m_currentState = GameState::MainMenu;
                            }
                            m_showEndMessage = false;
                            m_gameOver = false; // Reset the gameOver flag when starting a new game
                        }
                        else if (m_menuSelection == 1) {
                            m_currentState = GameState::Introduction;
                        }
                        else if (m_menuSelection == 2) {
                            m_currentState = GameState::Quit;
                        }
                        break;
                    case GameState::Game:
                        if (!m_hook.is_active) {
                            // Stop swinging and launch at the current angle
                            m_hook.is_active = true;
                            m_hook.is_returning = false;
                            m_hook.attachedGoldIndex = -1; // Reset the attached gold
                            for (auto& gold : m_gold) {
                                gold.attachedToHook = false;
                            }
                        }
                        break;
                    case GameState::Introduction:
                        m_currentState = GameState::MainMenu; // Return to main menu on any key
                        break;
                    default:
                        break;
                    }
                    break;
                }
            }
        }
    }
    
    void Game::run() {
        if (!init() && !init_elements()) {
            std::cerr << "Failed to initialize!" << std::endl;
            return;
        }

        // Main game loop
        while (m_currentState != GameState::Quit) {
            handleEvents();
            update();
            render();
            SDL_Delay(16);
        }

        close();
    }
}