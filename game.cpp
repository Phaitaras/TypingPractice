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
    InitAudioDevice();
    SetTargetFPS(60);

    //audio
    music = LoadMusicStream("../assets/sounds/bgmusic.wav");
    PlayMusicStream(music);
    SetMusicVolume(music, 0.65f);

    Sound typeSound = LoadSound("../assets/sounds/clickCorrect.wav");
    Sound wordCompleteSound = LoadSound("../assets/sounds/wordCorrect.wav");
    Sound modeCompleteSound = LoadSound("../assets/sounds/win.wav");
    Sound mouseClickSound = LoadSound("../assets/sounds/mousepik.wav");

    sounds.push_back(typeSound);
    sounds.push_back(wordCompleteSound);
    sounds.push_back(modeCompleteSound);
    sounds.push_back(mouseClickSound);
    
    //textures
    Texture2D rabbit = LoadTexture("../assets/sprites/dubu.png");
    Texture2D bear = LoadTexture("../assets/sprites/nongbear.png");
    Texture2D background = LoadTexture("../assets/sprites/bg.png");
    Texture2D stage = LoadTexture("../assets/sprites/stage.png");
    textures = {rabbit, bear, background, stage};

    textures.push_back(rabbit);
    textures.push_back(bear);
    textures.push_back(background);
    textures.push_back(stage);

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
    CloseAudioDevice();
    CloseWindow();
}

bool Game::GameShouldClose() const {
    return WindowShouldClose();
}

//themes
const int TEXT_BOX_SIZE = 60;
const Color TEXT_BOX_COLOR = Color{227, 226, 254, 255};
const Color TEXT_BOX_COLOR_HIGHLIGHTED = Color{180, 177, 255, 255};
const Color TEXT_COLOR = Color{117, 79, 186, 255};
const int TEXT_SPACING = 2;
int backgroundPos = 0;
int backgroundPos2 = 720;
Rectangle source;

Vector2 XYtoVector2(int x, int y){
    return Vector2{(float)x, (float)y};
}

//rectangle with letter
TextBox::TextBox(int x, int y, char letter, Font font, Color color): letter(letter){
    DrawRectangle(x, y, TEXT_BOX_SIZE, TEXT_BOX_SIZE, color);
    DrawTextEx(font, std::string(1, letter).c_str(), XYtoVector2(x-7+TEXT_BOX_SIZE/2, y-15+TEXT_BOX_SIZE/2), 35, TEXT_SPACING, TEXT_COLOR);
}

//Screen functions
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

//MainScreen functions
bool MainScreen::mouseOnText(Rectangle textbox){
    if (CheckCollisionPointRec(GetMousePosition(), textbox)) mouseonText = true;
    else mouseonText = false;
    return mouseonText;
}

void MainScreen::typingName(){
    SetMouseCursor(MOUSE_CURSOR_IBEAM);
    int key = GetCharPressed();
    std::cout << "key: " << key << std::endl;
    while (key > 0) {
        if ((key >= 32) && (key <= 125) && (letterCount < MAX_INPUT_CHARS))
        {
            name[letterCount] = (char)key;
            name[letterCount + 1] = '\0'; // Add null terminator at the end of the string.
            letterCount++;
            std::cout << "name: " << name << std::endl;
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)){
        letterCount--;
        if (letterCount < 0) letterCount = 0;
        name[letterCount] = '\0';
    } 

    if (mouseOnText(nameBox)) framesCounter++;
    else framesCounter = 0;
}

