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



//Screen Functions
Screen::Screen(int w, int h) {
    width = w;
    height = h;
}

bool Screen::buttonClicked(Rectangle button) {
    buttonPressed = false;
    if(CheckCollisionPointRec(GetMousePosition(), button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        buttonPressed= true;
    }
    return buttonPressed;
}
//MainScreen Functions
// bool MainScreen::buttonMode1Clicked(){
//     buttonPressedMode1 = false;
//     if(CheckCollisionPointRec(GetMousePosition(), buttonMode1) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
//         buttonPressedMode1 = true;
//     }
//     return buttonPressedMode1;
// }

// bool MainScreen::buttonMode2Clicked(){
//     buttonPressedMode2 = false;
//     if(CheckCollisionPointRec(GetMousePosition(), buttonMode2) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
//         buttonPressedMode2 = true;
//     }
//     return buttonPressedMode2;
// }
void MainScreen::draw(std::vector<Texture2D> textures, Font font) {
    BeginDrawing();
        ClearBackground(WHITE);
        DrawTexture(textures[2], 20, 0, Color{255, 255, 255, 75});
        DrawTextEx(
            font, msg.c_str(),
            XYtoVector2(
                (width/2) - (MeasureText(msg.c_str(), 60)/3),
                height/2 - 120
            ),
            60,
            textSpacing,
            textColor
        );

        //button-mode1
        DrawRectangleRec(buttonMode1, buttonClicked(buttonMode1) ? textColor : WHITE);
        DrawText("Typing Trials", buttonMode1.x + 30, buttonMode1.y + 15, 20, buttonClicked(buttonMode1) ? WHITE : textColor);
        DrawRectangleLines((width/2) - 100, (height/2) - 40, 200, 50, BLACK);

        //button-mode2
        DrawRectangleRec(buttonMode2, buttonClicked(buttonMode2) ? WHITE : textColor);
        DrawText("Ticking Time Bomb", buttonMode2.x + 10, buttonMode2.y + 10, 20, buttonClicked(buttonMode2) ? textColor : WHITE);
        DrawRectangleLines((width/2) - 100, (height/2) - 40, 200, 50, BLACK);

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


bool GameScreen::timesUp() {
    return frames < 0;
}


TypingTrials::TypingTrials() : GameScreen() {
    frames = FRAME;
} 

void TypingTrials::typedWord(){
    ++wordTyped;
    wpm = (int)wordTyped * FRAME / (FRAME - frames);
    typingIndex = 0;
    setCurrentWord(nextWord);
    setNextWord(getRandomWord(wordPool));
}

void TypingTrials::draw(std::vector<Texture2D> textures, Font font){
    BeginDrawing();
    ClearBackground(Color{245, 240, 255, 255});

    //background
    DrawTexture(textures[2], 20, 0, Color{255, 255, 255, 75});

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

void GameScreen::drawScore(std::vector<Texture2D> textures, Font font) {
    BeginDrawing();
        ClearBackground(WHITE);

        //bg
        DrawTexture(textures[2], 20, 0, Color{255, 255, 255, 75}); 
        
        //bg rectangle
        DrawRectangle(
            (width/2) - ((MeasureText(TextFormat("m i s t a k e s: %d", incorrectLetters), 40) + 80)/2),
            height/2 - 200,
            (MeasureText(TextFormat("m i s t a k e s: %d", incorrectLetters), 40)) + 80,
            height/2 + 40, textBoxColor
        );

        DrawTextEx(font, 
            TextFormat("S C O R E"),
            XYtoVector2((width/2) - ((MeasureText(TextFormat("S C O R E"), 60) - 120)/2),
            height/2 - 140), 60, textSpacing, WHITE
        );

        //wpm
        DrawTextEx(font, 
            TextFormat("w p m: %d", wpm),
            XYtoVector2((width/2) - (((MeasureText(TextFormat("w p m: %d", wpm), 40) - 40)/2)),
            height/2 - 60), 40, textSpacing, textColor
        );

        //incorrect words
        DrawTextEx(font, 
            TextFormat("m i s t a k e s: %d", incorrectLetters),
            XYtoVector2((width/2) - (((MeasureText(TextFormat("m i s t a k e s: %d", incorrectLetters), 40) - 80)/2)),
            height/2 - 20), 40, textSpacing, textColor
        );

        //count
        DrawTextEx(font, 
            TextFormat("s c o r e: %d", wordTyped),
            XYtoVector2((width/2) - (((MeasureText(TextFormat("s c o r e: %d", wordTyped), 40)) - 80)/2),
            height/2 + 20), 40, textSpacing, textColor
        );

        DrawRectangleRec(buttonNext, buttonClicked(buttonNext) ? textColor : WHITE);
        DrawText("Next", buttonNext.x + 60, buttonNext.y + 15, 20, buttonClicked(buttonNext) ? WHITE : textColor);
        DrawRectangleLinesEx(buttonNext, 2, Color{0, 0, 0, 255});

    EndDrawing();
}


TickingTimeBomb::TickingTimeBomb() : GameScreen(){
    frames = FRAME;
}

void TypingTrials::reset() {
    frames = FRAME;
    wpm = 0;
    typingIndex = 0;
    wordTyped = 0;
    incorrectLetters = 0;
    lettersTyped = 0;
    currentWord = getRandomWord(wordPool);
    nextWord = getRandomWord(wordPool);
}

void TickingTimeBomb::reset() {
    frames = FRAME;
    wpm = 0;
    typingIndex = 0;
    wordTyped = 0;
    incorrectLetters = 0;
    lettersTyped = 0;
    currentWord = getRandomWord(wordPool);
    nextWord = getRandomWord(wordPool);

}

void TickingTimeBomb::typedWord(){
    ++wordTyped;
    wpm = (int)wordTyped * FRAME / (FRAME - frames);
    typingIndex = 0;
    setCurrentWord(nextWord);
    setNextWord(getRandomWord(wordPool));
}

void TickingTimeBomb::update(char key){
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
        frames = FRAME;
    } 

}

void TickingTimeBomb::draw(std::vector<Texture2D> textures, Font font){
    BeginDrawing();
    ClearBackground(Color{245, 240, 255, 255});

    //background
    DrawTexture(textures[2], 20, 0, Color{255, 255, 255, 75});

    //stage
    DrawTexture(textures[3], 0, 0, WHITE);

    //timer bar
    DrawRectangleLines(width/2 - 180, 420, 368, 28, WHITE);
    DrawRectangle(width/2 - 176, 424, frames - 120, 20, WHITE);

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
        XYtoVector2(width/2 + 188 - (MeasureText(TextFormat("Score: %d", wordTyped), 25)/2),
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

char key;

void Game::Tick(std::vector<MainScreen>& mains, std::vector<GameScreen*>& modes) {
    //Update all
    //...
    GameScreen* tt = modes[0];
    GameScreen* ttb = modes[1];
    MainScreen mainMenu = mains[0];
    MainScreen end = mains[1];

    switch (gameState) {
        case titleScreen:
        // Update ----------------------------------------------------------------------------------
            if(mainMenu.buttonClicked(mainMenu.getButton1())){
                gameState = mode1;
                tt->reset();
            }
            if(mainMenu.buttonClicked(mainMenu.getButton2())){
                gameState = mode2;
                ttb->reset();
            }

            if (IsKeyPressed(KEY_LEFT_CONTROL)) {
                gameState = endScreen;
            }

        // Drawing ---------------------------------------------------------------------------------
            mainMenu.draw(textures, font);
            break;
        case mode1:
        // Update ----------------------------------------------------------------------------------
            key = GetCharPressed();

            tt->update(key);

            // frames -= 1
            tt->framesCount();

        //timer: 60 seconds (60fps)
            if (tt->timesUp()){
                if (tt->buttonClicked(tt->getButtonNext())){
                    gameState = endScreen;
                }
                tt->drawScore(textures, font);
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

            break;

        case mode2:
             // Update ----------------------------------------------------------------------------------
            key = GetCharPressed();

            ttb->update(key);

            // frames -= 1
            ttb->framesCount();

            //timer: 60 seconds (60fps)
            if (ttb->timesUp()){
                if (ttb->buttonClicked(ttb->getButtonNext())) gameState = endScreen;
                ttb->drawScore(textures, font);
            } else {
                ttb->draw(textures, font);
            }

            if (ttb->getFrames() % 30 == 0 && ttb->getIdleIndex() < 1){
                ttb->setIdleIndex(1);
            } else if (ttb->getFrames() % 30 == 0 && ttb->getIdleIndex() > 0){
                ttb->setIdleIndex(0);
            }

            //force quit
            if (IsKeyPressed(KEY_LEFT_CONTROL)) gameState = endScreen;

        // Drawing ---------------------------------------------------------------------------------
            break;
        
        case endScreen:
            if (end.buttonClicked(end.getButton1())){
                gameState = mode1;
                tt->reset();
                // tt->setCurrentWord(modes[0]->getRandomWord(word_pool));
                // tt->setNextWord(modes[0]->getRandomWord(word_pool));
            }
            if(end.buttonClicked(end.getButton2())){
                gameState = mode2;
                ttb->reset();
                // ttb->setCurrentWord(modes[0]->getRandomWord(word_pool));
                // ttb->setNextWord(modes[0]->getRandomWord(word_pool));
            }

        // Update ----------------------------------------------------------------------------------
            if (IsKeyPressed(KEY_A)) {
                gameState = titleScreen;
            }

        // Drawing ---------------------------------------------------------------------------------
            mains[1].draw(textures, font);
            break;
    }  
}

