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

std::vector<std::string> jsonVec(ns::wordpool ns, std::string filename);

class TextBox{
    public:
    TextBox(int x, int y, char letter, Color color);
    char letter;
    bool matchInput();
};

class Screen{
public:
    Screen(): width(1280), height(720) {}
    Screen(int w, int h);
    virtual void draw() = 0;
protected:
    int width, height;
};

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
    void draw();
    std::string getRandomWord(std::vector<std::string> wordPool);
    void setCurrentWord(std::string w) { currentWord = w; }
    void setNextWord(std::string w) {nextWord = w; }
    bool typedLetter(char w);
    void typedWord() {
        ++wordTyped;
        wpm = (int) wordTyped * 3600 / (3600 - frames);
        typingIndex = 0;
        setCurrentWord(nextWord);
        setNextWord(getRandomWord(wordPool));
    }
    void framesCount() { --frames; }
    int getFrames() {return frames; }
    std::string getCurrentWord() {return currentWord; }
    std::string getNextWord() {return nextWord; }
    char getCurrentLetter() { return currentWord[typingIndex]; }
    int getTypingIndex() {return typingIndex; }
    int getWordsTyped() { return wordTyped; }
    void addTypingIndex() { ++typingIndex; }
    void addWordTyped() { ++wordTyped; }
    void addLettersTyped() { ++lettersTyped; }

protected:
    int typingIndex;
    int wpm;
    int wordTyped;
    int lettersTyped;
    int incorrectLetters;
    int frames;
    std::string currentWord;
    std::string nextWord;
    std::vector<std::string> wordPool;
};

class Game{
public:
    Game(int width, int height, std::string gameTitle);
    Game(const Game& other) = delete;
    Game& operator=(const Game& other) = delete;
    ~Game() noexcept;
    bool GameShouldClose() const;
    void Tick(std::vector<MainScreen> mains, std::vector<GameScreen> modes);
    void DrawWordOnScreen(std::string random_word, int typing_index);
protected:
    int width, height;
    GameState gameState;
    std::vector<std::string> word_pool; // our 1000 word json
};

//class score

#endif
