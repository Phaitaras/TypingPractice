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

enum GameState{titleScreen, mode1, mode2, endScreen, skipped};

namespace ns {
    struct wordpool {
        std::string language;
        std::vector<std::string> words;
    };
}

std::vector<std::string> jsonVec(ns::wordpool ns, std::string filename);

class TextBox{
public:
    TextBox(int x, int y, char letter, Color color);
    char letter;
};

class Screen{
public:
    Screen(): width(1280), height(720) {}
    Screen(int w, int h);
    virtual void draw() = 0;
protected:
    int width, height;
};

//screen for displaying messages or info
class MainScreen: public Screen{
public:
    MainScreen(): Screen() { msg = ""; }
    MainScreen(std::string t): Screen() { msg = t; }
    void draw();
protected:
    std::string msg;

};

class GameScreen: public Screen{
public:
    GameScreen();
    GameScreen(int index, int speed, int typed, int f, int w, int h);
    GameScreen(int index, int speed, int typed, int f);
    std::string getRandomWord(std::vector<std::string> wordPool);
    //random current/next?
    void setCurrentWord(std::string w) { currentWord = w; }
    void setNextWord(std::string w) { nextWord = w; }
    bool typedLetter(char w);
    void typedWord();
    bool timesUp();
    int getFrames() { return frames; }
    std::string getCurrentWord() { return currentWord; }
    std::string getNextWord() { return nextWord; }
    int getTypingIndex() { return typingIndex; }
    int getWordsTyped() { return wordTyped; }
    void addTypingIndex() { ++typingIndex; }
    void addWordTyped() { ++wordTyped; }
    void addLettersTyped() { ++lettersTyped; }
    void DrawWordOnScreen(std::string random_word, int typing_index);
    virtual void draw() = 0;
    virtual void framesCount() = 0;
    virtual void update(char key) = 0;
    virtual void drawScore() = 0;
protected:
    int typingIndex;
    int wpm;
    int incorrectLetters;
    int wordTyped;
    int lettersTyped;
    int frames;
    std::string currentWord;
    std::string nextWord;
    std::vector<std::string> wordPool;
};



class TypingTrials: public GameScreen{
public:
    TypingTrials();
    void draw();
    void framesCount() { --frames; }
    void update(char key);
    void drawScore();
};

class Game{
public:
    Game(int width, int height, std::string gameTitle);
    Game(const Game& other) = delete;
    Game& operator=(const Game& other) = delete;
    ~Game() noexcept;
    bool GameShouldClose() const;
    void Tick(std::vector<MainScreen>& mains, std::vector<GameScreen*>& modes);
protected:
    int width, height;
    GameState gameState;
    std::vector<std::string> word_pool; // our 1000 word json
};

//class score

#endif
