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
        modes.push_back(&typingTrials);

        while (!game.GameShouldClose()){
            game.Tick(mains, modes);
        }

        return 0;
}