void MainScreen::draw(std::vector<Texture2D> textures, Font font) {
    BeginDrawing();
        ClearBackground(WHITE);
        DrawTexture(textures[2], backgroundPos, backgroundPos - 8, Color{255, 255, 255, 75});
        
        DrawTextEx(
            font, msg.c_str(),
            XYtoVector2(
                (width/2) - (MeasureText(msg.c_str(), 60)/3),
                height/2 - 120
            ),
            60,
            TEXT_SPACING,
            TEXT_COLOR
        );
        
        //button-mode1
        DrawRectangleRec(buttonMode1, buttonClicked(buttonMode1) ? TEXT_COLOR : WHITE);
        DrawText("Typing Trials", buttonMode1.x + 30, buttonMode1.y + 15, 20, buttonClicked(buttonMode1) ? WHITE : TEXT_COLOR);
        DrawRectangleLinesEx(buttonMode1, TEXT_SPACING, BLACK);

        //button-mode2
        DrawRectangleRec(buttonMode2, buttonClicked(buttonMode2) ? WHITE : TEXT_COLOR);
        DrawText("Ticking Time Bomb", buttonMode2.x + 10, buttonMode2.y + 10, 20, buttonClicked(buttonMode2) ? TEXT_COLOR : WHITE);

        //prectice-mode
        DrawRectangleRec(practiceMode, buttonClicked(practiceMode) ? TEXT_COLOR : WHITE);
        DrawText("Practice Mode", practiceMode.x + 30, practiceMode.y + 15, 20, buttonClicked(practiceMode) ? WHITE : TEXT_COLOR);
        DrawRectangleLinesEx(practiceMode, TEXT_SPACING, BLACK);

        //namebox
        DrawRectangleRec(nameBox, TEXT_BOX_COLOR);
        if (mouseOnText(nameBox)) DrawRectangleLines((int)nameBox.x, (int)nameBox.y, (int)nameBox.width, (int)nameBox.height, TEXT_COLOR);
            else DrawRectangleLines((int)nameBox.x, (int)nameBox.y, (int)nameBox.width, (int)nameBox.height, TEXT_BOX_COLOR_HIGHLIGHTED);

            DrawText(name, (int)nameBox.x + 5, (int)nameBox.y + 8, 40, TEXT_COLOR);

            DrawText(TextFormat("ENTER NAME: %i/%i", letterCount, MAX_INPUT_CHARS),(width/2) - 100, (height/2) + 200, 20, TEXT_BOX_COLOR_HIGHLIGHTED);

            if (mouseOnText(nameBox))
            {
                if (letterCount < MAX_INPUT_CHARS)
                {
                    // Draw blinking underscore char
                    if (((framesCounter/20)%2) == 0) DrawText("_", (int)nameBox.x + 8 + MeasureText(name, 40), (int)nameBox.y + 12, 40, TEXT_COLOR);
                }
                else DrawText("Press BACKSPACE to delete chars...", 230, 300, 20, TEXT_BOX_COLOR);
            }

    EndDrawing();
}

