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
    icon = LoadImage("../assets/sprites/icon2.png");
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
    mouseonText = false;
    if (CheckCollisionPointRec(GetMousePosition(), textbox)) mouseonText = true;
    else mouseonText = false;
    return mouseonText;
}

void MainScreen::typingName(){
    if (mouseOnText(nameBox)) {
        SetMouseCursor(MOUSE_CURSOR_IBEAM);
        int key = GetCharPressed();

        while (key > 0) {
            std::cout << "key: " << key << std::endl;
            if ((key >= 32) && (key <= 125) && (letterCount < MAX_INPUT_CHARS)) {
                name[letterCount] = (char)key;
                name[letterCount + 1] = '\0'; // Add null terminator at the end of the string.
                letterCount++;
                std::cout << "name: " << name << std::endl;
            }
            key = GetCharPressed(); // Check next character in the queue
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            letterCount--;
            if (letterCount < 0) letterCount = 0;
            name[letterCount] = '\0';
        }
    } else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }

    if (mouseOnText(nameBox)) {
        framesCounter++;
    } else {
        framesCounter = 0;
    }
}

void MainScreen::setName(){
    finalName = std::string(name);
}

std::string MainScreen::getName() {
  if (name != nullptr && std::strlen(name) > 0) {
    return finalName;
  } else {
    return "Unknown";
  }
}

void MainScreen::draw(std::vector<Texture2D> textures, Font font) {
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

}

void MainScreen::drawNameBox(){
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

    DrawRectangleRec(confirmNameBox, buttonClicked(confirmNameBox) ? WHITE : TEXT_COLOR);
    DrawText("->", confirmNameBox.x + 10, confirmNameBox.y + 10, 20, buttonClicked(confirmNameBox) ? TEXT_COLOR : WHITE);
}


json MainScreen::loadJsonFile() {
    std::ifstream file("../scorefile.json");
    if (!file) {
        std::cerr << "Error opening file.\n";
    }

    json jsonData;
    file >> jsonData;

    return jsonData;
}

void MainScreen::saveJsonFile(const json& jsonData) {
    std::ofstream outputFile("../scorefile.json");
    if (!outputFile) {
        std::cerr << "Error opening file for writing.\n";
    }

    outputFile << std::setw(4) << jsonData << std::endl;
    std::cout << "File updated successfully.\n";
}

void MainScreen::addPlayerName(GameScreen* gameScreen, std::string mode) {
    json newPlayer = {
        {"name", getName()},
    };

    json jsonData = loadJsonFile();
    json& playerData = jsonData[mode]["player"];

    playerData.push_back(newPlayer);

    saveJsonFile(jsonData);
}

void MainScreen::addPlayerData(GameScreen* gameScreen, std::string mode) {
    json jsonData = loadJsonFile();
    json& playerData = jsonData[mode]["player"];

    // Find the player's entry based on their name
    std::string playerName = getName();
    bool foundPlayer = false;

    for (json& player : playerData) {
        if (player["name"] == playerName && !player.contains("score") && !player.contains("wpm")) {
            player["score"] = gameScreen->getWordsTyped();
            player["wpm"] = gameScreen->getWPM();
            foundPlayer = true;
            break; 
        }
    }

    if (!foundPlayer) {
        json newPlayer = {
            {"name", playerName},
            {"score", gameScreen->getWordsTyped()},
            {"wpm", gameScreen->getWPM()}
        };
        playerData.push_back(newPlayer);
    }

    std::sort(playerData.begin(), playerData.end(), SortByWPMDesc());
    saveJsonFile(jsonData);
}




