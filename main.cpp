#include "game.hpp"
using namespace std;

// for convenience
int main(){
        const int screenWidth = 1280;
        const int screenHeight = 720;
        MainScreen startingScreen = MainScreen("SPACE");
        TypingTrials typingTrials = TypingTrials();
        MainScreen endingScreen = MainScreen("THE END");

        Game game{screenWidth, screenHeight, "Typing Game"};
        std::vector<MainScreen> mains = {startingScreen, endingScreen};
        std::vector<GameScreen*> modes;
        //Font font = LoadFontEx("../assets/dpcomic.ttf", 30, 0, 0);
        modes.push_back(&typingTrials);

        while (!game.GameShouldClose()){
            game.Tick(mains, modes);
            // BeginDrawing();
            // ClearBackground(WHITE);
            // DrawTextEx(font, "hello", Vector2{200.0f, 200.0f}, font.baseSize, 0, BLACK);
            // EndDrawing();

        }

        return 0;
}