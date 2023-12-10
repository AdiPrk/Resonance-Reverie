#include <Engine/dogpch.h>
#include "Scenes/titleScene.h"

int main() {
    // Create and Run Application!
    Dog::DogEngine Engine(1280, 720);
    return Engine.Run(TitleScene::GetInstance());
}