void MainScreen::drawScoreBoard(std::vector<Texture2D> textures, Font font,std::string mode) {
    json jsonData = loadJsonFile();
    json playerData = jsonData[mode]["player"];
    int numPlayers = std::min(10, static_cast<int>(playerData.size()));
    std::string modetype = "Mode: " + mode;

    BeginDrawing();
        ClearBackground(WHITE);
        DrawTexture(textures[2], backgroundPos, backgroundPos - 8, Color{255, 255, 255, 75});
        DrawRectangleRec(buttonBack, buttonClicked(buttonBack) ? WHITE : TEXT_COLOR);
        DrawText("<--", buttonBack.x + 10, buttonBack.y + 10, 20, buttonClicked(buttonBack) ? TEXT_COLOR : WHITE);

        int leaderboardWidth = 600;
        int leaderboardHeight = (numPlayers + 1) * 30 + 50; 

        int leaderboardX = width / 2.0f - leaderboardWidth / 2;
        int leaderboardY = height / 2.0f - leaderboardHeight / 2;

        Rectangle leaderboardRect{leaderboardX, leaderboardY, leaderboardWidth, leaderboardHeight};

        // Draw the background rectangle for the leaderboard
        DrawRectangleRec(leaderboardRect, TEXT_COLOR);

        // Draw the headers
        DrawText("Top Ten Players", leaderboardX + 10, leaderboardY + 10, 20, WHITE);
        DrawText(modetype.c_str(), leaderboardX + 300, leaderboardY + 10, 20, WHITE);
        DrawText("Name", leaderboardX + 50, leaderboardY + 50, 20, WHITE);
        DrawText("Score", leaderboardX + 300, leaderboardY + 50, 20, WHITE);
        DrawText("WPM", leaderboardX + 500, leaderboardY + 50, 20, WHITE);

        // Draw the player entries
        for (int i = 0; i < numPlayers; i++) {
            std::string name = playerData[i]["name"];
            int score = playerData[i]["score"];
            int wpm = playerData[i]["wpm"];

            int yPos = leaderboardY + 80 + i * 30;

            // Use alternating colors for odd and even rows
            Color rowColor = (i % 2 == 0) ? TEXT_BOX_COLOR : TEXT_BOX_COLOR_HIGHLIGHTED;

            // Draw the background rectangle for the row
            DrawRectangle(leaderboardX + 10, yPos, leaderboardWidth - 20, 30, rowColor);

            // Draw the player information
            DrawText(name.c_str(), leaderboardX + 50, yPos + 5, 20, BLACK);
            DrawText(std::to_string(score).c_str(), leaderboardX + 300, yPos + 5, 20, BLACK);
            DrawText(std::to_string(wpm).c_str(), leaderboardX + 500, yPos + 5, 20, BLACK);
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

        DrawRectangleRec(buttonScoreBoard, buttonClicked(buttonScoreBoard) ? WHITE : TEXT_COLOR);
        DrawText("Scoreboard", buttonScoreBoard.x + 40, buttonScoreBoard.y + 15, 20, buttonClicked(buttonScoreBoard) ? TEXT_COLOR : WHITE);
        DrawRectangleLinesEx(buttonScoreBoard, 2, Color{0, 0, 0, 255});

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
    wpm = (int)wordTyped * 3600 / (FRAME - frames);
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
        PlaySound(sounds[0]);
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
    totalframes = 0;
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
    wpm = (int)wordTyped * 3600 / totalframes;
    frames = FRAME;
    typingIndex = 0;
    setCurrentWord(nextWord);
    setNextWord(getRandomWord(wordPool));
}

void TickingTimeBomb::update(char key, std::vector<Sound> sounds){
    framesCount();
    if (typedLetter(key)) {
        PlaySound(sounds[0]);
        ++lettersTyped;
        ++typingIndex;
    } else if (key > 0 && !timesUp()) {
        ++incorrectLetters;
    }

    if (typingIndex >= currentWord.size()) {
        PlaySound(sounds[1]);
        typedWord();
    }

    if (frames == 0){ //last wpm calculation before ending
        wpm = (int)wordTyped * 3600 / totalframes;
    }
}

void TickingTimeBomb::draw(std::vector<Texture2D> textures, Font font){
    addTotalFrames();
    BeginDrawing();
    ClearBackground(Color{245, 240, 255, 255});

    //background
    DrawTexture(textures[2], backgroundPos, backgroundPos - 8, Color{255, 255, 255, 75});

    //stage
    DrawTexture(textures[3], 0, 0, WHITE);

    //timer bar
    DrawRectangleLines(width/2 - 180, 420, 368, 28, WHITE);
    DrawRectangle(width/2 - 176, 424, frames + 5, 20, WHITE);

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

PracticeMode::PracticeMode() : GameScreen(){}

void PracticeMode::reset(){
    frames = 0;
    wpm = 0;
    typingIndex = 0;
    wordTyped = 0;
    incorrectLetters = 0;
    lettersTyped = 0;
    characterSelectMenu = true;
    currentWord = getRandomWord(wordPool);
    nextWord = getRandomWord(wordPool);
}

void PracticeMode::update(char key, std::vector<Sound> sounds){
    framesCount();
    if (typedLetter(key)) {
        PlaySound(sounds[0]);
        ++lettersTyped;
        ++typingIndex;
    }
    else if (key > 0 && !timesUp()) {
        ++incorrectLetters;
    }
    if(typingIndex >= currentWord.size()) {
        PlaySound(sounds[1]);
        typedWord();
    } 
}

void PracticeMode::typedWord(){
    ++wordTyped;
    typingIndex = 0;
    setCurrentWord(nextWord);
    setNextWord(getRandomWord(wordPool));
}

void PracticeMode::draw(std::vector<Texture2D> textures, Font font){
    BeginDrawing();
    ClearBackground(Color{245, 240, 255, 255});

    //background
    DrawTexture(textures[2], backgroundPos, backgroundPos - 8, Color{255, 255, 255, 75});

    DrawRectangleRec(buttonBack, buttonClicked(buttonBack) ? WHITE : TEXT_COLOR);
    DrawText("back", buttonBack.x + 10, buttonBack.y + 10, 20, buttonClicked(buttonBack) ? TEXT_COLOR : WHITE);

    //stage
    DrawTexture(textures[3], 0, 0, WHITE);

    //next word
    DrawTextEx(font, 
        TextFormat("Next Word: %s", nextWord.c_str()),
        XYtoVector2(width/2 - MeasureText(TextFormat("Next Word: %s", nextWord.c_str()), 20)/2, height/2 + 25),
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

void Game::Tick(std::vector<MainScreen*>& mains, std::vector<GameScreen*>& modes) {
    //Update all
    GameScreen* tt = modes[0];
    GameScreen* ttb = modes[1];
    GameScreen* ptm = modes[2];
    MainScreen* mainMenu = mains[0];
    MainScreen* end = mains[1];

    UpdateMusicStream(music);
    
    //bg update
    if (backgroundPos > -712){
        backgroundPos--;
    } else {
        backgroundPos = 0;
    }

    switch (gameState) {
        case titleScreen:
        // Update ----------------------------------------------------------------------------------
            //buttons
            mainMenu->typingName();
            if(mainMenu->buttonClicked(mainMenu->getConfirmBox())){
                mainMenu->setName();
            }
            if(mainMenu->buttonClicked(mainMenu->getButton1())){
                gameState = mode1;
                PlaySound(sounds[3]);
                mainMenu->addPlayerName(tt, "TypingTrials");
                tt->reset();
            }
            if(mainMenu->buttonClicked(mainMenu->getButton2())){
                gameState = mode2;
                PlaySound(sounds[3]);
                mainMenu->addPlayerName(ttb, "TickingTimeBomb");
                ttb->reset();
            }
            if(mainMenu->buttonClicked(mainMenu->getPracticeButton())){
                gameState = practicemode;
            }
            //forcequit
            // if (IsKeyPressed(KEY_LEFT_CONTROL)) {
            //     gameState = endScreen;
            // }
        // Drawing ---------------------------------------------------------------------------------
            BeginDrawing();
                mainMenu->drawNameBox();
                mainMenu->draw(textures, font);
            EndDrawing();
            break;

        case mode1:
        // Update ----------------------------------------------------------------------------------
            tt->update(GetCharPressed(), sounds);

            //timer: 60 seconds (60fps)
            if (tt->timesUp() && !tt->getCharacterSelectMenu()){
                if (tt->buttonClicked(tt->getButtonNext()))  gameState = endScreen;
                    PlaySound(sounds[2]);
                if (tt->buttonClicked(tt->getButtonScoreBoard())){
                    mainMenu->addPlayerData(tt, "TypingTrials");
                    gameState = scoreBoard1;
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

        case scoreBoard1:
            mainMenu->drawScoreBoard(textures, font, "TypingTrials");
            if(mainMenu->buttonClicked(mainMenu->getButtonBack())) gameState = mode1;
            break;

        case mode2:
             // Update ----------------------------------------------------------------------------------
            ttb->update(GetCharPressed(), sounds);

            //timer
            if (ttb->timesUp() && !ttb->getCharacterSelectMenu()){
                if (ttb->buttonClicked(ttb->getButtonNext())) gameState = endScreen;
                PlaySound(sounds[2]);
                if (ttb->buttonClicked(ttb->getButtonScoreBoard())){
                    mainMenu->addPlayerData(ttb, "TickingTimeBomb");
                    gameState = scoreBoard2;
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
        
        case scoreBoard2:
            mainMenu->drawScoreBoard(textures, font, "TickingTimeBomb");
            if(mainMenu->buttonClicked(mainMenu->getButtonBack())) gameState = mode2;
            break;

        case practicemode:
            ptm->update(GetCharPressed(), sounds);

            //timer
            if (!ptm->getCharacterSelectMenu()){
                if (ptm->buttonClicked(ptm->getButtonBack())){ 
                    gameState = titleScreen;
                    ptm->reset();
                }
                ptm->draw(textures, font);   
            } else {
                //character selection
                if (ptm->buttonClicked(Rectangle{width/2.0f-270.0f, 125, source.width * 4, source.height * 4})){
                    PlaySound(sounds[3]);
                    ptm->setCharacterIndex(0);
                    ptm->setCharacterSelectMenu(false);
                    ptm->resetFrames();
                }
                if (ptm->buttonClicked(Rectangle{width/2.0f+30.0f, 125, source.width * 4, source.height * 4})){
                    PlaySound(sounds[3]);
                    ptm->setCharacterIndex(1);
                    ptm->setCharacterSelectMenu(false);
                    ptm->resetFrames();
                }
                ptm->drawCharacterSelection(textures, font);
            }

            //character animation
            if (ptm->getFrames() % 30 == 0 && ptm->getIdleIndex() < 1){
                ptm->setIdleIndex(1);
            } else if (ptm->getFrames() % 30 == 0 && ptm->getIdleIndex() > 0){
                ptm->setIdleIndex(0);
            }

            //force quit
            if (IsKeyPressed(KEY_LEFT_CONTROL)) gameState = endScreen;

            break;

        case endScreen:
            if (end->buttonClicked(end->getButton1())){
                PlaySound(sounds[3]);
                gameState = mode1;
                tt->reset();

            }
            if(end->buttonClicked(end->getButton2())){
                PlaySound(sounds[3]);
                gameState = mode2;
                ttb->reset();
            }
            if(end->buttonClicked(end->getPracticeButton())){
                PlaySound(sounds[3]);
                gameState = practicemode;
                ptm->reset();
            }
        // Update ----------------------------------------------------------------------------------
            if (IsKeyPressed(KEY_A)) {
                gameState = titleScreen;
            }

        // Drawing ---------------------------------------------------------------------------------
            BeginDrawing();
                end->draw(textures, font);
            EndDrawing();
            break;
    }  
}

