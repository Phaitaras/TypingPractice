#ifndef game_HPP
#define game_HPP
#define MAX_INPUT_CHARS     9

#include <raylib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <assert.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum GameState{titleScreen, mode1, mode2, practicemode, endScreen, skipped, scoreBoard1, scoreBoard2};

namespace ns {
    struct wordpool {
        std::string language;
        std::vector<std::string> words;
    };
}

struct NameScore{
    char name;
    int score;
};

struct SortByWPMDesc {
    bool operator()(const json& a, const json& b) const {
        return a["score"] > b["score"];
    }
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
    Rectangle getButtonScoreBoard() {return buttonScoreBoard;}
    Rectangle getButtonBack() {return buttonBack;}
    bool getCharacterSelectMenu() { return characterSelectMenu; }
    virtual int getWPM() { return wpm; }
    virtual int getWordTyped() { return wordTyped; }

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
    Rectangle buttonBack = {100, 100, 100, 45};
    Rectangle buttonNext = {(width/2.0f) - 100, (height/2.0f) + 100, 200, 50};
    Rectangle buttonScoreBoard = {(width/2.0f) - 100, (height/2.0f) + 250, 200, 50};

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
    void addTotalFrames(){ ++totalframes; }
private:
    const int FRAME = 300; // 5 sec
    int totalframes;
};

class PracticeMode: public GameScreen{
public: 
    PracticeMode();
    void typedWord();
    void draw(std::vector<Texture2D> textures, Font font);
    void update(char key, std::vector<Sound> sounds);
    void reset();
    void resetFrames(){ frames = 0; }
private:
};

//screen for displaying messages or info
class MainScreen: public Screen{
public:
    MainScreen(): Screen() { msg = ""; }
    MainScreen(std::string t): Screen() { msg = t; }

    void draw(std::vector<Texture2D> textures, Font font);
    Rectangle getButton1() { return buttonMode1; }
    Rectangle getButton2() { return buttonMode2; }
    Rectangle getPracticeButton() {return practiceMode;}
    Rectangle getNameBox() {return nameBox;}
    Rectangle getConfirmBox() {return confirmNameBox;}
    Rectangle getButtonBack() {return buttonBack;}
    virtual std::string getName();
    void setName();

    bool mouseOnText(Rectangle textbox);
    void typingName();
    void drawNameBox();
    json loadJsonFile();
    void addPlayerName(GameScreen* gameScreen, std::string mode);
    void addPlayerData(GameScreen* gameScreen, std::string mode);
    void saveJsonFile(const json& jsonData);
    void drawScoreBoard(std::vector<Texture2D> textures, Font font,std::string mode);
protected:
    std::string msg;
    std::string finalName;
private:
    Rectangle buttonMode1 = {(width/2.0f) - 100, (height/2.0f) - 30, 200, 50};
    Rectangle buttonMode2 = {(width/2.0f) - 100, (height/2.0f) + 30, 200, 50};
    Rectangle practiceMode = {(width/2.0f) - 100, (height/2.0f) + 90, 200, 50};

    Rectangle nameBox = { width/2.0f - 100, (height/2.0f) + 240, 200, 50 };
    Rectangle confirmNameBox = { width/2.0f + 115, (height/2.0f) + 240, 35, 50 };
    Rectangle buttonBack = {100, 100, 100, 50};

    bool mouseonText = false;
    char name[MAX_INPUT_CHARS + 1] = "\0";
    int letterCount = 0;
    int framesCounter = 0;
    json scorefile;

};

class Game{
public:
    Game(int width, int height, std::string gameTitle);
    Game(const Game& other) = delete;
    Game& operator=(const Game& other) = delete;
    ~Game() noexcept;
    bool GameShouldClose() const;
    void Tick(std::vector<MainScreen*>& mains, std::vector<GameScreen*>& modes);
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