//GameScreen functions
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
        int x_pos = (GetScreenWidth()/2) + (TEXT_BOX_SIZE * 3/2 * (i)) - (TEXT_BOX_SIZE * 3/4 * current_word.length() - TEXT_BOX_SIZE/4);

        if (i == typing_index){ //highlighted letter textbox
            text.push_back(TextBox{x_pos, 280, current_word[i], font, TEXT_BOX_COLOR_HIGHLIGHTED});
            DrawRectangleLinesEx(Rectangle{(float) x_pos - 4, 276.0f, TEXT_BOX_SIZE + 8.0f,  TEXT_BOX_SIZE + 8.0f}, 2, Color{143, 139, 231, 255});
        
        } else { //normal letter textbox
            text.push_back(TextBox{x_pos, 300, current_word[i], font, TEXT_BOX_COLOR});
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

void GameScreen::drawScore(std::vector<Texture2D> textures, Font font) {
    BeginDrawing();
        ClearBackground(WHITE);

        //bg
        DrawTexture(textures[2], backgroundPos, backgroundPos - 8, Color{255, 255, 255, 75});
        
        //bg rectangle
        DrawRectangle(
            (width/2) - ((MeasureText(TextFormat("m i s t a k e s: %d", incorrectLetters), 40) + 80)/2),
            height/2 - 200,
            (MeasureText(TextFormat("m i s t a k e s: %d", incorrectLetters), 40)) + 80,
            height/2 + 40, TEXT_BOX_COLOR
        );

        DrawTextEx(font, 
            TextFormat("S C O R E"),
            XYtoVector2((width/2) - ((MeasureText(TextFormat("S C O R E"), 60) - 120)/2),
            height/2 - 140), 60, TEXT_SPACING, WHITE
        );

        //wpm
        DrawTextEx(font, 
            TextFormat("w p m: %d", wpm),
            XYtoVector2((width/2) - (((MeasureText(TextFormat("w p m: %d", wpm), 40) - 40)/2)),
            height/2 - 80), 40, TEXT_SPACING, TEXT_COLOR
        );

        //incorrect words
        DrawTextEx(font, 
            TextFormat("m i s t a k e s: %d", incorrectLetters),
            XYtoVector2((width/2) - (((MeasureText(TextFormat("m i s t a k e s: %d", incorrectLetters), 40) - 80)/2)),
            height/2 - 20), 40, TEXT_SPACING, TEXT_COLOR
        );

        //count
        DrawTextEx(font, 
            TextFormat("s c o r e: %d", wordTyped),
            XYtoVector2((width/2) - (((MeasureText(TextFormat("s c o r e: %d", wordTyped), 40)) - 80)/2),
            height/2 + 40), 40, TEXT_SPACING, TEXT_COLOR
        );

        DrawRectangleRec(buttonNext, buttonClicked(buttonNext) ? TEXT_COLOR : WHITE);
        DrawText("Next", buttonNext.x + 80, buttonNext.y + 15, 20, buttonClicked(buttonNext) ? WHITE : TEXT_COLOR);
        DrawRectangleLinesEx(buttonNext, 2, Color{0, 0, 0, 255});

    EndDrawing();
}

void GameScreen::drawCharacterSelection(std::vector<Texture2D> textures, Font font){
    BeginDrawing();
    ClearBackground(Color{245, 240, 255, 255});

    //background
    DrawTexture(textures[2], backgroundPos, backgroundPos - 8, Color{255, 255, 255, 75});

    //stage
    DrawTexture(textures[3], 0, 0, WHITE);

    //text
    DrawTextEx(font, TextFormat("- SELECT YOUR TYPING FRIEND -"),
        Vector2{width/2.0f - MeasureTextEx(font, TextFormat("- SELECT YOUR TYPING FRIEND -"), 36, 2).x/2.0f, 385},
        36, TEXT_SPACING, WHITE);

    source = Rectangle{idleIndex * TEXT_BOX_SIZE * 1.0f, 0.0f, 60.0f, 60.0f};

    //rabbit
    DrawTexturePro(textures[0], source, Rectangle{width/2.0f-270.0f, 125, source.width * 4, source.height * 4}, Vector2{0, 0}, 0, WHITE);

    //bear
    DrawTexturePro(textures[1], source, Rectangle{width/2.0f+30.0f, 125, source.width * 4, source.height * 4}, Vector2{0, 0}, 0, WHITE);
    EndDrawing();
}

//Typing Trials functions
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
    DrawTexture(textures[2], backgroundPos, backgroundPos - 8, Color{255, 255, 255, 75});

    //stage
    DrawTexture(textures[3], 0, 0, WHITE);

    //timer bar
    DrawRectangleLines(width/2 - 180, 420, 368, 28, WHITE);
    DrawRectangle(width/2 - 176, 424, frames / 10, 20, WHITE);

    //next word
    DrawTextEx(font, 
        TextFormat("Next Word: %s", nextWord.c_str()),
        XYtoVector2(width/2 - MeasureText(TextFormat("Next Word: %s", nextWord.c_str()), 20)/2, height/2 + 25),
        25, TEXT_SPACING, WHITE
    );
    //wpm
    DrawTextEx(font, TextFormat("WPM: %d", wpm), XYtoVector2(width/2 - 180, height/2 + 100), 25, TEXT_SPACING, WHITE);
    //count
    DrawTextEx(font, 
        TextFormat("Score: %d", wordTyped),
        XYtoVector2(width/2 + 188 - MeasureText(TextFormat("Score: %d", wordTyped), 20),
        height/2 + 100),
        25, TEXT_SPACING, WHITE
    );

    source = Rectangle{idleIndex * TEXT_BOX_SIZE * 1.0f, 0.0f, 60.0f, 60.0f};

    DrawWordOnScreen(currentWord, typingIndex, font);

    DrawTextureRec(textures[characterIndex], source, Vector2{
        ((GetScreenWidth()/2) + (TEXT_BOX_SIZE * 3/2 * (typingIndex)) - (TEXT_BOX_SIZE * 3/4 * currentWord.length() - TEXT_BOX_SIZE/4) * 1.0f)
        , 220}, WHITE);
    EndDrawing();
}

