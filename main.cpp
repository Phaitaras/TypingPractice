#include "game.hpp"
using namespace std;

int main(){
    const int screenWidth = 1280;
    const int screenHeight = 720;

    Game game{screenWidth, screenHeight, "Typing Game"};

    std::cout << GetScreenWidth();

    while (!game.GameShouldClose()){
        game.Tick();
    }

    return 0;
}