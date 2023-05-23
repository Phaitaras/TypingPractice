#include "game.hpp"

std::vector<std::string> json_vec(ns:: wordpool wp,std::string filename){
    std::ifstream i(filename);
    json json_file;
    i >> json_file;

    wp.language = json_file["name"];

    const auto& words = json_file["words"];
    for (const auto& word : words) {
        wp.words.push_back(word);
    }

    return wp.words;
}


Game::Game(int width, int height, std::string title): width(width), height(height), gameState(titleScreen){
    assert(!GetWindowHandle()); //Check if window has not been created
    InitWindow(width, height, title.c_str());
    SetTargetFPS(60);

    ns::wordpool eng;
    this->word_pool = json_vec(eng, "words.json");

}

Game::~Game() noexcept {
    assert(GetWindowHandle()); //Check if there is a window
    CloseWindow();
}

bool Game::GameShouldClose() const {
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

void Game::DrawWordOnScreen(std::string current_word, int typing_index){
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

std::string test;
std::string current_word, next_word;
int typing_index;
int key;
bool correctInput;
int wpm, word_typed, letters_typed, incorrect_letters;
int frames;

void Game::Tick(){
    //Update all
    //...

    switch (gameState){
        case titleScreen:
        // Update ----------------------------------------------------------------------------------
            test = "title screen, space to go game";

            if (IsKeyPressed(KEY_SPACE)){
                gameState = TypingTrials;
                typing_index, wpm, word_typed = 0;
                frames = 3600; //60 second
                current_word = word_pool[GetRandomValue(0, word_pool.size() - 1)];
                next_word = word_pool[GetRandomValue(0, word_pool.size() - 1)];
            }

        // Drawing ---------------------------------------------------------------------------------
            BeginDrawing();
                ClearBackground(WHITE);
                DrawText(test.c_str(), width/2, height/2, 20, textColor);
            EndDrawing();
            break;


        case TypingTrials:
        // Update ----------------------------------------------------------------------------------
            key = GetCharPressed();
            frames--;

            //timer: 60 seconds (60fps)
            if (frames <= 0){
                gameState = endScreen;
            }

            //force quit
            if (IsKeyPressed(KEY_LEFT_SHIFT)) gameState = endScreen;

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
            break;


        case endScreen:
        // Update ----------------------------------------------------------------------------------
            test = "end screen";

        // Drawing ---------------------------------------------------------------------------------
            BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText(test.c_str(), width/2, height/2, 20, BLACK);
            EndDrawing();
            break;
    }
}
