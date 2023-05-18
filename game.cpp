#include "game.hpp"

Game::Game(int width, int height, std::string title){
    assert(!GetWindowHandle()); //Check if window has not been created
    InitWindow(width, height, title.c_str());
    SetTargetFPS(60);
}

Game::~Game() noexcept {
    assert(GetWindowHandle()); //Check if there is a window
    CloseWindow();
}

bool Game::GameShouldClose() const{
    return WindowShouldClose();
}

void Game::Tick(){
    BeginDrawing();
    Update();
    Draw();
    EndDrawing();
}

void Game::Update(){

}

void Game::Draw(){
    ClearBackground(RAYWHITE);
}

