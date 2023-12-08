#include <Engine/engine.h>
#include "game.h"

int main() {
    // Create Engine and Game
    DogEngine Engine(SCREEN_WIDTH, SCREEN_HEIGHT);
    Game ResonanceReverie(Engine.GetWindow());
    
    // Run Engine!
    return Engine.Run(&ResonanceReverie);
}