#include "game.hpp"


//read json file into vec
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
    //init window
    assert(!GetWindowHandle()); //Check if window has not been created
    InitWindow(width, height, title.c_str()); 
    SetTargetFPS(60);
    
    //textures
    Texture2D rabbit = LoadTexture("../assets/dubu.png");
    Texture2D bear = LoadTexture("../assets/nongbear.png");
    Texture2D background = LoadTexture("../assets/bg.png");
    Texture2D stage = LoadTexture("../assets/stage.png");
    textures = {rabbit, bear, background, stage};

    //fonts
    //font = LoadFontEx("../assets/rainyhearts.ttf", 30, 0, 0);
    font = LoadFontEx("../assets/levi-windows.ttf", 25, 0, 0);
    icon = LoadImage("../assets/icon2.png");
    SetWindowIcon(icon);

    //init dictionary/wordpool
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
const int TEXTBOXSIZE = 60;
const Color TEXTBOXCOLOR = Color{227, 226, 254, 255};
const Color TEXTBOXCOLORHIGHLIGHTED = Color{180, 177, 255, 255};
const Color TEXTCOLOR = Color{117, 79, 186, 255};
const int TEXTSPACING = 2;
Rectangle source;

Vector2 XYtoVector2(int x, int y){
    return Vector2{(float)x, (float)y};
}

//rectangle with letter
TextBox::TextBox(int x, int y, char letter, Font font, Color color): letter(letter){
    DrawRectangle(x, y, TEXTBOXSIZE, TEXTBOXSIZE, color);
    DrawTextEx(font, std::string(1, letter).c_str(), XYtoVector2(x-7+TEXTBOXSIZE/2, y-15+TEXTBOXSIZE/2), 35, TEXTSPACING, TEXTCOLOR);
}

//Screen Functions
Screen::Screen(int w, int h) {
    width = w;
    height = h;
    SetTargetFPS(10);
}