void TypingTrials::update(char key, std::vector<Sound> sounds) {
    framesCount();
    if (typedLetter(key)) {
        ++typingIndex;
        ++lettersTyped;
    }
    else if (key > 0 && !timesUp()) {
        ++incorrectLetters;
    }

    if(typingIndex >= currentWord.size()) {
        PlaySound(sounds[1]);
        typedWord();
    } 
}

void TypingTrials::reset() {
    frames = FRAME;
    wpm = 0;
    typingIndex = 0;
    wordTyped = 0;
    incorrectLetters = 0;
    lettersTyped = 0;
    characterSelectMenu = true;
    currentWord = getRandomWord(wordPool);
    nextWord = getRandomWord(wordPool);
}

//Ticking Time Bomb functions
TickingTimeBomb::TickingTimeBomb() : GameScreen(){
    frames = FRAME;
}

void TickingTimeBomb::reset() {
    frames = FRAME;
    wpm = 0;
    typingIndex = 0;
    wordTyped = 0;
    incorrectLetters = 0;
    lettersTyped = 0;
    characterSelectMenu = true;
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

void TickingTimeBomb::update(char key, std::vector<Sound> sounds){
    framesCount();
    if (typedLetter(key)) {
        ++lettersTyped;
        ++typingIndex;
    }
    else if (key > 0 && !timesUp()) {
        ++incorrectLetters;
    }
    if(typingIndex >= currentWord.size()) {
        PlaySound(sounds[1]);
        typedWord();
        frames = FRAME;
    } 

}

void TickingTimeBomb::draw(std::vector<Texture2D> textures, Font font){
    BeginDrawing();
    ClearBackground(Color{245, 240, 255, 255});

    //background
    DrawTexture(textures[2], backgroundPos, backgroundPos - 8, Color{255, 255, 255, 75});

    //stage
    DrawTexture(textures[3], 0, 0, WHITE);

    //timer bar
    DrawRectangleLines(width/2 - 180, 420, 368, 28, WHITE);
    DrawRectangle(width/2 - 176, 424, frames - 120, 20, WHITE);

    //next word
    DrawTextEx(font, 
        TextFormat("Next Word: %s", nextWord.c_str()),
        XYtoVector2(width/2 - MeasureText(TextFormat("Next Word: %s", nextWord.c_str()), 20)/2, height/2 + 25),
        25, TEXT_SPACING, WHITE
    );
    //wpm
    DrawTextEx(font, TextFormat("WPM: %d", wpm), XYtoVector2(width/2 - 180, height/2 + 100), 25, TEXT_SPACING, WHITE);
    //count
    DrawTextEx(font, 
        TextFormat("Score: %d", wordTyped),
        XYtoVector2(width/2 + 188 - (MeasureText(TextFormat("Score: %d", wordTyped), 25)/2),
        height/2 + 100),
        25, TEXT_SPACING, WHITE
    );

    source = Rectangle{idleIndex * TEXT_BOX_SIZE * 1.0f, 0.0f, 60.0f, 60.0f};

    DrawWordOnScreen(currentWord, typingIndex, font);

    DrawTextureRec(textures[characterIndex], source, Vector2{
        ((GetScreenWidth()/2) + (TEXT_BOX_SIZE * 3/2 * (typingIndex)) - (TEXT_BOX_SIZE * 3/4 * currentWord.length() - TEXT_BOX_SIZE/4) * 1.0f)
        , 220}, WHITE);
    EndDrawing();
}

char key;

void Game::Tick(std::vector<MainScreen>& mains, std::vector<GameScreen*>& modes) {
//Update all
    UpdateMusicStream(music);

    //bg update
    if (backgroundPos > -712){
        backgroundPos--;
    } else {
        backgroundPos = 0;
    }

    GameScreen* tt = modes[0];
    GameScreen* ttb = modes[1];
    MainScreen mainMenu = mains[0];
    MainScreen end = mains[1];

    switch (gameState) {
        case titleScreen:
        // Update ----------------------------------------------------------------------------------
            //buttons
            if(mainMenu.mouseOnText(mainMenu.getNameBox())) {
                mainMenu.typingName();
            } else {
                SetMouseCursor(MOUSE_CURSOR_DEFAULT);
            }
            if(mainMenu.buttonClicked(mainMenu.getButton1())){
                PlaySound(sounds[3]);
                gameState = mode1;
                tt->reset();
            }
            if(mainMenu.buttonClicked(mainMenu.getButton2())){
                PlaySound(sounds[3]);
                gameState = mode2;
                ttb->reset();
            }
            //forcequit
            // if (IsKeyPressed(KEY_LEFT_CONTROL)) {
            //     gameState = endScreen;
            // }
        // Drawing ---------------------------------------------------------------------------------
            mainMenu.draw(textures, font);
            break;

        case mode1:
        // Update ----------------------------------------------------------------------------------
            tt->update(GetCharPressed(), sounds);

            //timer: 60 seconds (60fps)
            if (tt->timesUp() && !tt->getCharacterSelectMenu()){
                if (tt->buttonClicked(tt->getButtonNext())) {
                    gameState = endScreen;
                    PlaySound(sounds[2]);
                }
                tt->drawScore(textures, font);

            } else if (!tt->getCharacterSelectMenu()){
                tt->draw(textures, font);
                
            } else {
                //character selection
                if (tt->buttonClicked(Rectangle{width/2.0f-270.0f, 125, source.width * 4, source.height * 4})){
                    PlaySound(sounds[3]);
                    tt->setCharacterIndex(0);
                    tt->setCharacterSelectMenu(false);
                    tt->resetFrames();
                }
                if (tt->buttonClicked(Rectangle{width/2.0f+30.0f, 125, source.width * 4, source.height * 4})){
                    PlaySound(sounds[3]);
                    tt->setCharacterIndex(1);
                    tt->setCharacterSelectMenu(false);
                    tt->resetFrames();
                }
                tt->drawCharacterSelection(textures, font);
            }

            //character animation
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
            ttb->update(GetCharPressed(), sounds);
            std::cout << ttb->getFrames() % 30 << "\n";

            //timer
            if (ttb->timesUp() && !ttb->getCharacterSelectMenu()){
                if (ttb->buttonClicked(ttb->getButtonNext())) {
                    gameState = endScreen;
                    PlaySound(sounds[2]);
                }
                ttb->drawScore(textures, font);

            } else if (!ttb->getCharacterSelectMenu()){
                ttb->draw(textures, font);
                
            } else {
                //character selection
                if (ttb->buttonClicked(Rectangle{width/2.0f-270.0f, 125, source.width * 4, source.height * 4})){
                    PlaySound(sounds[3]);
                    ttb->setCharacterIndex(0);
                    ttb->setCharacterSelectMenu(false);
                    ttb->resetFrames();
                }
                if (ttb->buttonClicked(Rectangle{width/2.0f+30.0f, 125, source.width * 4, source.height * 4})){
                    PlaySound(sounds[3]);
                    ttb->setCharacterIndex(1);
                    ttb->setCharacterSelectMenu(false);
                    ttb->resetFrames();
                }
                ttb->drawCharacterSelection(textures, font);
            }

            //character animation
            if (ttb->getFrames() % 30 == 0 && ttb->getIdleIndex() < 1){
                ttb->setIdleIndex(1);
            } else if (ttb->getFrames() % 30 == 0 && ttb->getIdleIndex() > 0){
                ttb->setIdleIndex(0);
            }

            //force quit
            if (IsKeyPressed(KEY_LEFT_CONTROL)) gameState = endScreen;

            break;
        
        case endScreen:
            if (end.buttonClicked(end.getButton1())){
                PlaySound(sounds[3]);
                gameState = mode1;
                tt->reset();
                // tt->setCurrentWord(modes[0]->getRandomWord(word_pool));
                // tt->setNextWord(modes[0]->getRandomWord(word_pool));
            }
            if(end.buttonClicked(end.getButton2())){
                PlaySound(sounds[3]);
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

