#pragma once
#include <Engine/dogpch.h>

class Game : public Dog::Application
{
public:
    Game(Dog::Window* window);
    ~Game();
        
    void Update(float dt) override;
    void Render(float dt, float ct, float itf) override;

private:  
    unsigned int m_Width;
    unsigned int m_Height;
    Dog::Window* m_Window;

    Dog::Scene m_ActiveScene;
};