bool Screen::buttonClicked(Rectangle button) {
    buttonPressed = false;
    if(CheckCollisionPointRec(GetMousePosition(), button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        buttonPressed= true;
    }
    return buttonPressed;
}

bool MainScreen::mouseOnText(Rectangle textbox){
    if (CheckCollisionPointRec(GetMousePosition(), textbox)) mouseonText = true;
    else mouseonText = false;
    return mouseonText;
}

void MainScreen::typingName(){
    if(mouseOnText(nameBox)){
        // Set the window's cursor to the I-Beam
        SetMouseCursor(MOUSE_CURSOR_IBEAM);
        int key = GetCharPressed();

        while (key > 0){
            if ((key >= 32) && (key <= 125) && (letterCount < MAX_INPUT_CHARS))
            {
                name[letterCount] = (char)key;
                name[letterCount+1] = '\0'; // Add null terminator at the end of the string.
                letterCount++;
            }

            key = GetCharPressed();  // Check next character in the queue
        }

        if (IsKeyPressed(KEY_BACKSPACE)){
            letterCount--;
            if (letterCount < 0) letterCount = 0;
            name[letterCount] = '\0';
        } 
    }else SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    
    if (mouseOnText(nameBox)) framesCounter++;
    else framesCounter = 0;
}

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
            TEXTSPACING,
            TEXTCOLOR
        );
        
        //button-mode1
        DrawRectangleRec(buttonMode1, buttonClicked(buttonMode1) ? TEXTCOLOR : WHITE);
        DrawText("Typing Trials", buttonMode1.x + 30, buttonMode1.y + 15, 20, buttonClicked(buttonMode1) ? WHITE : TEXTCOLOR);
        DrawRectangleLinesEx(buttonMode1, 2, BLACK);

        //button-mode2
        DrawRectangleRec(buttonMode2, buttonClicked(buttonMode2) ? WHITE : TEXTCOLOR);
        DrawText("Ticking Time Bomb", buttonMode2.x + 10, buttonMode2.y + 10, 20, buttonClicked(buttonMode2) ? TEXTCOLOR : WHITE);

        //prectice-mode
        DrawRectangleRec(practiceMode, buttonClicked(practiceMode) ? TEXTCOLOR : WHITE);
        DrawText("Practice Mode", practiceMode.x + 30, practiceMode.y + 15, 20, buttonClicked(practiceMode) ? WHITE : TEXTCOLOR);
        DrawRectangleLinesEx(practiceMode, 2, BLACK);

        //namebox
        DrawRectangleRec(nameBox, TEXTBOXCOLOR);
        if (mouseOnText(nameBox)) DrawRectangleLines((int)nameBox.x, (int)nameBox.y, (int)nameBox.width, (int)nameBox.height, TEXTCOLOR);
            else DrawRectangleLines((int)nameBox.x, (int)nameBox.y, (int)nameBox.width, (int)nameBox.height, TEXTBOXCOLORHIGHLIGHTED);

            DrawText(name, (int)nameBox.x + 5, (int)nameBox.y + 8, 40, TEXTCOLOR);

            DrawText(TextFormat("ENTER NAME: %i/%i", letterCount, MAX_INPUT_CHARS),(width/2) - 100, (height/2) + 200, 20, TEXTBOXCOLORHIGHLIGHTED);

            if (mouseOnText(nameBox))
            {
                if (letterCount < MAX_INPUT_CHARS)
                {
                    // Draw blinking underscore char
                    if (((framesCounter/20)%2) == 0) DrawText("_", (int)nameBox.x + 8 + MeasureText(name, 40), (int)nameBox.y + 12, 40, TEXTCOLOR);
                }
                else DrawText("Press BACKSPACE to delete chars...", 230, 300, 20, TEXTBOXCOLOR);
            }

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
        int x_pos = (GetScreenWidth()/2) + (TEXTBOXSIZE * 3/2 * (i)) - (TEXTBOXSIZE * 3/4 * current_word.length() - TEXTBOXSIZE/4);

        if (i == typing_index){ //highlighted letter textbox
            text.push_back(TextBox{x_pos, 280, current_word[i], font, TEXTBOXCOLORHIGHLIGHTED});
            DrawRectangleLinesEx(Rectangle{(float) x_pos - 4, 276.0f, TEXTBOXSIZE + 8.0f,  TEXTBOXSIZE + 8.0f}, 2, Color{143, 139, 231, 255});
        
        } else { //normal letter textbox
            text.push_back(TextBox{x_pos, 300, current_word[i], font, TEXTBOXCOLOR});
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
        25, TEXTSPACING, WHITE
    );
    //wpm
    DrawTextEx(font, TextFormat("WPM: %d", wpm), XYtoVector2(width/2 - 180, height/2 + 100), 25, TEXTSPACING, WHITE);
    //count
    DrawTextEx(font, 
        TextFormat("Score: %d", wordTyped),
        XYtoVector2(width/2 + 188 - MeasureText(TextFormat("Score: %d", wordTyped), 20),
        height/2 + 100),
        25, TEXTSPACING, WHITE
    );

    source = Rectangle{idleIndex * TEXTBOXSIZE * 1.0f, 0.0f, 60.0f, 60.0f};

    DrawWordOnScreen(currentWord, typingIndex, font);

    DrawTextureRec(textures[1], source, Vector2{
        ((GetScreenWidth()/2) + (TEXTBOXSIZE * 3/2 * (typingIndex)) - (TEXTBOXSIZE * 3/4 * currentWord.length() - TEXTBOXSIZE/4) * 1.0f)
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
            height/2 + 40, TEXTBOXCOLOR
        );

        DrawTextEx(font, 
            TextFormat("S C O R E"),
            XYtoVector2((width/2) - ((MeasureText(TextFormat("S C O R E"), 60) - 120)/2),
            height/2 - 140), 60, TEXTSPACING, WHITE
        );

        //wpm
        DrawTextEx(font, 
            TextFormat("w p m: %d", wpm),
            XYtoVector2((width/2) - (((MeasureText(TextFormat("w p m: %d", wpm), 40) - 40)/2)),
            height/2 - 80), 40, TEXTSPACING, TEXTCOLOR
        );

        //incorrect words
        DrawTextEx(font, 
            TextFormat("m i s t a k e s: %d", incorrectLetters),
            XYtoVector2((width/2) - (((MeasureText(TextFormat("m i s t a k e s: %d", incorrectLetters), 40) - 80)/2)),
            height/2 - 20), 40, TEXTSPACING, TEXTCOLOR
        );

        //count
        DrawTextEx(font, 
            TextFormat("s c o r e: %d", wordTyped),
            XYtoVector2((width/2) - (((MeasureText(TextFormat("s c o r e: %d", wordTyped), 40)) - 80)/2),
            height/2 + 40), 40, TEXTSPACING, TEXTCOLOR
        );

        DrawRectangleRec(buttonNext, buttonClicked(buttonNext) ? TEXTCOLOR : WHITE);
        DrawText("Next", buttonNext.x + 80, buttonNext.y + 15, 20, buttonClicked(buttonNext) ? WHITE : TEXTCOLOR);
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
        25, TEXTSPACING, WHITE
    );
    //wpm
    DrawTextEx(font, TextFormat("WPM: %d", wpm), XYtoVector2(width/2 - 180, height/2 + 100), 25, TEXTSPACING, WHITE);
    //count
    DrawTextEx(font, 
        TextFormat("Score: %d", wordTyped),
        XYtoVector2(width/2 + 188 - (MeasureText(TextFormat("Score: %d", wordTyped), 25)/2),
        height/2 + 100),
        25, TEXTSPACING, WHITE
    );

    source = Rectangle{idleIndex * TEXTBOXSIZE * 1.0f, 0.0f, 60.0f, 60.0f};

    DrawWordOnScreen(currentWord, typingIndex, font);

    DrawTextureRec(textures[1], source, Vector2{
        ((GetScreenWidth()/2) + (TEXTBOXSIZE * 3/2 * (typingIndex)) - (TEXTBOXSIZE * 3/4 * currentWord.length() - TEXTBOXSIZE/4) * 1.0f)
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

            if(mainMenu.buttonClicked(mainMenu.getNameBox())){
                mainMenu.typingName();
            }
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

