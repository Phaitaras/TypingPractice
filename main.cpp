#include "game.hpp"
#include <nlohmann/json.hpp>
using namespace std;
using json = nlohmann::json;

// for convenience
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