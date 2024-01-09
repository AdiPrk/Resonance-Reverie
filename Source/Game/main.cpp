#include <Engine/dogpch.h>
#include "Scenes/titleScene.h"

int main() {
    // Create and Run Application!
    Dog::DogEngine Engine(WINDOW_WIDTH, WINDOW_HEIGHT);
    return Engine.Run(TitleScene::GetInstance());
}