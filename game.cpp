#include "game.hpp"

Game::Game(int width, int height, std::string title): width(width), height(height), gameState(titleScreen){
    assert(!GetWindowHandle()); //Check if window has not been created
    InitWindow(width, height, title.c_str());
    SetTargetFPS(60);
    this->word_pool = {"miki", "music", "putter", "monkey", "pooper", "fard", "ball", "bob", "drake"};
}

Game::~Game() noexcept {
    assert(GetWindowHandle()); //Check if there is a window
    CloseWindow();
}

bool Game::GameShouldClose() const {
    return WindowShouldClose();
}



//textbox themes
const int textBox_size = 60;
const Color textBox_color = Color{186, 255, 204, 255};
const Color textBox_color_highlighted = Color{166, 247, 187, 255};

TextBox::TextBox(int x, int y, char letter, Color color): letter(letter){
    DrawRectangle(x, y, textBox_size, textBox_size, color);
    DrawText(std::string(1, letter).c_str(), x-7+textBox_size/2, y-15+textBox_size/2, textBox_size/2, BLACK);
}

void Game::DrawWordOnScreen(std::string current_word, int typing_index){
    std::vector<TextBox> text;

    for (int i = 0; i < current_word.size(); i++){
        int x_pos = (GetScreenWidth()/2) + (textBox_size * 3/2 * (i)) - (textBox_size * 3/4 * current_word.length() - textBox_size/4);

        if (i == typing_index){
            text.push_back(TextBox{x_pos, 280, current_word[i], textBox_color_highlighted});
            //border decoration it doesnt work wtf
            //DrawRectangleLines(x_pos, 280, textBox_size + 2, textBox_size + 2, Color{84, 218, 119});
        
        } else {
            text.push_back(TextBox{x_pos, 300, current_word[i], textBox_color});
        }
    }
}

std::string test;
std::string current_word, next_word;
int typing_index;
int key;
bool correctInput;

void Game::Tick(){
    //Update all
    //...

    switch (gameState){
        case titleScreen:
        // Update ----------------------------------------------------------------------------------
            test = "title screen, space to go game";
            if (IsKeyPressed(KEY_SPACE)){
                gameState = gameScreen;
                typing_index = 0;
                current_word = word_pool[GetRandomValue(0, word_pool.size() - 1)];
                next_word = word_pool[GetRandomValue(0, word_pool.size() - 1)];
            }

        // Drawing ---------------------------------------------------------------------------------
            BeginDrawing();
                ClearBackground(WHITE);
                DrawText(test.c_str(), width/2, height/2, 20, BLACK);
            EndDrawing();
            break;


        case gameScreen:
        // Update ----------------------------------------------------------------------------------
            test = "game screen";
            key = GetCharPressed();

            //check if key input = index of the current letter
            if (key == current_word[typing_index] || key == current_word[typing_index] - 32){
                typing_index++;
            }

            //reset index and random a new word
            if (typing_index >= current_word.size()){
                typing_index = 0;
                current_word = next_word;
                next_word = word_pool[GetRandomValue(0, word_pool.size() - 1)];
            }

        // Drawing ---------------------------------------------------------------------------------
            BeginDrawing();
                ClearBackground(Color{210, 248, 249});
                DrawRectangle(0, height/2 - 10, 1280, 20, Color{134, 234, 167, 255});
                DrawRectangle(0, height/2 + 10, 1280, 500, Color{115, 216, 149, 255});
                DrawRectangle(width/2 - 1, height/2 - 1, 2, 2, BLACK);
                
                DrawText(
                    TextFormat("Next Word: %s", next_word.c_str()),
                    width/2 - MeasureText(TextFormat("Next Word: %s", next_word.c_str()), 20)/2,
                    height/2 + 30, 20, BLACK
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
