#ifndef game_HPP
#define game_HPP
#define MAX_INPUT_CHARS     9

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

struct NameScore{
    std::string name;
    std::vector<int> score;
};

std::vector<std::string> jsonVec(ns::wordpool ns, std::string filename);

class TextBox{
public:
    TextBox(int x, int y, char letter, Font font, Color color);
    char letter;
};

class Screen{
public:
    Screen(): width(1280), height(720) {}
    Screen(int w, int h);
    virtual void draw(std::vector<Texture2D> textures, Font font) = 0;
    virtual bool buttonClicked(Rectangle button);
protected:
    int width, height;
    bool buttonPressed;
};

//screen for displaying messages or info
class MainScreen: public Screen{
public:
    MainScreen(): Screen() { msg = ""; }
    MainScreen(std::string t): Screen() { msg = t; }

    void draw(std::vector<Texture2D> textures, Font font);
    Rectangle getButton1() { return buttonMode1; }
    Rectangle getButton2() { return buttonMode2; }
    Rectangle getNameBox() { return nameBox; }
    char* getName() { return name; }

    bool mouseOnText(Rectangle textbox);
    void typingName();
protected:
    std::string msg;
private:
    Rectangle buttonMode1 = {(width/2.0f) - 100, (height/2.0f) - 30, 200, 50};
    Rectangle buttonMode2 = {(width/2.0f) - 100, (height/2.0f) + 30, 200, 50};
    Rectangle practiceMode = {(width/2.0f) - 100, (height/2.0f) + 90, 200, 50};

    Rectangle nameBox = { width/2.0f - 100, (height/2.0f) + 240, 200, 50 };
    bool mouseonText = false;
    char name[MAX_INPUT_CHARS + 1] = "\0";
    int letterCount = 0;
    int framesCounter = 0;

};

class GameScreen: public Screen{
public:
    GameScreen();
    GameScreen(int index, int idleIndex, int speed, int typed, int f, int w, int h);
    GameScreen(int index, int idleIndex, int speed, int typed, int f);
    std::string getRandomWord(std::vector<std::string> wordPool);
    //random current/next?
    void setCurrentWord(std::string w) { currentWord = w; }
    void setNextWord(std::string w) { nextWord = w; }
    void setIdleIndex(int i) { idleIndex = i; }
    void setCharacterIndex(int i) { characterIndex = i; }
    void setCharacterSelectMenu(bool b) { characterSelectMenu  = b; }
    bool typedLetter(char w);
    virtual void typedWord() = 0;
    virtual void resetFrames() = 0;
    bool timesUp();

    //get
    int getFrames() { return frames; }
    std::string getCurrentWord() { return currentWord; }
    std::string getNextWord() { return nextWord; }
    int getTypingIndex() { return typingIndex; }
    int getIdleIndex() { return idleIndex; }
    int getWordsTyped() { return wordTyped; }
    Rectangle getButtonNext() { return buttonNext; }
    bool getCharacterSelectMenu() { return characterSelectMenu; }

    //add-reduct
    void framesCount() { --frames; }
    virtual void reset() = 0;

    void DrawWordOnScreen(std::string random_word, int typing_index, Font font);
    virtual void draw(std::vector<Texture2D> textures, Font font) = 0;
    virtual void update(char key, std::vector<Sound> sounds) = 0;
    virtual void drawScore(std::vector<Texture2D> textures, Font font);
    virtual void drawCharacterSelection(std::vector<Texture2D> textures, Font font);
protected:
    bool characterSelectMenu = true;
    int characterIndex;
    int typingIndex;
    int idleIndex = 0;
    int wpm;
    int incorrectLetters;
    int wordTyped;
    int lettersTyped;
    int frames;
    std::string currentWord;
    std::string nextWord;
    std::vector<std::string> wordPool;
    Rectangle buttonNext = {(width/2.0f) - 100, (height/2.0f) + 100, 200, 50};
};

class TypingTrials: public GameScreen{
public:
    TypingTrials();
    void typedWord();
    void draw(std::vector<Texture2D> textures, Font font);
    void update(char key, std::vector<Sound> sounds);
    void reset();
    void resetFrames(){ frames = FRAME; }
private:
    const int FRAME = 3600; //60sec
};

class TickingTimeBomb: public GameScreen{
public:
    TickingTimeBomb();
    void typedWord();
    void draw(std::vector<Texture2D> textures, Font font);
    void update(char key, std::vector<Sound> sounds);
    void reset();
    void resetFrames(){ frames = FRAME; }
private:
    const int FRAME = 480; //8sec
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
    std::vector<Texture2D> textures;
    std::vector<Sound> sounds;
    Music music;
    Font font;
    Image icon;

};

//class score

#endif
