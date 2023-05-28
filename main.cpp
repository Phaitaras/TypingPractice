#include "game.hpp"
using namespace std;

bool sortByScoreDesc(const json& a, const json& b) {
    return a["wpm"] > b["wpm"];
}


// for convenience
int main(){

    const int screenWidth = 1280;
    const int screenHeight = 720;
    const std::string title = "FUZZY TYPERS";
    MainScreen startingScreen = MainScreen(title);
    TypingTrials typingTrials = TypingTrials();
    TickingTimeBomb tickingTimeBomb = TickingTimeBomb();
    MainScreen endingScreen = MainScreen("RETRY ?");

    Game game{screenWidth, screenHeight, title};
    std::vector<MainScreen*> mains;
    mains.push_back(&startingScreen);
    mains.push_back(&endingScreen);

    std::vector<GameScreen*> modes;
    modes.push_back(&typingTrials);
    modes.push_back(&tickingTimeBomb);

    while (!game.GameShouldClose()){
        game.Tick(mains, modes);
    }

    // std::ifstream file("../scorefile.json");
    // if (!file) {
    //     std::cerr << "Error opening file.\n";
    //     return 1;
    // }

    // json jsonData;
    // file >> jsonData;

    // // Modify the JSON data
    // json newPlayer = {
    //     {"name", "Alice"},
    //     {"score", 30},
    //     {"wpm", 35}
    // };
    // jsonData["TypingTrials"]["player"].push_back(newPlayer);

    //  // Write the modified JSON data back to the file
    // std::ofstream outputFile("data.json");
    // if (!outputFile) {
    //     std::cerr << "Error opening file for writing.\n";
    //     return 1;
    // }

    // outputFile << std::setw(4) << jsonData << std::endl;

    // std::cout << "File updated successfully.\n";

    // return 0;


    return 0;

}

