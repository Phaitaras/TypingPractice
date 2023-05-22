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

class GameScreen{
public:
    GameScreen() {}
    GameScreen(std::string Title);
    GameScreen(const GameScreen& other) = delete;
    GameScreen& operator=(const GameScreen& other) = delete;
    ~GameScreen() noexcept;
    bool GameScreenShouldClose() const;
    virtual void Tick();
    void DrawWordOnScreen(std::string random_word, int typing_index);
    std::string GetRandomWord();
protected:
    const int width = 1280;
    const int height = 720;
    std::string titlereal;
    std::vector<std::string> word_pool; // our 1000 word json
};

class TypingTrials: public GameScreen{
public:
    TypingTrials(): TypingIndex(0), Wpm(0), WordTyped(0), Frames(3600) {}
    TypingTrials(int Index, int Speed, int Typed, int Frames); 
    void Tick();
protected:
    int TypingIndex;
    int Wpm;
    int WordTyped;
    int Frames;
};

class EndScreen: public GameScreen{
public:
    void Tick();
};

#endif
