#include <iostream>
#include <cstdlib>
#include <ctime>
#include <SDL_image.h>
#include <sstream> 

#include "utility.h"
#include "constants.h"
#include "elements.h"
#include "game_execution.h"

namespace GoldMiner {
    Game::Game(const char* title, int width, int height) : 
        m_window(nullptr),
        m_renderer(nullptr),
        m_background(nullptr),
        m_goldTexture(nullptr),
        m_hookTexture(nullptr),
        m_ropeTexture(nullptr),
        m_scoreTexture(nullptr),
        m_font(nullptr),
        m_timerTexture(nullptr),
        m_startTextTexture(nullptr),
        m_introductionTextTexture(nullptr),
        m_quitTextTexture(nullptr),
        m_congratsTexture(nullptr),
        m_sorryTexture(nullptr),
        m_score(0),
        m_timerSeconds(15),
        m_timerStartTicks(0),
        m_width(width),
        m_height(height),
        m_title(title),
        m_menuSelection(0),
        m_currentState(GameState::MainMenu),
        swingingRight(true),
        m_endMessageTimer(0),
        m_showEndMessage(false), // Initialize the showEndMessage to false
        m_gameOver(false),
        swingSpeedMultiplier(1.5f) // Initialize swingSpeedMultiplier to 1
    
    {
        srand(static_cast<unsigned int>(time(NULL)));
    }


    Game::~Game() {
        close();
    }

    bool Game::init() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
            return false;
        }
        m_window = SDL_CreateWindow(m_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_width, m_height, SDL_WINDOW_SHOWN);
        if (!m_window) {
            std::cerr << "SDL window creation failed: " << SDL_GetError() << std::endl;
            return false;
        }
        m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED); // Read gWindow into gRenderer
        if (!m_renderer) {
            std::cerr << "SDL renderer creation failed: " << SDL_GetError() << std::endl;
            return false;
        }
        SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

        // Initialize SDL_ttf
        if (TTF_Init() == -1) {
            std::cerr << "SDL_ttf initialization failed: " << TTF_GetError() << std::endl;
            return false;
        }

        //Load font
        m_font = loadFont("assets/arial.ttf", 28);

        if (!m_font) return false;

        SDL_Color textColor = { 193, 154, 107 };

        m_startTextTexture = loadTextTexture("Start Game", textColor);
        if (!m_startTextTexture) return false;
        m_introductionTextTexture = loadTextTexture("Introduction", textColor);
        if (!m_introductionTextTexture) return false;
        m_quitTextTexture = loadTextTexture("Quit", textColor);
        if (!m_quitTextTexture) return false;

        // Initialize SDL_mixer
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) return false;
        
        return true;
    }

    bool Game::init_elements() {
        m_background = loadTexture("assets/background.png");
        if (!m_background) return false;
        m_player.texture = loadTexture("assets/digger.png");
        if (!m_player.texture) return false;
        m_goldTexture = loadTexture("assets/Gold.png");
        if (!m_goldTexture) return false;
        m_hook.hookTexture = loadTexture("assets/simple-hook.png");
        if (!m_hook.hookTexture) return false;
        m_hook.ropeTexture = loadTexture("assets/rope.png");
        if (!m_hook.ropeTexture) return false;
        m_backgroundMusic = LoadMusic("assets/background_music.mp3");
        if (!m_backgroundMusic) return false;
        m_goldCollisionSound = LoadSoundEffect("assets/gold_collision.wav");
        if (!m_goldCollisionSound) return false;
        m_tickSound = LoadSoundEffect("assets/tick_sound.wav");
        if (!m_tickSound) return false;

        Mix_VolumeChunk(m_goldCollisionSound, MIX_MAX_VOLUME / 3);
        Mix_VolumeChunk(m_tickSound, MIX_MAX_VOLUME / 4);
        
        m_player.x = m_width / 2 - PLAYER_WIDTH / 2;
        m_player.y = 68;

        m_gold.clear();
        m_gold.resize(NUM_GOLD);
        for (int i = 0; i < NUM_GOLD; ++i) {
            m_gold[i].x = rand() % (m_width - 60) + 20; // Ensure gold doesn't spawn out of bounds
            m_gold[i].y = m_height / 2 + rand() % (m_height / 2 - 60) + 20; //Ensure gold doesn't spawn out of bounds, min size is 20, max size is 80
            m_gold[i].collected = false;
            m_gold[i].size = rand() % 61 + 20; // Random size between 20 and 80
            m_gold[i].rect = { m_gold[i].x, m_gold[i].y, m_gold[i].size, m_gold[i].size };
            m_gold[i].attachedToHook = false;
            m_gold[i].value = m_gold[i].size;
        }

        m_hook.x = m_player.x + PLAYER_WIDTH / 2;
        m_hook.y = m_player.y + PLAYER_HEIGHT; //Position hook under the player
        m_hook.length = 0;
        m_hook.angle = M_PI / 2;
        m_hook.is_active = false;
        m_hook.is_returning = false;
        m_hook.attachedGoldIndex = -1; // Initially, no gold is attached

        m_score = 0;
        m_timerSeconds = 15;
        updateScoreTexture();
        updateTimerTexture();
        m_timerStartTicks = SDL_GetTicks();

        Mix_PlayMusic(m_backgroundMusic, -1); // -1 = loop forever
        return true;
    }

    void Game::close() {
        SDL_DestroyTexture(m_player.texture);
        SDL_DestroyTexture(m_goldTexture);
        SDL_DestroyTexture(m_hook.hookTexture);
        SDL_DestroyTexture(m_ropeTexture);
        SDL_DestroyTexture(m_background);
        SDL_DestroyTexture(m_scoreTexture);
        SDL_DestroyTexture(m_timerTexture);
        SDL_DestroyTexture(m_startTextTexture);
        SDL_DestroyTexture(m_introductionTextTexture);
        SDL_DestroyTexture(m_congratsTexture);
        SDL_DestroyTexture(m_sorryTexture);

        if (m_font) TTF_CloseFont(m_font);
        if (m_renderer) SDL_DestroyRenderer(m_renderer);
        if (m_window) SDL_DestroyWindow(m_window);

        if (m_backgroundMusic) Mix_FreeMusic(m_backgroundMusic);
        if (m_goldCollisionSound) Mix_FreeChunk(m_goldCollisionSound);
        if (m_tickSound) Mix_FreeChunk(m_tickSound);
        
        Mix_Quit();
        TTF_Quit();
        SDL_Quit();
    }
}