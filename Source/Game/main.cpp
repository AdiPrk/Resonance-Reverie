#include <Engine/engine.h>
#include "game.h"

int main() {
    // Create Engine and Game
    Dog::DogEngine Engine(1600, 900);
    Game ResonanceReverie(Engine.GetWindow());
    
    // Run Engine!
    return Engine.Run(&ResonanceReverie);
}