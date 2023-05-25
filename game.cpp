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
    
    //textures
    Texture2D rabbit = LoadTexture("../assets/dubu.png");
    Texture2D bear = LoadTexture("../assets/nongbear.png");
    Texture2D background = LoadTexture("../assets/bg.png");
    Texture2D stage = LoadTexture("../assets/stage.png");

    textures.push_back(rabbit);
    textures.push_back(bear);
    textures.push_back(background);
    textures.push_back(stage);

    //font = LoadFontEx("../assets/rainyhearts.ttf", 30, 0, 0);
    font = LoadFontEx("../assets/levi-windows.ttf", 25, 0, 0);
    icon = LoadImage("../assets/icon2.png");
    SetWindowIcon(icon);

    ns::wordpool eng;
    this->word_pool = jsonVec(eng, "../words.json");

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
const Color textBoxColor = Color{227, 226, 254, 255};
const Color textBoxColorHighlighted = Color{180, 177, 255, 255};
const Color textColor = Color{117, 79, 186, 255};
const int textSpacing = 2;
Rectangle source;

Vector2 XYtoVector2(int x, int y){
    return Vector2{(float)x, (float)y};
}

TextBox::TextBox(int x, int y, char letter, Font font, Color color): letter(letter){
    DrawRectangle(x, y, textBoxSize, textBoxSize, color);
    DrawTextEx(font, std::string(1, letter).c_str(), XYtoVector2(x-7+textBoxSize/2, y-15+textBoxSize/2), 35, textSpacing, textColor);
}

void TypingTrials::drawScore(Font font) {
    BeginDrawing();
        ClearBackground(WHITE);

        //bg rectangle
        DrawRectangle(
            width/2 - (MeasureText(TextFormat("Incorrect Letters: %d", incorrectLetters), 40)/2 + 40),
            height/2 - 260,
            (MeasureText(TextFormat("Incorrect Letters: %d", incorrectLetters), 40)) + 80,
            height/2 + 40, PURPLE
        );

        DrawTextEx(font, 
            TextFormat("Score"),
            XYtoVector2((width/2) - (MeasureText(TextFormat("Score"), 60)/2),
            height/2 - 180), 60, textSpacing, WHITE
        );

        //incorrect words
        DrawTextEx(font, 
            TextFormat("Incorrect Letters: %d", incorrectLetters),
            XYtoVector2((width/2) - (MeasureText(TextFormat("Incorrect Letters: %d", incorrectLetters), 40)/2),
            height/2 - 40), 40, textSpacing, textColor
        );

        //wpm
        DrawTextEx(font, 
            TextFormat("WPM: %d", wpm),
            XYtoVector2((width/2) - (MeasureText(TextFormat("WPM: %d", wpm), 40)/2),
            height/2 - 80), 40, textSpacing, textColor
        );

        //count
        DrawTextEx(font, 
            TextFormat("Score: %d", wordTyped),
            XYtoVector2((width/2) - (MeasureText(TextFormat("Score: %d", wordTyped), 40)/2),
            height/2), 40, textSpacing, textColor
        );

    EndDrawing();
}

//Screen Functions
Screen::Screen(int w, int h) {
    width = w;
    height = h;
}

//MainScreen Functions
void MainScreen::draw(std::vector<Texture2D> textures, Font font) {
    BeginDrawing();
        ClearBackground(WHITE);
        DrawTextEx(font, msg.c_str(), XYtoVector2(width/2, height/2), font.baseSize, textSpacing, textColor);
    EndDrawing();
}

//GameScreen Functions
GameScreen::GameScreen() : Screen(), typingIndex(0), idleIndex(0), wpm(0), wordTyped(0), frames(0), lettersTyped(0), incorrectLetters(0) {
    SetTargetFPS(60);
    ns::wordpool eng;
    this->wordPool = jsonVec(eng, "../words.json");
    setCurrentWord(getRandomWord(wordPool));
    setNextWord(getRandomWord(wordPool));
}

GameScreen::GameScreen(int index, int idleIndex, int speed, int typed, int f, int w, int h)
    : Screen(w, h), typingIndex(index), idleIndex(idleIndex), wpm(speed), wordTyped(typed), lettersTyped(0), incorrectLetters(0), frames(f) {
    SetTargetFPS(60);
    ns::wordpool eng;
    this->wordPool = jsonVec(eng, "../words.json");
    setCurrentWord(getRandomWord(wordPool));
    setNextWord(getRandomWord(wordPool));
}

GameScreen::GameScreen(int index, int idleIndex, int speed, int typed, int f)
    : Screen(), typingIndex(index), idleIndex(idleIndex), wpm(speed), wordTyped(typed), lettersTyped(0), incorrectLetters(0), frames(f) {
        
};

