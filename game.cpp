#include "game.hpp"

//json vec
std::vector<std::string> jsonVec(ns::wordpool wp, std::string fileName){
    std::ifstream i(fileName);
    json jsonFile;
    i >> jsonFile;

    wp.language = jsonFile["name"];

    const auto& words = jsonFile["words"];
    for (const auto& word : words) {
        wp.words.push_back(word);
    }

    return wp.words;
}

//Game Functions
Game::Game(int width, int height, std::string title): width(width), height(height), gameState(titleScreen){
    assert(!GetWindowHandle()); //Check if window has not been created
    InitWindow(width, height, title.c_str());
    SetTargetFPS(60);

    ns::wordpool eng;
    this->word_pool = jsonVec(eng, "../TypingPractice/words.json");

}

Game::~Game() noexcept {
    assert(GetWindowHandle()); //Check if there is a window
    CloseWindow();
}

bool Game::GameShouldClose() const {
    return WindowShouldClose();
}

//themes
const int textBoxSize = 60;
const Color textBoxColor = Color{186, 255, 204, 255};
const Color textBoxColorHighlighted = Color{166, 247, 187, 255};
const Color textColor = Color{44, 115, 62, 255};

TextBox::TextBox(int x, int y, char letter, Color color): letter(letter){
    DrawRectangle(x, y, textBoxSize, textBoxSize, color);
    DrawText(std::string(1, letter).c_str(), x-7+textBoxSize/2, y-15+textBoxSize/2, textBoxSize/2, textColor);
}


//Screen Functions
Screen::Screen(int w, int h) {
    width = w;
    height = h;
}

//MainScreen Functions
void MainScreen::draw() {
    BeginDrawing();
        ClearBackground(WHITE);
        DrawText(msg.c_str(), width/2, height/2, 20, textColor);
    EndDrawing();
}

//GameScreen Functions
GameScreen::GameScreen() : Screen(), typingIndex(0), wpm(0), wordTyped(0), frames(3600), lettersTyped(0), incorrectLetters(0) {
    SetTargetFPS(60);
    ns::wordpool eng;
    this->wordPool = jsonVec(eng, "../TypingPractice/words.json");
    setCurrentWord(getRandomWord(wordPool));
    setNextWord(getRandomWord(wordPool));
}

GameScreen::GameScreen(int index, int speed, int typed, int f, int w, int h)
    : Screen(w, h), typingIndex(index), wpm(speed), wordTyped(typed), lettersTyped(0), incorrectLetters(0), frames(f) {
    SetTargetFPS(60);
    ns::wordpool eng;
    this->wordPool = jsonVec(eng, "../TypingPractice/words.json");
    setCurrentWord(getRandomWord(wordPool));
    setNextWord(getRandomWord(wordPool));
}

void GameScreen::DrawWordOnScreen(std::string current_word, int typing_index){
    std::vector<TextBox> text;

    for (int i = 0; i < current_word.size(); i++){
        int x_pos = (GetScreenWidth()/2) + (textBoxSize * 3/2 * (i)) - (textBoxSize * 3/4 * current_word.length() - textBoxSize/4);

        if (i == typing_index){ //highlighted letter textbox
            text.push_back(TextBox{x_pos, 280, current_word[i], textBoxColorHighlighted});
            //border decoration it doesnt work wtf
            //DrawRectangleLines(x_pos, 280, textBox_size + 2, textBox_size + 2, Color{84, 218, 119});
        
        } else { //normal letter textbox
            text.push_back(TextBox{x_pos, 300, current_word[i], textBoxColor});
        }
    }
}

std::string GameScreen::getRandomWord(std::vector<std::string> wordPool) {
    return wordPool[GetRandomValue(0, wordPool.size() - 1)];
}

void GameScreen::draw() {
    BeginDrawing();
    ClearBackground(Color{210, 248, 249});

    //background
    DrawRectangle(0, height/2 - 10, 1280, 20, Color{134, 234, 167, 255});
    DrawRectangle(0, height/2 + 10, 1280, 500, Color{115, 216, 149, 255});

    //timer bar
    DrawRectangleLines(width/2 - 180, 420, 368, 28, textColor);
    DrawRectangle(width/2 - 176, 424, frames / 10, 20, textColor);

    //next word
    DrawText(
        TextFormat("Next Word: %s", nextWord.c_str()),
        width/2 - MeasureText(TextFormat("Next Word: %s", nextWord.c_str()), 20)/2,
        height/2 + 30, 20, textColor
    );
    //wpm
    DrawText(TextFormat("WPM: %d", wpm), width/2 - 180, height/2 + 100, 20, textColor);
    //count
    DrawText(
        TextFormat("Score: %d", wordTyped),
        width/2 + 188 - MeasureText(TextFormat("Score: %d", wordTyped), 20),
        height/2 + 100, 20, textColor
    );

    DrawWordOnScreen(currentWord, typingIndex);
    EndDrawing();
}

bool GameScreen::typedLetter(char w) { 
    return w == currentWord[typingIndex] || w == currentWord[typingIndex] - 32;
}

void GameScreen::typedWord(){
    ++wordTyped;
    wpm = (int)wordTyped * 3600 / (3600 - frames);
    typingIndex = 0;
    setCurrentWord(nextWord);
    setNextWord(getRandomWord(wordPool));
}


int incorrectLetters;
char key;
int frames;



void Game::Tick(std::vector<MainScreen>& mains, std::vector<GameScreen>& modes) {
    //Update all
    //...

    switch (gameState){
        case titleScreen:
        // Update ----------------------------------------------------------------------------------
            if (IsKeyPressed(KEY_SPACE)){
                gameState = TypingTrials;
                modes[0].setCurrentWord(modes[0].getRandomWord(word_pool));
                modes[0].setNextWord(modes[0].getRandomWord(word_pool));
            }

            if (IsKeyPressed(KEY_Q)) {
                gameState = endScreen;
            }

        // Drawing ---------------------------------------------------------------------------------
            mains[0].draw();
            break;


        case TypingTrials:
        // Update ----------------------------------------------------------------------------------
            key = GetCharPressed();

            // frames -= 1
            modes[0].framesCount();

            //timer: 60 seconds (60fps)
            if (modes[0].getFrames() < 0){
                gameState = endScreen;
            }

            //force quit
            if (IsKeyPressed(KEY_LEFT_SHIFT)) gameState = endScreen;

            //check if key input = index of the current letter
            if (modes[0].typedLetter(key)) {
                modes[0].addLettersTyped();
                modes[0].addTypingIndex();
            } else if (key > 0) {
                ++incorrectLetters;
            }

            //reset index and random a new word
            if (modes[0].getTypingIndex() >= modes[0].getCurrentWord().size()) {
                modes[0].typedWord();
            }

        // Drawing ---------------------------------------------------------------------------------
            modes[0].draw();
            break;

        case endScreen:

        // Update ----------------------------------------------------------------------------------
            if (IsKeyPressed(KEY_A)) {
                gameState = titleScreen;
            }

        // Drawing ---------------------------------------------------------------------------------
            mains[1].draw();
            break;
    }  


}
