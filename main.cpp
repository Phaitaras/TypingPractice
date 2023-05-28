#include "game.hpp"
using namespace std;


// for convenience
int main(){

    const int screenWidth = 1280;
    const int screenHeight = 720;
    const std::string title = "FUZZY TYPERS";
    MainScreen startingScreen = MainScreen(title);
    TypingTrials typingTrials = TypingTrials();
    TickingTimeBomb tickingTimeBomb = TickingTimeBomb();
    PracticeMode practicemode = PracticeMode();
    MainScreen endingScreen = MainScreen("RETRY ?");

    Game game{screenWidth, screenHeight, title};
    std::vector<MainScreen*> mains;
    mains.push_back(&startingScreen);
    mains.push_back(&endingScreen);

    std::vector<GameScreen*> modes;
    modes.push_back(&typingTrials);
    modes.push_back(&tickingTimeBomb);
    modes.push_back(&practicemode);

    while (!game.GameShouldClose()){
        game.Tick(mains, modes);
    }

    return 0;

}

