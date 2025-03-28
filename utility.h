#pragma once
#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <fstream>
#include <SDL.h>
#include <vector>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "elements.h"

namespace GoldMiner {
    enum class GameState {
        MainMenu,
        Game,
        Introduction,
        Quit
    };

    class Game {
    public:
        Game(const char* title, int width, int height);
        ~Game(); //Destructor
        bool init();
        bool init_elements();

        void updateScoreTexture();
        void updateTimerTexture();

        void handleEvents();
        void update();
        void render();

        void run(); // Main game loop management

        Mix_Music* LoadMusic(const std::string&);
        Mix_Chunk* LoadSoundEffect(const std::string&);
        SDL_Texture* loadTexture(const std::string&); 
        SDL_Texture* loadTextTexture(const std::string& text, SDL_Color color);
        TTF_Font* loadFont(const std::string& fontPath, int fontSize);
        TTF_Font* loadFontSmall(const std::string& fontPath, int fontSize);

        void close(); 
        SDL_Renderer* getRenderer() const { return m_renderer; } 
        int getScore() const { return m_score; }
        void setScore(int score) { m_score = score; updateScoreTexture(); }
        void update_highestScore(int final_Score, const std::string& des); 
        void renderHighestScore(); 
        GameState getCurrentState() const { return m_currentState; }
        void setCurrentState(GameState state) { m_currentState = state; }

        Mix_Music* m_backgroundMusic;
        Mix_Chunk* m_goldCollisionSound;
        Mix_Chunk* m_tickSound;
    private:
        SDL_Window* m_window;
        SDL_Renderer* m_renderer;
        SDL_Texture* m_background;
        Player m_player;
        std::vector<Gold> m_gold;
        SDL_Texture* m_goldTexture;
        Hook m_hook;

        int m_score;
        int m_highestscore; 
        int gold_add; 
        SDL_Texture* m_scoreTexture;
        TTF_Font* m_font;

        SDL_Texture* m_hookTexture; 
        SDL_Texture* m_ropeTexture; 
        SDL_Texture* m_congratsTexture;
        SDL_Texture* m_sorryTexture;
        SDL_Texture* m_highestScoreTexture; 
        Uint32 m_endMessageTimer; //Timer for game end messages.
        bool m_showEndMessage;

        // Sound-related members
        
        int m_timerSeconds;
        Uint32 m_timerStartTicks;
        SDL_Texture* m_timerTexture;
        bool m_gameOver; //Tracks when the game is over, for cleanup and menu state.
        
        //Menu Variables
        SDL_Texture* m_startTextTexture;
        SDL_Texture* m_introductionTextTexture;
        SDL_Texture* m_quitTextTexture;

        int m_menuSelection; // 0: Start, 1: Introduction, 2: Quit
        int m_width;
        int m_height;
        const char* m_title;

        GameState m_currentState; // Current game state
        bool swingingRight; // Hook swing direction

        double swingSpeedMultiplier; // Multiplier to increase swinging speed
    };
}

#endif // !UTILITY_H
