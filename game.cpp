#include "game.hpp"

std::vector<std::string> json_vec(ns:: wordpool wp,std::string filename){
    std::ifstream i("../" + filename);
    json json_file;
    i >> json_file;

    wp.language = json_file["name"];

    const auto& words = json_file["words"];
    for (const auto& word : words) {
        wp.words.push_back(word);
    }

    return wp.words;
}


GameScreen::GameScreen(std::string title) {
    assert(!GetWindowHandle()); //Check if window has not been created
    titlereal = title;
    InitWindow(width, height, titlereal.c_str());
    SetTargetFPS(60);
    ns::wordpool eng;
    this->word_pool = json_vec(eng, "words.json");
}

GameScreen::~GameScreen() noexcept {
    assert(GetWindowHandle()); //Check if there is a window
    CloseWindow();
}

bool GameScreen::GameScreenShouldClose() const {
    return WindowShouldClose();
}

//themes
const int textBox_size = 60;
const Color textBox_color = Color{186, 255, 204, 255};
const Color textBox_color_highlighted = Color{166, 247, 187, 255};
const Color textColor = Color{44, 115, 62, 255};

TextBox::TextBox(int x, int y, char letter, Color color): letter(letter){
    DrawRectangle(x, y, textBox_size, textBox_size, color);
    DrawText(std::string(1, letter).c_str(), x-7+textBox_size/2, y-15+textBox_size/2, textBox_size/2, textColor);
}

void GameScreen::DrawWordOnScreen(std::string current_word, int typing_index){
    std::vector<TextBox> text;

    for (int i = 0; i < current_word.size(); i++){
        int x_pos = (GetScreenWidth()/2) + (textBox_size * 3/2 * (i)) - (textBox_size * 3/4 * current_word.length() - textBox_size/4);

        if (i == typing_index){ //highlighted letter textbox
            text.push_back(TextBox{x_pos, 280, current_word[i], textBox_color_highlighted});
            //border decoration it doesnt work wtf
            //DrawRectangleLines(x_pos, 280, textBox_size + 2, textBox_size + 2, Color{84, 218, 119});
        
        } else { //normal letter textbox
            text.push_back(TextBox{x_pos, 300, current_word[i], textBox_color});
        }
    }
}

std::string GameScreen::GetRandomWord() {
    return word_pool[GetRandomValue(0, word_pool.size() - 1)];
}

TypingTrials::TypingTrials(int Index, int Speed, int Typed, int Frames) {
    SetTargetFPS(60);

    ns::wordpool eng;
    this->word_pool = json_vec(eng, "words.json");
}

std::string test;
std::string current_word, next_word;
int typing_index;
int key;
bool correctInput;
int wpm, word_typed, letters_typed, incorrect_letters;
int frames;

void GameScreen::Tick(){
    //this is the ticking for the title screen/base class only
    // Update ----------------------------------------------------------------------------------
    test = "title screen, space to go game";
        //if press space typing trials start ticking instead
        if (IsKeyPressed(KEY_SPACE)){
            TypingTrials hello1(0, 0, 0, 3600);
            hello1.Tick();
        }

        // Drawing ---------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(WHITE);
            DrawText(test.c_str(), width/2, height/2, 20, textColor);
        EndDrawing();
}

void TypingTrials::Tick() {
    //     //Typing Trials
    //     // Update ----------------------------------------------------------------------------------
        key = GetCharPressed();
        frames--;

        //timer: 60 seconds (60fps)
        if (frames <= 0){
            EndScreen TheEnd;
            TheEnd.Tick();
        }

        //force quit
        if (IsKeyPressed(KEY_LEFT_SHIFT)) {
            EndScreen TheEnd;
            TheEnd.Tick();
        }
        //check if key input = index of the current letter
        if (key == current_word[typing_index] || key == current_word[typing_index] - 32){
            typing_index++;
            letters_typed++;
        } else if (key > 0){
            incorrect_letters++;
        }

        //reset index and random a new word
        if (typing_index >= current_word.size()){
            word_typed++;
            wpm = (int) word_typed * 3600 / (3600 - frames);
            typing_index = 0;
            current_word = next_word;
            next_word = word_pool[GetRandomValue(0, word_pool.size() - 1)];
        }

        // Drawing ---------------------------------------------------------------------------------
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
                TextFormat("Next Word: %s", next_word.c_str()),
                width/2 - MeasureText(TextFormat("Next Word: %s", next_word.c_str()), 20)/2,
                height/2 + 30, 20, textColor
            );
            //wpm
            DrawText(TextFormat("WPM: %d", wpm), width/2 - 180, height/2 + 100, 20, textColor);
            //count
            DrawText(
                TextFormat("Score: %d", word_typed),
                width/2 + 188 - MeasureText(TextFormat("Score: %d", word_typed), 20),
                height/2 + 100, 20, textColor
            );

            DrawWordOnScreen(current_word, typing_index);
            

        EndDrawing();
}

void EndScreen::Tick() {
    //endscreen
    // Update ----------------------------------------------------------------------------------
    test = "end screen";

    // Drawing ---------------------------------------------------------------------------------
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText(test.c_str(), width/2, height/2, 20, BLACK);
    EndDrawing();
}
