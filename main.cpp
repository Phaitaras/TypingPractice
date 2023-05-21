#include "game.hpp"
#include <nlohmann/json.hpp>
using namespace std;
using json = nlohmann::json;

// for convenience
int main(){
    const int screenWidth = 1280;
    const int screenHeight = 720;

    Game game{screenWidth, screenHeight, "Typing Game"};

    while (!game.GameShouldClose()){
        game.Tick();
    }

    /*.json file located in build folder*/
    std::ifstream i("test.json");
    json j;
    i >> j;
    std::cout << j;
    
    return 0;
}