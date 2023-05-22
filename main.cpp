#include "game.hpp"
using namespace std;

// for convenience
int main(){
        const int screenWidth = 1280;
        const int screenHeight = 720;

        GameScreen game{"Typing Game"};

        while (!game.GameScreenShouldClose()){
            game.Tick();
        }

        return 0;
}