void GameScreen::DrawWordOnScreen(std::string current_word, int typing_index, Font font){
    std::vector<TextBox> text;

    for (int i = 0; i < current_word.size(); i++){
        int x_pos = (GetScreenWidth()/2) + (textBoxSize * 3/2 * (i)) - (textBoxSize * 3/4 * current_word.length() - textBoxSize/4);

        if (i == typing_index){ //highlighted letter textbox
            text.push_back(TextBox{x_pos, 280, current_word[i], font, textBoxColorHighlighted});
            DrawRectangleLinesEx(Rectangle{(float) x_pos - 4, 276.0f, textBoxSize + 8.0f,  textBoxSize + 8.0f}, 2, Color{143, 139, 231, 255});
        
        } else { //normal letter textbox
            text.push_back(TextBox{x_pos, 300, current_word[i], font, textBoxColor});
        }
    }
}

std::string GameScreen::getRandomWord(std::vector<std::string> wordPool) {
    return wordPool[GetRandomValue(0, wordPool.size() - 1)];
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

TypingTrials::TypingTrials() : GameScreen() {
    frames = 3600;
    SetTargetFPS(60);
    ns::wordpool eng;
    this->wordPool = jsonVec(eng, "../words.json");
    setCurrentWord(getRandomWord(wordPool));
    setNextWord(getRandomWord(wordPool));
} 

void TypingTrials::draw(std::vector<Texture2D> textures, Font font){
    BeginDrawing();
    ClearBackground(Color{245, 240, 255, 255});

    //background
    DrawTexture(textures[2], 0, 0, Color{255, 255, 255, 75});

    //stage
    DrawTexture(textures[3], 0, 0, WHITE);

    //timer bar
    DrawRectangleLines(width/2 - 180, 420, 368, 28, WHITE);
    DrawRectangle(width/2 - 176, 424, frames / 10, 20, WHITE);

    //next word
    DrawTextEx(font, 
        TextFormat("Next Word: %s", nextWord.c_str()),
        XYtoVector2(width/2 - MeasureText(TextFormat("Next Word: %s", nextWord.c_str()), 20)/2, height/2 + 25),
        25, textSpacing, WHITE
    );
    //wpm
    DrawTextEx(font, TextFormat("WPM: %d", wpm), XYtoVector2(width/2 - 180, height/2 + 100), 25, textSpacing, WHITE);
    //count
    DrawTextEx(font, 
        TextFormat("Score: %d", wordTyped),
        XYtoVector2(width/2 + 188 - MeasureText(TextFormat("Score: %d", wordTyped), 20),
        height/2 + 100),
        25, textSpacing, WHITE
    );

    source = Rectangle{idleIndex * textBoxSize * 1.0f, 0.0f, 60.0f, 60.0f};

    DrawWordOnScreen(currentWord, typingIndex, font);

    DrawTextureRec(textures[1], source, Vector2{
        ((GetScreenWidth()/2) + (textBoxSize * 3/2 * (typingIndex)) - (textBoxSize * 3/4 * currentWord.length() - textBoxSize/4) * 1.0f)
        , 220}, WHITE);
    EndDrawing();
}

bool GameScreen::timesUp() {
    return frames < 0;
}

void TypingTrials::update(char key) {
    framesCount();
    if (typedLetter(key)) {
        addLettersTyped();
        addTypingIndex();
    }
    else if (key > 0 && !timesUp()) {
        ++incorrectLetters;
    }

    if(typingIndex >= currentWord.size()) {
        typedWord();
    } 
}

char key;

void Game::Tick(std::vector<MainScreen>& mains, std::vector<GameScreen*>& modes) {
    //Update all
    //...
    GameScreen* tt = modes[0];

    switch (gameState){
        case titleScreen:
        // Update ----------------------------------------------------------------------------------
            if (IsKeyPressed(KEY_SPACE)){
                gameState = mode1;
                modes[0]->setCurrentWord(modes[0]->getRandomWord(word_pool));
                modes[0]->setNextWord(modes[0]->getRandomWord(word_pool));
            }

            if (IsKeyPressed(KEY_Q)) {
                gameState = endScreen;
            }


        // Drawing ---------------------------------------------------------------------------------
            mains[0].draw(textures, font);
            break;


        case mode1:
        // Update ----------------------------------------------------------------------------------
            key = GetCharPressed();

            tt->update(key);

            // frames -= 1
            tt->framesCount();

            //timer: 60 seconds (60fps)
            if (tt->timesUp()){
                tt->drawScore(font);
            } else {
                tt->draw(textures, font);
            }

            if (tt->getFrames() % 30 == 0 && tt->getIdleIndex() < 1){
                tt->setIdleIndex(1);
            } else if (tt->getFrames() % 30 == 0 && tt->getIdleIndex() > 0){
                tt->setIdleIndex(0);
            }

            //force quit
            if (IsKeyPressed(KEY_LEFT_CONTROL)) gameState = endScreen;

            if (IsKeyPressed(KEY_LEFT_SHIFT)) gameState = skipped;

        // Drawing ---------------------------------------------------------------------------------
            // tt->draw();
            break;

        case endScreen:

        // Update ----------------------------------------------------------------------------------
            if (IsKeyPressed(KEY_A)) {
                gameState = titleScreen;
            }

        // Drawing ---------------------------------------------------------------------------------
            mains[1].draw(textures, font);
            break;

        case skipped:
            modes[0]->drawScore(font);
            break;

    }  


}

