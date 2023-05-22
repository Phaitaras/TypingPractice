#ifndef game_HPP
#define game_HPP

#include <raylib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <assert.h>

enum GameState{titleScreen, TypingTrials, TickingTimeBomb, endScreen};

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
