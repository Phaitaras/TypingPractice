#ifndef game_HPP
#define game_HPP

#include <raylib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <assert.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum GameState{titleScreen, TypingTrials, TickingTimeBomb, endScreen};

namespace ns {
    struct wordpool {
        std::string language;
        std::vector<std::string> words;
    };
}

std::vector<std::string> json_vec(std::string filename);

class TextBox{
    public:
    TextBox(int x, int y, char letter, Color color);
    char letter;
    bool matchInput();
};

class Game{
public:
    Game(int width, int height, std::string gameTitle);
    Game(const Game& other) = delete;
    Game& operator=(const Game& other) = delete;
    ~Game() noexcept;
    bool GameShouldClose() const;
    void Tick();
    int width, height;
    GameState gameState;
    std::vector<std::string> word_pool; // our 1000 word json
    void DrawWordOnScreen(std::string random_word, int typing_index);
};

#endif
